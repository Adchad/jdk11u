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
 /* 

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
  

  //TODO c'est le code pour faire l'alloc en assembleur
  //__ print_state();
  
  __ cmpq(var_size_in_bytes, 65536);
  __ jcc(Assembler::above, slow_case);
  __ subq(var_size_in_bytes, 1);
  __ bsrq(t1, var_size_in_bytes);
  __ leaq(t2, Address(0, t1, Address::times_1, 29));
  __ leaq(t2, Address(0, var_size_in_bytes, Address::times_4));
  __ xorq(t1, t2);
  __ cmpq(var_size_in_bytes, 8);
  __ sbbq(obj, obj);
  __ andq(t1, obj);
  __ xorq(t1, t2);
 
 // __ print_state();
  __ movq(t2, Address(thread, Thread::pseudo_tlab_offset()));
  __ cmpq(t2, 0);
  __ jcc(Assembler::equal, slow_case);
  __ movq(obj, t1);
  __ shlq(t1,12);
  __ addq(t2, t1);
  __ movq(obj,Address(t2,0));
  __ cmpq(Address(0,t2, Address::times_4,4), obj);
  __ jcc(Assembler::below, slow_case);
  //__ movq(Address(Address(t2,8), t2), obj );
  __ shlq(t2, 9);
  __ addl(obj, 1);
  __ movl(Address(t2,0),obj);
  __ movq(t1, Address(t2,obj,Address::times_8, 8));
  __ movq(obj, Address(t1,0));

 
  //__ stop("Je test");
 */




 __ jmp(slow_case);

  

}
