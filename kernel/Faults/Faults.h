#ifndef INCLUDED_KERNEL_FAULTS_FAULTS_H
#define INCLUDED_KERNEL_FAULTS_FAULTS_H

#include <Common.h>
#include <kernel/Registers.h>

typedef struct __Registers Registers_t;

START_NS(Kernel)
START_NS(Interrupts)

C_CODE




void initialize_fault_handlers();

void invalid_opcode_fault(Registers_t regs);



C_END
END_NS // Interrupts
END_NS // Kernel



#endif