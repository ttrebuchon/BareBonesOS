#include "context.h"
#include <kernel/Utility.hh>
#include <Utils/math>
#include <kernel/Registers.h>

extern "C" void thread_entry_asm();

namespace Kernel
{
extern "C" {

	extern "C" void __save_x86_registers(registers_t, registers_t*);
	extern "C" void save_x86_registers(registers_t*);

	
	void __save_context_store(volatile context_t*, void*, void*, void*);
	
	__attribute__((__noreturn__))
	void load_context(volatile context_t* c)
	{
		load_destroy_context(c, nullptr);
	}
	
	__attribute__((__noreturn__))
	void load_destroy_context(volatile context_t* c, void(*destr)(volatile context_t*))
	{
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
		
		#ifdef __ENV_AARCH64__
		
		
		
		
		asm volatile("\
		mov SP, %0; \
		mov FP, %1; \
		mov x2, %2; \
		mov x1, %4; \
		mov x0, %3; \
		br x2; \
		NOP" : : "r"(esp), "r"(ebp), "r"(eip), "r"(rx0), "r"(rx1));
		
		#elif defined(__ENV_x86__)
		
		asm volatile(" \n\
		mov %0, %%esp; \n\
		mov %1, %%ebp; \n\
		mov %2, %%ecx; \n\
		mov %4, %%ebx; \n\
		mov %3, %%eax; \n\
		jmp *%%ecx; \n\
		NOP" : : "r"(esp), "r"(ebp), "r"(eip), "r"(rx0), "r"(rx1));

		
		
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
	
	/*asm(" \
	.global save_context, function; \
	save_context: \
		mov x1, lr; \
		mov x2, fp; \
		mov x3, sp; \
		sub sp, sp, 32; \
		stp x3, x2, [sp, #0]; \
		str x1, [sp, #16]; \
		bl __save_context_store; \
		ret");*/
	
	void some_func(volatile context_t* c, void* someP)
	{
		ASM_READ_ESP(c->ip);
		assert(c->ip == ((char*)c->stack.sp - sizeof(addr_t)*6));
		{
			void** g;
			ASM_READ_EBP(g);
			assert(g[0] == c->stack.fp);
		}
	}
	
	__attribute__((__returns_twice__))
	int save_context(volatile context_t* c)
	{
		
		#ifdef __ENV_AARCH64__
		
		
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
		
		
		
		
		
		#elif defined(__ENV_x86__)
		
		//#error TODO

		ASM_READ_ESP(c->stack.sp);
		asm volatile ("NOP");
		ASM_READ_EBP(c->ip);
		c->stack.fp = ((void**)c->ip)[0];
		c->ip = ((void**)c->ip)[1];
		save_x86_registers(const_cast<registers_t*>(&c->registers));


		
		#else
		
		#error Unknown architecture
		
		#endif
		
		c->registers.*register_pointers::r0 = 1;
		
		// Need to call some function
		// in this function in order for
		// generated asm to store frame and
		// link registers to the stack
		save_context_dummy();
		return 0;
	}
		
	
	void __save_context_store(volatile context_t* c, void* ip, void* fp, void* sp)
	{
		c->ip = ip;
	}

	void __save_x86_registers(registers_t regs, registers_t* ptr)
	{
		assert(ptr != nullptr);
		*ptr = regs;
	}
}
}