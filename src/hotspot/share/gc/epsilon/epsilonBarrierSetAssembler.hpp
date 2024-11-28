#ifndef EPSILONBARRIERSETASSEMBLER_HPP
#define EPSILONBARRIERSETASSEMBLER_HPP

#include "asm/macroAssembler.hpp"
#include "gc/shared/barrierSetAssembler.hpp"
#ifdef COMPILER1
class LIR_Assembler;
class StubAssembler;
#endif
class StubCodeGenerator;

class EpsilonBarrierSetAssembler: public BarrierSetAssembler {

public:
void tlab_allocate(MacroAssembler* masm, Register thread, Register obj, Register var_size_in_bytes, int con_size_in_bytes, Register t1, Register t2, Label& slow_case);


};

#endif // CPU_X86_GC_SHENANDOAH_SHENANDOAHBARRIERSETASSEMBLER_X86_HPP

