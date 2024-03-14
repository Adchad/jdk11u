//
// Created by adam on 5/3/22.
//
#include "roots.hpp"

#include "precompiled.hpp"
#include "aot/aotLoader.hpp"
#include "classfile/systemDictionary.hpp"
#include "classfile/stringTable.hpp"
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
#include "gc/shared/referenceProcessor.hpp"
#include "gc/shared/weakProcessor.hpp"
#include "gc/shared/oopStorage.inline.hpp"
#include "gc/epsilon/RemoteSpace.hpp"


void RootMark::do_it(){
	//StrongRootsScope scope(1);
	CLDToOopClosure clds(&rc, false);
	MarkingCodeBlobClosure blobs(&rc, false);

	{
		MutexLockerEx lock(CodeCache_lock, Mutex::_no_safepoint_check_flag);
		//printf("CodeCache\n");
		CodeCache::blobs_do(&blobs);
	}
	//Oop
	
	//printf("Universe\n");
    Universe::oops_do(&rc);
	//printf("JNI\n");
    JNIHandles::oops_do(&rc);
    //MarkingCodeBlobClosure each_active_code_blob(&rc, !CodeBlobToOopClosure::FixRelocations);
	//printf("Threads\n");
    Threads::oops_do(&rc, &blobs);
	//printf("ObjectSync\n");
    ObjectSynchronizer::oops_do(&rc);
	//printf("Management\n");
    Management::oops_do(&rc);
	//printf("Jvmti\n");
    JvmtiExport::oops_do(&rc);
	//printf("SystemDictionary\n");
    SystemDictionary::oops_do(&rc);
	//printf("WeakProcessor\n");
	WeakProcessor::oops_do(&rc);
    //ClassLoaderDataGraph::always_strong_oops_do(&rc, true);
	//printf("ClassLoader\n");
	ClassLoaderDataGraph::cld_do(&clds);
	//printf("AOTLoader\n");
    AOTLoader::oops_do(&rc);

    ObjectSynchronizer::oops_do(&rc);

	StringTable::oops_do(&rc);

	CodeBlobToOopClosure cbtoc((OopClosure*)&rc, false);
	CodeCache::scavenge_root_nmethods_do(&cbtoc);

	
	struct hw_list* curr_poule = RemoteSpace::poule;
	while(curr_poule !=nullptr){
		rc.do_oop(&curr_poule->hw);
		curr_poule = curr_poule->next;
	}
}
