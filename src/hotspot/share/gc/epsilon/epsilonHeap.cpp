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

#include "precompiled.hpp"
#include "gc/epsilon/epsilonHeap.hpp"
#include "gc/epsilon/epsilonMemoryPool.hpp"
#include "gc/epsilon/epsilonThreadLocalData.hpp"
#include "memory/allocation.hpp"
#include "memory/allocation.inline.hpp"
#include "memory/resourceArea.hpp"
#include "gc/epsilon/RemoteSpace.hpp"
#include "gc/epsilon/roots.hpp"
#include "code/codeCache.hpp"
#include "prims/jvmtiExport.hpp"

#define REMOTE_SPACE


class VM_Pause: public VM_Operation {
private:
  const GCCause::Cause _cause;
  EpsilonHeap* const _heap;
  static size_t _req_id;
public:
  VM_Pause(GCCause::Cause cause) : VM_Operation(),
                                            _cause(cause),
                                            _heap(EpsilonHeap::heap()) {};

  VM_Operation::VMOp_Type type() const { return VMOp_Dummy; }
  const char* name()             const { return "Epsilon Collection"; }

  bool evaluate_at_safepoint() const {return true;}

  bool evaluate_concurrently() const {return false;}


  virtual bool doit_prologue() {
    //size_t id = OrderAccess::load_acquire(&_req_id);

    // Need to take the Heap lock before managing backing storage.
	//printf("before lock\n");
    //Heap_lock->lock();
	//printf("after lock\n");

    //// Heap lock also naturally serializes GC requests, and allows us to coalesce
    //// back-to-back GC requests from many threads. Avoid the consecutive GCs
    //// if we started waiting when other GC request was being handled.
    //if (id < OrderAccess::load_acquire(&_req_id)) {
	//  printf("Don't need to do c ollection\n");
    //  Heap_lock->unlock();
    //  return false;
    //}

    //// No contenders. Start handling a new GC request.
    //Atomic::inc(&_req_id);
    //return true;
	return true;
  }

  virtual void doit() {
      size_t id = OrderAccess::load_acquire(&_req_id);
      Heap_lock->lock();

      if (id < OrderAccess::load_acquire(&_req_id)) {
        Heap_lock->unlock();
        return;
      }
      Atomic::inc(&_req_id);


	  //printf("Start pause\n");
	  _heap->collect_impl();
	  //printf("End pause \n");


	  Heap_lock->unlock();
  }

  virtual void doit_epilogue() {
    //Heap_lock->unlock();
  }
};

size_t VM_Pause::_req_id = 0;


jint EpsilonHeap::initialize() {
  counter=0;
  size_t align = _policy->heap_alignment();
  size_t init_byte_size = align_up(_policy->initial_heap_byte_size(), align);
  size_t max_byte_size  = align_up(_policy->max_heap_byte_size(), align);

  // Initialize backing storage
  ReservedSpace heap_rs = Universe::reserve_heap(max_byte_size, align);
  _virtual_space.initialize(heap_rs, init_byte_size);

  MemRegion committed_region((HeapWord*)_virtual_space.low(),          (HeapWord*)_virtual_space.high());
  MemRegion  reserved_region((HeapWord*)_virtual_space.low_boundary(), (HeapWord*)_virtual_space.high_boundary());

  initialize_reserved_region(reserved_region.start(), reserved_region.end());

#ifdef REMOTE_SPACE
  _space = new RemoteSpace();
  ((RemoteSpace*) _space)->set_fd(heap_rs.fd_for_heap());
  ((RemoteSpace*) _space)->set_range(heap_rs.base(), heap_rs.size());
  shm = ((RemoteSpace*) _space)->shm;
#else
  _space = new ContiguousSpace();
#endif

  _space->initialize(committed_region, /* clear_space = */ true, /* mangle_space = */ true);

  cap = committed_region.word_size()*8;

  // Precompute hot fields
  _max_tlab_size = MIN2(CollectedHeap::max_tlab_size(), align_object_size(EpsilonMaxTLABSize / HeapWordSize));
  _step_counter_update = MIN2<size_t>(max_byte_size / 16, EpsilonUpdateCountersStep);
  _step_heap_print = (EpsilonPrintHeapSteps == 0) ? SIZE_MAX : (max_byte_size / EpsilonPrintHeapSteps);
  _decay_time_ns = (int64_t) EpsilonTLABDecayTime * NANOSECS_PER_MILLISEC;

  // Enable monitoring
  _monitoring_support = new EpsilonMonitoringSupport(this);
  _last_counter_update = 0;
  _last_heap_print = 0;

  // Install barrier set
  BarrierSet::set_barrier_set(new EpsilonBarrierSet());

  // All done, print out the configuration
  if (init_byte_size != max_byte_size) {
    log_info(gc)("Resizeable heap; starting at " SIZE_FORMAT "M, max: " SIZE_FORMAT "M, step: " SIZE_FORMAT "M",
                 init_byte_size / M, max_byte_size / M, EpsilonMinHeapExpand / M);
  } else {
    log_info(gc)("Non-resizeable heap; start/max: " SIZE_FORMAT "M", init_byte_size / M);
  }

  if (UseTLAB) {
    log_info(gc)("Using TLAB allocation; max: " SIZE_FORMAT "K", _max_tlab_size * HeapWordSize / K);
    if (EpsilonElasticTLAB) {
      log_info(gc)("Elastic TLABs enabled; elasticity: %.2fx", EpsilonTLABElasticity);
    }
    if (EpsilonElasticTLABDecay) {
      log_info(gc)("Elastic TLABs decay enabled; decay time: " SIZE_FORMAT "ms", EpsilonTLABDecayTime);
    }
  } else {
    log_info(gc)("Not using TLAB allocation");
  }

  return JNI_OK;
}

void EpsilonHeap::post_initialize() {
  CollectedHeap::post_initialize();
  ((RemoteSpace*) _space)->post_initialize();
}

void EpsilonHeap::initialize_serviceability() {
  _pool = new EpsilonMemoryPool(this);
  _memory_manager.add_pool(_pool);
}

GrowableArray<GCMemoryManager*> EpsilonHeap::memory_managers() {
  GrowableArray<GCMemoryManager*> memory_managers(1);
  memory_managers.append(&_memory_manager);
  return memory_managers;
}

GrowableArray<MemoryPool*> EpsilonHeap::memory_pools() {
  GrowableArray<MemoryPool*> memory_pools(1);
  memory_pools.append(_pool);
  return memory_pools;
}

size_t EpsilonHeap::unsafe_max_tlab_alloc(Thread* thr) const {
  // Return max allocatable TLAB size, and let allocation path figure out
  // the actual allocation size. Note: result should be in bytes.
  return _max_tlab_size * HeapWordSize;
}

EpsilonHeap* EpsilonHeap::heap() {
  CollectedHeap* heap = Universe::heap();
  assert(heap != NULL, "Uninitialized access to EpsilonHeap::heap()");
  assert(heap->kind() == CollectedHeap::Epsilon, "Not an Epsilon heap");
  return (EpsilonHeap*)heap;
}

HeapWord* EpsilonHeap::allocate_work(size_t size) {
  HeapWord* res = allocate_work_impl(size);
  if (res == NULL) {
	vm_collect_impl();
    res = allocate_work_impl(size);
  }
  return res;
}	

HeapWord* EpsilonHeap::allocate_work_klass(size_t size, Klass* klass) {
  HeapWord* res = allocate_work_klass_impl(size, klass);
  if (res == nullptr) {
	vm_collect_impl();
    res = allocate_work_klass_impl(size, klass);
  }
  return res;
}	

HeapWord* EpsilonHeap::allocate_work_impl(size_t size) {
    assert(is_object_aligned(size), "Allocation size should be aligned: " SIZE_FORMAT, size);
	return _space->par_allocate(size);
}




HeapWord* EpsilonHeap::allocate_work_klass_impl(size_t size, Klass* klass) {
    assert(is_object_aligned(size), "Allocation size should be aligned: " SIZE_FORMAT, size);
    HeapWord* mem =  _space->par_allocate_klass(size, klass);
    //HeapWord* mem =  _space->par_allocate(size);

	size_t used = _space->used();
	{
	 size_t last = _last_counter_update;
   	 if ((used - last >= _step_counter_update) && Atomic::cmpxchg(used, &_last_counter_update, last) == last) {
   	   _monitoring_support->update_counters();
   	 }
	}

	{
    size_t last = _last_heap_print;
    if ((used - last >= _step_heap_print) && Atomic::cmpxchg(used, &_last_heap_print, last) == last) {
      print_heap_info(used);
      print_metaspace_info();
    }
    }

	return mem;
}


void EpsilonHeap::concurrent_post_allocate(HeapWord* allocated, size_t size, Klass* klass){
	((RemoteSpace*)_space)->concurrent_post_allocate(allocated, size, klass);

	size_t used = _space->used();
	{
    size_t last = _last_counter_update;
    if ((used - last >= _step_counter_update) && Atomic::cmpxchg(used, &_last_counter_update, last) == last) {
      _monitoring_support->update_counters();
    }
  }
}

HeapWord* EpsilonHeap::allocate_new_tlab(size_t min_size,
                                         size_t requested_size,
                                         size_t* actual_size) {
  Thread* thread = Thread::current();

  // Defaults in case elastic paths are not taken
  bool fits = true;
  size_t size = requested_size;
  size_t ergo_tlab = requested_size;
  int64_t time = 0;

  if (EpsilonElasticTLAB) {
    ergo_tlab = EpsilonThreadLocalData::ergo_tlab_size(thread);

    if (EpsilonElasticTLABDecay) {
      int64_t last_time = EpsilonThreadLocalData::last_tlab_time(thread);
      time = (int64_t) os::javaTimeNanos();

      assert(last_time <= time, "time should be monotonic");

      // If the thread had not allocated recently, retract the ergonomic size.
      // This conserves memory when the thread had initial burst of allocations,
      // and then started allocating only sporadically.
      if (last_time != 0 && (time - last_time > _decay_time_ns)) {
        ergo_tlab = 0;
        EpsilonThreadLocalData::set_ergo_tlab_size(thread, 0);
      }
    }

    // If we can fit the allocation under current TLAB size, do so.
    // Otherwise, we want to elastically increase the TLAB size.
    fits = (requested_size <= ergo_tlab);
    if (!fits) {
      size = (size_t) (ergo_tlab * EpsilonTLABElasticity);
    }
  }

  // Always honor boundaries
  size = MAX2(min_size, MIN2(_max_tlab_size, size));

  // Always honor alignment
  size = align_up(size, MinObjAlignment);

  // Check that adjustments did not break local and global invariants
  assert(is_object_aligned(size),
         "Size honors object alignment: " SIZE_FORMAT, size);
  assert(min_size <= size,
         "Size honors min size: "  SIZE_FORMAT " <= " SIZE_FORMAT, min_size, size);
  assert(size <= _max_tlab_size,
         "Size honors max size: "  SIZE_FORMAT " <= " SIZE_FORMAT, size, _max_tlab_size);
  assert(size <= CollectedHeap::max_tlab_size(),
         "Size honors global max size: "  SIZE_FORMAT " <= " SIZE_FORMAT, size, CollectedHeap::max_tlab_size());

  if (log_is_enabled(Trace, gc)) {
    ResourceMark rm;
    log_trace(gc)("TLAB size for \"%s\" (Requested: " SIZE_FORMAT "K, Min: " SIZE_FORMAT
                          "K, Max: " SIZE_FORMAT "K, Ergo: " SIZE_FORMAT "K) -> " SIZE_FORMAT "K",
                  thread->name(),
                  requested_size * HeapWordSize / K,
                  min_size * HeapWordSize / K,
                  _max_tlab_size * HeapWordSize / K,
                  ergo_tlab * HeapWordSize / K,
                  size * HeapWordSize / K);
  }

  // All prepared, let's do it!
  HeapWord* res = allocate_work(size);

  if (res != NULL) {
    // Allocation successful
    *actual_size = size;
    if (EpsilonElasticTLABDecay) {
      EpsilonThreadLocalData::set_last_tlab_time(thread, time);
    }
    if (EpsilonElasticTLAB && !fits) {
      // If we requested expansion, this is our new ergonomic TLAB size
      EpsilonThreadLocalData::set_ergo_tlab_size(thread, size);
    }
  } else {
    // Allocation failed, reset ergonomics to try and fit smaller TLABs
    if (EpsilonElasticTLAB) {
      EpsilonThreadLocalData::set_ergo_tlab_size(thread, 0);
    }
  }

  return res;
}

HeapWord* EpsilonHeap::mem_allocate(size_t size, bool *gc_overhead_limit_was_exceeded) {
  *gc_overhead_limit_was_exceeded = false;
  return allocate_work(size);
}

HeapWord* EpsilonHeap::mem_allocate_klass(size_t size, bool *gc_overhead_limit_was_exceeded, Klass* klass) {
    *gc_overhead_limit_was_exceeded = false;
    return allocate_work_klass(size, klass);
}

void EpsilonHeap::collect(GCCause::Cause cause) {
  switch (cause) {
    case GCCause::_metadata_GC_threshold:
    case GCCause::_metadata_GC_clear_soft_refs:
      // Receiving these causes means the VM itself entered the safepoint for metadata collection.
      // While Epsilon does not do GC, it has to perform sizing adjustments, otherwise we would
      // re-enter the safepoint again very soon.

      assert(SafepointSynchronize::is_at_safepoint(), "Expected at safepoint");
      log_info(gc)("GC request for \"%s\" is handled", GCCause::to_string(cause));
      MetaspaceGC::compute_new_size();
      print_metaspace_info();
      break;
      //log_info(gc)("GC request for \"%s\" is ignored", GCCause::to_string(cause));
  }

  //if (SafepointSynchronize::is_at_safepoint()) {
  //  	  //printf("safepoint\n");
  //  	  collect_impl();
  //      } else {
  //  	  //printf("no safepoint\n");
  //  	  vm_collect_impl();
  //      }

#if COMPILER2_OR_JVMCI
    DerivedPointerTable::clear();
#endif
}


void EpsilonHeap::collect_impl(){
  CodeCache::gc_prologue();
  BiasedLocking::preserve_marks();
  DerivedPointerTable::set_active(false);
  //DerivedPointerTable::update_pointers();
  ((RemoteSpace*)_space)->stw_pre_collect();
  //((RemoteSpace*)_space)->collect();
  //BiasedLocking::restore_marks();
  //CodeCache::gc_epilogue();
  //JvmtiExport::gc_epilogue();
  //_monitoring_support->update_counters();
}

void EpsilonHeap::vm_collect_impl(){
  VM_Pause pause(gc_cause());
  VMThread::execute(&pause);
}

void EpsilonHeap::do_full_collection(bool clear_all_soft_refs) {
//	if (SafepointSynchronize::is_at_safepoint()) {
//    	  //printf("safepoint\n");
//    	  collect_impl();
//        } else {
//    	  //printf("no safepoint\n");
//    	  vm_collect_impl();
//    }
}

void EpsilonHeap::safe_object_iterate(ObjectClosure *cl) {
  printf("Trying to safe_object iterate\n");
  _space->safe_object_iterate(cl);
}

void EpsilonHeap::print_on(outputStream *st) const {
  st->print_cr("Epsilon Heap");

  // Cast away constness:
  ((VirtualSpace)_virtual_space).print_on(st);

  st->print_cr("Allocation space:");
  _space->print_on(st);

  MetaspaceUtils::print_on(st);
}

void EpsilonHeap::print_tracing_info() const {
  print_heap_info(used());
  print_metaspace_info();
}

void EpsilonHeap::print_heap_info(size_t used) const {
  size_t reserved  = max_capacity();
  size_t committed = capacity();

  if (reserved != 0) {
    log_info(gc)("Heap: " SIZE_FORMAT "%s reserved, " SIZE_FORMAT "%s (%.2f%%) committed, "
                 SIZE_FORMAT "%s (%.2f%%) used",
            byte_size_in_proper_unit(reserved),  proper_unit_for_byte_size(reserved),
            byte_size_in_proper_unit(committed), proper_unit_for_byte_size(committed),
            committed * 100.0 / reserved,
            byte_size_in_proper_unit(used),      proper_unit_for_byte_size(used),
            used * 100.0 / reserved);
  } else {
    log_info(gc)("Heap: no reliable data");
  }
}

void EpsilonHeap::print_metaspace_info() const {
  size_t reserved  = MetaspaceUtils::reserved_bytes();
  size_t committed = MetaspaceUtils::committed_bytes();
  size_t used      = MetaspaceUtils::used_bytes();

  if (reserved != 0) {
    log_info(gc, metaspace)("Metaspace: " SIZE_FORMAT "%s reserved, " SIZE_FORMAT "%s (%.2f%%) committed, "
                            SIZE_FORMAT "%s (%.2f%%) used",
            byte_size_in_proper_unit(reserved),  proper_unit_for_byte_size(reserved),
            byte_size_in_proper_unit(committed), proper_unit_for_byte_size(committed),
            committed * 100.0 / reserved,
            byte_size_in_proper_unit(used),      proper_unit_for_byte_size(used),
            used * 100.0 / reserved);
  } else {
    log_info(gc, metaspace)("Metaspace: no reliable data");
  }
}





