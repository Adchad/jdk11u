//
// Created by adam on 5/3/22.
//
#include "roots.hpp"

#include "precompiled.hpp"
#include "aot/aotLoader.hpp"
#include "classfile/systemDictionary.hpp"
#include "code/codeCache.hpp"
#include "gc/shared/collectedHeap.hpp"
#include "gc/shared/gcTimer.hpp"
#include "gc/shared/gcTraceTime.inline.hpp"
#include "logging/log.hpp"
#include "memory/resourceArea.hpp"
#include "memory/universe.hpp"
#include "oops/objArrayKlass.inline.hpp"
#include "oops/oop.inline.hpp"
#include "prims/jvmtiExport.hpp"
#include "runtime/jniHandles.hpp"
#include "runtime/thread.hpp"
#include "runtime/vmThread.hpp"
#include "services/management.hpp"
#include "utilities/stack.inline.hpp"

void RootMark::do_it(){

    switch (_root_type) {
        case universe:
            Universe::oops_do(&rc);
            break;

        case jni_handles:
            JNIHandles::oops_do(&rc);
            break;

        case threads:
        {
            MarkingCodeBlobClosure each_active_code_blob(&rc, !CodeBlobToOopClosure::FixRelocations);
            Threads::oops_do(&rc, &each_active_code_blob);
        }
            break;

        case object_synchronizer:
            ObjectSynchronizer::oops_do(&rc);
            break;

        case management:
            Management::oops_do(&rc);
            break;

        case jvmti:
            JvmtiExport::oops_do(&rc);
            break;

        case system_dictionary:
            SystemDictionary::oops_do(&rc);
            break;

        case class_loader_data:
            ClassLoaderDataGraph::always_strong_oops_do(&rc, true);
            break;

        case code_cache:
            // Do not treat nmethods as strong roots for mark/sweep, since we can unload them.
            //CodeCache::scavenge_root_nmethods_do(CodeBlobToOopClosure(&rc));
            AOTLoader::oops_do(&rc);
            break;

        default:
            fatal("Unknown root type");
    }

}