#include "context.h"
#include <kernel/Utility.hh>
#include <Utils/math>



namespace Kernel
{
extern "C" {
	
	void __save_context_store(context_t*, void*, void*, void*);
	
	__attribute__((__noreturn__))
	void load_context(context_t* c)
	{
		load_destroy_context(c, nullptr);
	}
	
	__attribute__((__noreturn__))
	void load_destroy_context(context_t* c, void(*destr)(context_t*))
	{
		__DO_CLI__;
		#ifndef TESTING
		//asm volatile("cli");
		#endif
		void* ebp = c->stack.fp;
		void* esp = c->stack.sp;
		void* eip = c->ip;
		
		if (destr)
		{
			destr(c);
		}
		
		
		#ifdef __ENV_AARCH64__
		
		asm volatile("\
		mov SP, %0; \
		mov FP, %1; \
		mov x1, %2; \
		mov w0, #1; " 
		#ifndef TESTING
		"sti; "
		#endif
		"br x1; \
		NOP" : : "r"(esp), "r"(ebp), "r"(eip));
		
		#elif defined(__ENV_x86__)
		
		asm volatile("\
		mov %0, %%esp; \
		mov %1, %%ebp;\
		mov %2, %%ecx;\
		mov %%eax, 0; "
		#ifndef TESTING
		"sti; "
		#endif
		"jmp *(%%ecx); \
		NOP" : : "r"(esp), "r"(ebp), "r"(eip));

		
		
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
	
	void some_func(context_t* c, void* someP)
	{
		ASM_MOV(%0, %%ESP, : "=r"(c->ip));
		ASM_READ_ESP(c->ip);
		assert(c->ip == c->stack.sp - sizeof(addr_t)*6);
		{
			void** g;
			ASM_READ_EBP(g);
			assert(g[0] == c->stack.fp);
		}
	}
	
	//__attribute__((__returns_twice__))
	int save_context(volatile context_t* c)
	{
		
		#if __ENV__ == aarch64
		ASM_READ_ESP(c->stack.sp);
		c->stack.sp = (void*)((addr_t)c->stack.sp + 32);
		asm volatile("NOP");
		ASM_READ_EBP(c->ip);
		c->stack.fp = ((void**)c->ip)[0];
		c->ip = ((void**)c->ip)[1];
		#elif __ENV__ == x86
		
		#error TODO
		
		#else
		
		#error Unknown architecture
		
		#endif
		
		// Need to call some function
		// in this function in order for 		// generated asm to store frame and
		//link registers to the stack
		save_context_dummy();
		return 0;
	}
		
	
	void __save_context_store(context_t* c, void* ip, void* fp, void* sp)
	{
		c->ip = ip;
	}
}
}