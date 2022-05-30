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
    Universe::oops_do(&rc);
    JNIHandles::oops_do(&rc);
    MarkingCodeBlobClosure each_active_code_blob(&rc, !CodeBlobToOopClosure::FixRelocations);
    Threads::oops_do(&rc, &each_active_code_blob);
    ObjectSynchronizer::oops_do(&rc);
    Management::oops_do(&rc);
    JvmtiExport::oops_do(&rc);
    SystemDictionary::oops_do(&rc);
    ClassLoaderDataGraph::always_strong_oops_do(&rc, true);
    AOTLoader::oops_do(&rc);
}