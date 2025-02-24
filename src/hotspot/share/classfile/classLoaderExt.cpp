/*
 * Copyright (c) 2015, 2019, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#include "precompiled.hpp"
#include "classfile/classFileParser.hpp"
#include "classfile/classFileStream.hpp"
#include "classfile/classListParser.hpp"
#include "classfile/classLoader.inline.hpp"
#include "classfile/classLoaderExt.hpp"
#include "classfile/classLoaderData.inline.hpp"
#include "classfile/klassFactory.hpp"
#include "classfile/modules.hpp"
#include "classfile/sharedPathsMiscInfo.hpp"
#include "classfile/systemDictionaryShared.hpp"
#include "classfile/vmSymbols.hpp"
#include "memory/allocation.inline.hpp"
#include "memory/filemap.hpp"
#include "memory/resourceArea.hpp"
#include "oops/instanceKlass.hpp"
#include "oops/oop.inline.hpp"
#include "oops/symbol.hpp"
#include "runtime/arguments.hpp"
#include "runtime/handles.inline.hpp"
#include "runtime/java.hpp"
#include "runtime/javaCalls.hpp"
#include "runtime/os.hpp"
#include "services/threadService.hpp"
#include "utilities/stringUtils.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstring>
#include "gc/epsilon/RemoteSpace.hpp"
#include "gc/epsilon/rpcMessages.hpp"

jshort ClassLoaderExt::_app_class_paths_start_index = ClassLoaderExt::max_classpath_index;
jshort ClassLoaderExt::_app_module_paths_start_index = ClassLoaderExt::max_classpath_index;
jshort ClassLoaderExt::_max_used_path_index = 0;
bool ClassLoaderExt::_has_app_classes = false;
bool ClassLoaderExt::_has_platform_classes = false;

void ClassLoaderExt::append_boot_classpath(ClassPathEntry* new_entry) {
#if INCLUDE_CDS
  if (UseSharedSpaces) {
    warning("Sharing is only supported for boot loader classes because bootstrap classpath has been appended");
    FileMapInfo::current_info()->header()->set_has_platform_or_app_classes(false);
  }
#endif
  ClassLoader::add_to_boot_append_entries(new_entry);
}

void ClassLoaderExt::setup_app_search_path() {
  assert(DumpSharedSpaces, "this function is only used with -Xshare:dump");
  _app_class_paths_start_index = ClassLoader::num_boot_classpath_entries();
  char* app_class_path = os::strdup(Arguments::get_appclasspath());

  if (strcmp(app_class_path, ".") == 0) {
    // This doesn't make any sense, even for AppCDS, so let's skip it. We
    // don't want to throw an error here because -cp "." is usually assigned
    // by the launcher when classpath is not specified.
    trace_class_path("app loader class path (skipped)=", app_class_path);
  } else {
    trace_class_path("app loader class path=", app_class_path);
    shared_paths_misc_info()->add_app_classpath(app_class_path);
    ClassLoader::setup_app_search_path(app_class_path);
  }
}

void ClassLoaderExt::process_module_table(ModuleEntryTable* met, TRAPS) {
  ResourceMark rm(THREAD);
  for (int i = 0; i < met->table_size(); i++) {
    for (ModuleEntry* m = met->bucket(i); m != NULL;) {
      char* path = m->location()->as_C_string();
      if (strncmp(path, "file:", 5) == 0) {
        path = ClassLoader::skip_uri_protocol(path);
        ClassLoader::setup_module_search_path(path, THREAD);
      }
      m = m->next();
    }
  }
}
void ClassLoaderExt::setup_module_paths(TRAPS) {
  assert(DumpSharedSpaces, "this function is only used with -Xshare:dump");
  _app_module_paths_start_index = ClassLoader::num_boot_classpath_entries() +
                              ClassLoader::num_app_classpath_entries();
  Handle system_class_loader (THREAD, SystemDictionary::java_system_loader());
  ModuleEntryTable* met = Modules::get_module_entry_table(system_class_loader);
  process_module_table(met, THREAD);
}

char* ClassLoaderExt::read_manifest(ClassPathEntry* entry, jint *manifest_size, bool clean_text, TRAPS) {
  const char* name = "META-INF/MANIFEST.MF";
  char* manifest;
  jint size;

  assert(entry->is_jar_file(), "must be");
  manifest = (char*) ((ClassPathZipEntry*)entry )->open_entry(name, &size, true, CHECK_NULL);

  if (manifest == NULL) { // No Manifest
    *manifest_size = 0;
    return NULL;
  }


  if (clean_text) {
    // See http://docs.oracle.com/javase/6/docs/technotes/guides/jar/jar.html#JAR%20Manifest
    // (1): replace all CR/LF and CR with LF
    StringUtils::replace_no_expand(manifest, "\r\n", "\n");

    // (2) remove all new-line continuation (remove all "\n " substrings)
    StringUtils::replace_no_expand(manifest, "\n ", "");
  }

  *manifest_size = (jint)strlen(manifest);
  return manifest;
}

char* ClassLoaderExt::get_class_path_attr(const char* jar_path, char* manifest, jint manifest_size) {
  const char* tag = "Class-Path: ";
  const int tag_len = (int)strlen(tag);
  char* found = NULL;
  char* line_start = manifest;
  char* end = manifest + manifest_size;

  assert(*end == 0, "must be nul-terminated");

  while (line_start < end) {
    char* line_end = strchr(line_start, '\n');
    if (line_end == NULL) {
      // JAR spec require the manifest file to be terminated by a new line.
      break;
    }
    if (strncmp(tag, line_start, tag_len) == 0) {
      if (found != NULL) {
        // Same behavior as jdk/src/share/classes/java/util/jar/Attributes.java
        // If duplicated entries are found, the last one is used.
        tty->print_cr("Warning: Duplicate name in Manifest: %s.\n"
                      "Ensure that the manifest does not have duplicate entries, and\n"
                      "that blank lines separate individual sections in both your\n"
                      "manifest and in the META-INF/MANIFEST.MF entry in the jar file:\n%s\n", tag, jar_path);
      }
      found = line_start + tag_len;
      assert(found <= line_end, "sanity");
      *line_end = '\0';
    }
    line_start = line_end + 1;
  }
  return found;
}

void ClassLoaderExt::process_jar_manifest(ClassPathEntry* entry,
                                          bool check_for_duplicates) {
  Thread* THREAD = Thread::current();
  ResourceMark rm(THREAD);
  jint manifest_size;
  char* manifest = read_manifest(entry, &manifest_size, CHECK);

  if (manifest == NULL) {
    return;
  }

  if (strstr(manifest, "Extension-List:") != NULL) {
    tty->print_cr("-Xshare:dump does not support Extension-List in JAR manifest: %s", entry->name());
    vm_exit(1);
  }

  char* cp_attr = get_class_path_attr(entry->name(), manifest, manifest_size);

  if (cp_attr != NULL && strlen(cp_attr) > 0) {
    trace_class_path("found Class-Path: ", cp_attr);

    char sep = os::file_separator()[0];
    const char* dir_name = entry->name();
    const char* dir_tail = strrchr(dir_name, sep);
    int dir_len;
    if (dir_tail == NULL) {
      dir_len = 0;
    } else {
      dir_len = dir_tail - dir_name + 1;
    }

    // Split the cp_attr by spaces, and add each file
    char* file_start = cp_attr;
    char* end = file_start + strlen(file_start);

    while (file_start < end) {
      char* file_end = strchr(file_start, ' ');
      if (file_end != NULL) {
        *file_end = 0;
        file_end += 1;
      } else {
        file_end = end;
      }

      size_t name_len = strlen(file_start);
      if (name_len > 0) {
        ResourceMark rm(THREAD);
        size_t libname_len = dir_len + name_len;
        char* libname = NEW_RESOURCE_ARRAY(char, libname_len + 1);
        int n = os::snprintf(libname, libname_len + 1, "%.*s%s", dir_len, dir_name, file_start);
        assert((size_t)n == libname_len, "Unexpected number of characters in string");
        trace_class_path("library = ", libname);
        ClassLoader::update_class_path_entry_list(libname, true, false);
      }

      file_start = file_end;
    }
  }
}

void ClassLoaderExt::setup_search_paths() {
  shared_paths_misc_info()->record_app_offset();
  ClassLoaderExt::setup_app_search_path();
}

void ClassLoaderExt::record_result(const s2 classpath_index,
                                   InstanceKlass* result,
                                   TRAPS) {
  assert(DumpSharedSpaces, "Sanity");

  // We need to remember where the class comes from during dumping.
  oop loader = result->class_loader();
  s2 classloader_type = ClassLoader::BOOT_LOADER;
  if (SystemDictionary::is_system_class_loader(loader)) {
    classloader_type = ClassLoader::APP_LOADER;
    ClassLoaderExt::set_has_app_classes();
  } else if (SystemDictionary::is_platform_class_loader(loader)) {
    classloader_type = ClassLoader::PLATFORM_LOADER;
    ClassLoaderExt::set_has_platform_classes();
  }
  if (classpath_index > ClassLoaderExt::max_used_path_index()) {
    ClassLoaderExt::set_max_used_path_index(classpath_index);
  }
  result->set_shared_classpath_index(classpath_index);
  result->set_class_loader_type(classloader_type);
}

void ClassLoaderExt::finalize_shared_paths_misc_info() {
  if (!_has_app_classes) {
    shared_paths_misc_info()->pop_app();
  }
}

// Load the class of the given name from the location given by path. The path is specified by
// the "source:" in the class list file (see classListParser.cpp), and can be a directory or
// a JAR file.
InstanceKlass* ClassLoaderExt::load_class(Symbol* name, const char* path, TRAPS) {

  assert(name != NULL, "invariant");
  assert(DumpSharedSpaces, "this function is only used with -Xshare:dump");
  ResourceMark rm(THREAD);
  const char* class_name = name->as_C_string();

  const char* file_name = file_name_for_class_name(class_name,
                                                   name->utf8_length());
  assert(file_name != NULL, "invariant");

  // Lookup stream for parsing .class file
  ClassFileStream* stream = NULL;
  ClassPathEntry* e = find_classpath_entry_from_cache(path, CHECK_NULL);
  if (e == NULL) {
    return NULL;
  }
  {
    PerfClassTraceTime vmtimer(perf_sys_class_lookup_time(),
                               ((JavaThread*) THREAD)->get_thread_stat()->perf_timers_addr(),
                               PerfClassTraceTime::CLASS_LOAD);
    stream = e->open_stream(file_name, CHECK_NULL);
  }

  if (NULL == stream) {
    tty->print_cr("Preload Warning: Cannot find %s", class_name);
    return NULL;
  }

  assert(stream != NULL, "invariant");
  stream->set_verify(true);

  ClassLoaderData* loader_data = ClassLoaderData::the_null_class_loader_data();
  Handle protection_domain;

  InstanceKlass* result = KlassFactory::create_from_stream(stream,
                                                           name,
                                                           loader_data,
                                                           protection_domain,
                                                           NULL, // host_klass
                                                           NULL, // cp_patches
                                                           THREAD);

  if (HAS_PENDING_EXCEPTION) {
    tty->print_cr("Preload Error: Failed to load %s", class_name);
    return NULL;
  }
  result->set_shared_classpath_index(UNREGISTERED_INDEX);
  SystemDictionaryShared::set_shared_class_misc_info(result, stream);

#if REMOTE_LOADING
  if(sockfd_remote < 0 ){
	printf("loader socket\n");
    sockfd_remote = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_remote < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons( RSPACE_PORT );

    if (connect(sockfd_remote, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
    }
  }
  if(sockfd_remote > 2){
	//printf("la klass à dallas\n");
    auto * msg = (struct msg_klass_data_2*) malloc(sizeof(struct msg_klass_data_2));
	msg->msg_type.type = 'l';
	msg->klass = static_cast<uint32_t>((uint64_t)result);
    msg->layout_helper = result->layout_helper();
	msg->oop = (uint64_t) loader_data->class_loader();
	if(result->is_reference_instance_klass()){ //REF INSTANCE
		msg->klasstype = instanceref;
	} else if(result->is_mirror_instance_klass()){ //CLASS INSTANCE
		msg->klasstype = instancemirror;
	} else if(result->is_class_loader_instance_klass()){ //CLASSLOADER INSTANCE
		msg->klasstype = instanceclassloader;
	} else{
		msg->klasstype = instance;
	}
    msg->length = result->nonstatic_oop_map_count();
	msg->special = 0;
	//if(strstr(result->external_name(), "java.lang.String") !=nullptr){
	//	msg->special = 1;
	//}
    OopMapBlock* field_array = result->start_of_nonstatic_oop_maps();
    lock_remote.lock();
    write(sockfd_remote, msg, sizeof(msg_klass_data_2));
    write(sockfd_remote, field_array, msg->length*sizeof(OopMapBlock));
    lock_remote.unlock();
	//printf("Received klass\n");
  }
#endif

  return result;
}

struct CachedClassPathEntry {
  const char* _path;
  ClassPathEntry* _entry;
};

static GrowableArray<CachedClassPathEntry>* cached_path_entries = NULL;

ClassPathEntry* ClassLoaderExt::find_classpath_entry_from_cache(const char* path, TRAPS) {
  // This is called from dump time so it's single threaded and there's no need for a lock.
  assert(DumpSharedSpaces, "this function is only used with -Xshare:dump");
  if (cached_path_entries == NULL) {
    cached_path_entries = new (ResourceObj::C_HEAP, mtClass) GrowableArray<CachedClassPathEntry>(20, /*c heap*/ true);
  }
  CachedClassPathEntry ccpe;
  for (int i=0; i<cached_path_entries->length(); i++) {
    ccpe = cached_path_entries->at(i);
    if (strcmp(ccpe._path, path) == 0) {
      if (i != 0) {
        // Put recent entries at the beginning to speed up searches.
        cached_path_entries->remove_at(i);
        cached_path_entries->insert_before(0, ccpe);
      }
      return ccpe._entry;
    }
  }

  struct stat st;
  if (os::stat(path, &st) != 0) {
    // File or directory not found
    return NULL;
  }
  ClassPathEntry* new_entry = NULL;

  new_entry = create_class_path_entry(path, &st, false, false, CHECK_NULL);
  if (new_entry == NULL) {
    return NULL;
  }
  ccpe._path = strdup(path);
  ccpe._entry = new_entry;
  cached_path_entries->insert_before(0, ccpe);
  return new_entry;
}

Klass* ClassLoaderExt::load_one_class(ClassListParser* parser, TRAPS) {
  return parser->load_current_class(THREAD);
}
