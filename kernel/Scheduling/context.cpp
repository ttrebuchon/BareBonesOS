#include "context.h"
#include <kernel/Utility.hh>
#include <Utils/math>
#include <kernel/Registers.h>

extern "C" void thread_entry_asm();

namespace Kernel
{
extern "C" {

	extern "C" void __save_x86_registers(registers_t, registers_t*);
	extern "C" int __save_x86_context(context_t*, void* esp, void* ebp, void* eip);
	extern "C" int restore_x86_context(const registers_t, void* esp);

	
	__attribute__((__noreturn__))
	void load_context(context_t* c)
	{
		load_destroy_context(c, nullptr);
	}
	
	__attribute__((__noreturn__))
	void load_destroy_context(context_t* c, void(*destr)(context_t*))
	{
		
		
		#ifdef __ENV_AARCH64__
		void* ebp = c->stack.fp;
		void* esp = c->stack.sp;
		void* eip = c->ip;
		
		registers_t regs = const_cast<const context_t*>(c)->registers;
		//register_copy(&c->registers, &regs); //= c->registers;

		__sync_synchronize();
		
		if (destr)
		{
			destr(c);
		}
		
		register_t rx0 = regs.*register_pointers::r0;// regs.x0;
		register_t rx1 = regs.*register_pointers::r1;// regs.x1;

		__sync_synchronize();
		
		
		
		asm volatile("\
		mov SP, %0; \
		mov FP, %1; \
		mov x2, %2; \
		mov x1, %4; \
		mov x0, %3; \
		br x2; \
		NOP" : : "r"(esp), "r"(ebp), "r"(eip), "r"(rx0), "r"(rx1));
		
		#elif defined(__ENV_x86__)

		assert(!destr);
		c->registers.edx = (register_t)c->ip;
		restore_x86_context(c->registers, c->stack.sp);
		__builtin_unreachable();

		
		
		#else
		#error Unknown processor architecture
		#endif
		
		assert(false);
		__builtin_unreachable();
		assert(false);
	}
	
	
	static void save_context_dummy()
	{
		volatile int x = 4;
	}
	
	void some_func(context_t* c, void* someP)
	{
		ASM_READ_ESP(c->ip);
		assert(c->ip == ((char*)c->stack.sp - sizeof(addr_t)*6));
		{
			void** g;
			ASM_READ_EBP(g);
			assert(g[0] == c->stack.fp);
		}
	}

	#ifdef __ENV_AARCH64__
	
	//__attribute__((__returns_twice__))
	int save_context(context_t* c)
	{
		
		
		
		
		/*asm volatile(
		"\
		mov %0, x0 ; \
		mov %1, x1 ; \
		mov %2, x2 ; \
		mov %3, x3 ; \
		mov %4, x4 ; \
		mov %5, x5 ; \
		mov %6, x6 ; \
		mov %7, x7 ; \
		mov %8, x8 ; \
		mov %9, x9 ; \
		mov %10, x10 ; \
		mov %11, x11 ; \
		mov %12, x12 ; \
		mov %13, x13 ; \
		mov %14, x14 ; \
		mov %15, x15 ; \
		mov %16, x16 ; \
		mov %17, x17 ; \
		mov %18, x18 ; \
		mov %19, x19 ; \
		mov %20, x20 ; \
		mov %21, x21 ; \
		mov %22, x22 ; \
		mov %23, x23 ; \
		mov %24, x24 ; \
		mov %25, x25 ; \
		mov %26, x26 ; \
		mov %27, x27 ; \
		mov %28, x28 ; \
		mov %29, x29"
		:
		"&=r"(c->registers.x0), 
		"=r"(c->registers.x1), 
		"=r"(c->registers.x2), 
		"=r"(c->registers.x3), 
		"=r"(c->registers.x4), 
		"=r"(c->registers.x5), 
		"=r"(c->registers.x6), 
		"=r"(c->registers.x7), 
		"=r"(c->registers.x8), 
		"=r"(c->registers.x9), 
		"=r"(c->registers.x10), 
		"=r"(c->registers.x11), 
		"=r"(c->registers.x12), 
		"=r"(c->registers.x13), 
		"=r"(c->registers.x14), 
		"=r"(c->registers.x15), 
		"=r"(c->registers.x16), 
		"=r"(c->registers.x17), 
		"=r"(c->registers.x18), 
		"=r"(c->registers.x19), 
		"=r"(c->registers.x20), 
		"=r"(c->registers.x21), 
		"=r"(c->registers.x22), 
		"=r"(c->registers.x23), 
		"=r"(c->registers.x24), 
		"=r"(c->registers.x25), 
		"=r"(c->registers.x26), 
		"=r"(c->registers.x27), 
		"=r"(c->registers.x28), 
		"=r"(c->registers.x29));*/
		
		
		
		ASM_READ_ESP(c->stack.sp);
		c->stack.sp = (void*)((addr_t)c->stack.sp + 32);
		asm volatile("NOP");
		ASM_READ_EBP(c->ip);
		c->stack.fp = ((void**)c->ip)[0];
		c->ip = ((void**)c->ip)[1];
		//c->registers.x0 = 1;
		c->registers.*register_pointers::r0 = 1;
		//*const_cast<registers_t*>(&c->registers) = reg;
		
		
	
		
		c->registers.*register_pointers::r0 = 1;
		
		// Need to call some function
		// in this function in order for
		// generated asm to store frame and
		// link registers to the stack
		save_context_dummy();
		return 0;
	}

	#endif

	void __save_x86_registers(registers_t regs, registers_t* ptr)
	{
		assert(ptr != nullptr);
		*ptr = regs;
	}

	int __save_x86_context(context_t* context, void* esp, void* ebp, void* eip)
	{
		context->ip = eip;
		context->stack.fp = ebp;
		context->stack.sp = esp;
		context->registers.ebp = (register_t)ebp;
		return 0;
	}
}
}