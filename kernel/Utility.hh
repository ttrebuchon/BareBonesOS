#ifndef INCLUDED_KERNEL_UTILITY_HH
#define INCLUDED_KERNEL_UTILITY_HH

#include <Common.h>

namespace Kernel
{
	namespace detail
	{
		
	}
	
	extern "C"
	{
		addr_t read_eip();
		void read_eip_ptr(addr_t*);
		[[noreturn]] void pop_eip(addr_t);
	}
	
	
	
}



#define ASM_EXEC(X, ...) asm volatile (#X __VA_ARGS__)





// ASM_OP1
#define ASM_OP1(OP, X, ...) asm volatile(#OP" "#X __VA_ARGS__)



// ASM_OP2
#define ASM_OP2(OP, X, Y, ...) asm volatile(#OP" "#X", "#Y __VA_ARGS__)




// ASM_MOV(X, Y) -> X = Y
#ifdef __aarch64__

#define ASM_MOV(X, Y, ...) ASM_OP2(mov, X, Y, __VA_ARGS__)

#else

#define ASM_MOV(X, Y, ...) ASM_OP2(mov, Y, X, __VA_ARGS__)

#endif



// ASM_GET
#define ASM_GET(X, Y) ASM_MOV(%0, X, : "=r"(Y))

// ASM_SET
#define ASM_SET(X, Y) ASM_MOV(X, %0, : : "r"(Y))



// ASM_READ_ESP / ASM_SET_ESP
// ASM_READ_EBP / ASM_SET_EBP
#ifdef __aarch64__

#define ASM_READ_ESP(X) ASM_GET(SP, X)
#define ASM_SET_ESP(X) ASM_SET(SP, X)

#define ASM_READ_EBP(X) //ASM_GET(BP, X)
#define ASM_SET_EBP(X) //ASM_SET(BP, X)


#else

#define ASM_READ_ESP(X) ASM_GET(%ESP, X)
#define ASM_SET_ESP(X) ASM_SET(%ESP, X)

#define ASM_READ_EBP(X) ASM_GET(%EBP, X)
#define ASM_SET_EBP(X) ASM_SET(%EBP, X)

#endif

#if !defined(TESTING) && !defined(FREE_TESTING)
	#define __DO_CLI__ asm volatile("cli")
	#define __DO_STI__ asm volatile("sti")
#else
	#define __DO_CLI__
	#define __DO_STI__ 
#endif








#endif