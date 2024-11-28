#include "interpreter/interpreter.hpp"
#include "interpreter/interp_masm.hpp"
#include "runtime/sharedRuntime.hpp"
#include "runtime/thread.hpp"
#include "vmreg_x86.inline.hpp"
#include "precompiled.hpp"
#include "utilities/macros.hpp"
#include "gc/epsilon/epsilonBarrierSetAssembler.hpp"
#ifdef COMPILER1
#include "c1/c1_LIRAssembler.hpp"
#include "c1/c1_MacroAssembler.hpp"
#include "gc/shenandoah/c1/shenandoahBarrierSetC1.hpp"
#endif

#define __ masm->


void EpsilonBarrierSetAssembler::tlab_allocate(MacroAssembler* masm,
                                        Register thread, Register obj,
                                        Register var_size_in_bytes,
                                        int con_size_in_bytes,
                                        Register t1,
                                        Register t2,
                                        Label& slow_case) {
  assert_different_registers(obj, t1, t2);
  assert_different_registers(obj, var_size_in_bytes, t1);
  Register end = t2;
  if (!thread->is_valid()) {
#ifdef _LP64
    thread = r15_thread;
#else
    assert(t1->is_valid(), "need temp reg");
    thread = t1;
    __ get_thread(thread);
#endif
  }

  __ verify_tlab();

  __ movptr(obj, Address(thread, JavaThread::tlab_top_offset()));
  if (var_size_in_bytes == noreg) {
    __ lea(end, Address(obj, con_size_in_bytes));
  } else {
    __ lea(end, Address(obj, var_size_in_bytes, Address::times_1));
  }
  __ cmpptr(end, Address(thread, JavaThread::tlab_end_offset()));
  __ jcc(Assembler::above, slow_case);

  // update the tlab top pointer
  __ movptr(Address(thread, JavaThread::tlab_top_offset()), end);

  // recover var_size_in_bytes if necessary
  if (var_size_in_bytes == end) {
    __ subptr(var_size_in_bytes, obj);
  }
  __ verify_tlab();

}
