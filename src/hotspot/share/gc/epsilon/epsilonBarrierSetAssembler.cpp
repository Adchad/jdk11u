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
  Label fast_case_end;
  Label slow_case_end;
  Label slow_case_and_print;
/*
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
*/ 

  //TODO c'est le code pour faire l'alloc en assembleur
  
  //__ warn("Before");

  //__ subq(stackp, 8);
  //__ movq(Addres(stackp, 8), var_size_in_bytes);
  //__ jmp(slow_case);

 // __ push(t1);
  //__ push(t2);
  if (var_size_in_bytes == t2 &&  var_size_in_bytes != noreg) {
	__ push(var_size_in_bytes);
  }
  //__ pusha();
  __ cmpq(var_size_in_bytes, 65536);
  __ jcc(Assembler::above, slow_case_end);
  //__ warn("First Branch");

  if (var_size_in_bytes == noreg) {
	__ mov64(t1, con_size_in_bytes);
  } else {
	__ movq(t1, var_size_in_bytes);   // movq  %rdi, %rcx 
  }

  __ shrq(t1, 3);				    // shrq  $3, %rcx 
  __ leaq(t2, Address(t1, -1));	    // leaq  -1(%rcx), %rax  
  __ bsrq(obj, t2);				    // bsrq  %rax, %rdx
  __ shlq(t2, 2);				    // salq  $2, %rax
  __ addq(obj, 5);                  // addq  $29, %rdx
  __ xorq(t2, obj);			        // xorq  %rdx, %rax
  __ cmpq(t1, 9);				    // cmpq  $9, %rcx 
  __ sbbq(t1, t1);				    // sbbq  %rcx, %rcx
  __ andq(t2, t1);                  // andq  %rcx, %rax
  //__ xorq(obj, t2);				    // xorq  %rdx, %rax
  __ xorq(t2, obj);				    // xorq  %rdx, %rax

  __ movq(t1, obj);


  __ movq(t2, Address(thread, Thread::pseudo_tlab_offset()));
  __ cmpq(t2, 0);
  __ jcc(Assembler::equal, slow_case_end);
  //__ warn("Second Branch");
  __ shlq(t1,3);
  __ addq(t2, t1);
  __ movq(t2, Address(t2,0));
  __ cmpq(t2, 0);
  __ jcc(Assembler::equal, slow_case_end);
  //__ warn("Third Branch");

 
  __ xorq(obj, obj);
  //__ print_state();
  //__ movl(obj,Address(t2,0)); // TODO Swap those 
  //__ movl(t2,Address(t2,4));  // four lines for 
  //__ cmpl(obj, t2);           // the two
  //__ print_state();           // below
  __ movl(obj,Address(t2,0));
  __ cmpl(obj, Address(t2,4));
  __ jcc(Assembler::aboveEqual, slow_case_end);
  //__ warn("Fourth Branch");
  //__ movq(Address(Address(t2,8), t2), obj );
  __ movq(t1, t2);
  __ addq(t1, 40);
  __ shll(obj, 3);
  __ movq(obj, Address(t1, obj)); //obj contient l'addresse finale
  __ movl(t1,Address(t2,0));
  __ addl(t1, 1);
  __ movl(Address(t2,0), t1);

  __ cmpq(obj, 0);
  __ jcc(Assembler::equal, slow_case_end);

  //__ print_state();
  //__ popa();
  if (var_size_in_bytes == t2 && var_size_in_bytes != noreg) {
	__ pop(var_size_in_bytes);
  }
  __ jmp(fast_case_end);


  //__ movq(var_size_in_bytes, Address(stackp, 8));
  //__ addq(stackp, 8)
  //
  
  __ bind(slow_case_end);
  if (var_size_in_bytes == t2 && var_size_in_bytes != noreg) {
	__ pop(var_size_in_bytes);
  }
  //__ pop(t2);
  //__ pop(t1);
  //__ popa();
  __ jmp(slow_case);


  //__ bind(slow_case_and_print);
  //__ print_state();
  //__ pop(var_size_in_bytes);
  //__ pop(t2);
  //__ pop(t1);
  //__ jmp(slow_case);


  __ bind(fast_case_end);
  //__ popa();
  //__ pop(var_size_in_bytes);
  //__ pop(t2);
  //__ pop(t1);


  //__ print_state();
  //__ warn("After");
 
  //__ stop("Je test");
 

  


  // __ jmp(slow_case);

  

}
