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
#include "oops/instanceKlass.inline.hpp"
#include "oops/instanceMirrorKlass.inline.hpp"
#include "oops/instanceRefKlass.inline.hpp"
#include "oops/instanceClassLoaderKlass.inline.hpp"
#include "oops/oop.inline.hpp"
#include "gc/epsilon/gc_helper.hpp"


void GCHelper::do_it(){
	struct linked_list* curr;
	oop curr_oop;

	curr = helper_roots;
	while(curr != nullptr){
		oop curr_oop = (oop) curr->value;
		hc->do_oop(&curr_oop);
		curr_oop->oop_iterate(hc);
		curr = curr->next;
	}
}
