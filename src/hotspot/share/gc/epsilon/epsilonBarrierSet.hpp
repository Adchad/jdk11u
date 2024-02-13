/*
 * Copyright (c) 2017, 2018, Red Hat, Inc. All rights reserved.
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

#ifndef SHARE_VM_GC_EPSILON_BARRIERSET_HPP
#define SHARE_VM_GC_EPSILON_BARRIERSET_HPP

#include "gc/shared/barrierSet.hpp"


// No interaction with application is required for Epsilon, and therefore
// the barrier set is empty.
class EpsilonBarrierSet: public BarrierSet {
  friend class VMStructs;

public:
  EpsilonBarrierSet();

  virtual void print_on(outputStream *st) const {}

  virtual void on_thread_create(Thread* thread);
  virtual void on_thread_destroy(Thread* thread);

  template <DecoratorSet decorators, typename BarrierSetT = EpsilonBarrierSet>
  class AccessBarrier: public BarrierSet::AccessBarrier<decorators, BarrierSetT> {
    typedef BarrierSet::AccessBarrier<decorators, BarrierSetT> Raw;

  public:

    // Needed for loads on non-heap weak references
//    template <typename T>
//    static oop oop_load_not_in_heap(T* addr){
//		uint32_t *array = (uint32_t*)(&addr);
//		if(( array[0] == 0xdeadbeef ) || ( array[1] == 0xdeadbeef) ){
//			printf("prout, addr: %p \n", addr);
//		}
//
//		oop value = Raw::template oop_load_not_in_heap<T>(addr);
//  		return value;
//	};
//

	static void breakpointable(){}

	static void check_addr(void* addr){
		if(*((uint32_t*)((uint64_t)addr - 4 /*KLASS_OFFSET*/)) == 0xffffffff){
			printf("Lecture d'un truc free, addr:%p\n", addr);
			breakpointable();
		}
	}
    // Needed for weak references
    static oop oop_load_in_heap_at(oop base, ptrdiff_t offset){
		void* addr =(void*)((uint64_t) base + (uint64_t) offset);
		check_addr(addr);

		oop value = Raw::oop_load_in_heap_at(base, offset);
		return value;
	}


	 // Defensive: will catch weak oops at addresses in heap
    template <typename T>
	static oop oop_load_in_heap(T* addr){
		check_addr((void*)addr);
	
		oop value = Raw::template oop_load_in_heap<T>(addr);
		return value;
	}

  };
};

template<>
struct BarrierSet::GetName<EpsilonBarrierSet> {
  static const BarrierSet::Name value = BarrierSet::EpsilonBarrierSet;
};

template<>
struct BarrierSet::GetType<BarrierSet::EpsilonBarrierSet> {
  typedef ::EpsilonBarrierSet type;
};

#endif // SHARE_VM_GC_EPSILON_BARRIERSET_HPP
