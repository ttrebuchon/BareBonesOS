#include "Faults.h"
#include <kernel/Interrupts/ISR.h>
#include <Utils/iostream>


namespace Kernel { namespace Interrupts
{
	extern "C"
	{

		void initialize_fault_handlers()
		{
			register_interrupt_handler(0x6, &invalid_opcode_fault);
		}





		void invalid_opcode_fault(Registers_t regs)
		{
			__DO_CLI__;



			std::cout << "Invalid opcode at: " << (void*)regs.eip << std::endl;

			ASM_CODE("\
						cli; \
						hlt");
		}


	}
}
}