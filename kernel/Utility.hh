#ifndef INCLUDED_KERNEL_UTILITY_HH
#define INCLUDED_KERNEL_UTILITY_HH

#include <Env.h>

#ifdef __cplusplus

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
#endif // __cplusplus

#if !defined(__VS_CODE__)

	#define ASM_CODE(...) asm volatile(__VA_ARGS__)

#else

	#define ASM_CODE(...)

#endif


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



// ASM_GET(X, Y) -> Y = X
#define ASM_GET(X, Y) ASM_MOV(%0, X, : "=r"(Y))

// ASM_SET -> X = Y
#define ASM_SET(X, Y) ASM_MOV(X, %0, : : "r"(Y))



// ASM_READ_ESP / ASM_SET_ESP
// ASM_READ_EBP / ASM_SET_EBP
#ifdef __aarch64__

#define ASM_READ_ESP(X) ASM_GET(SP, X)
#define ASM_SET_ESP(X) ASM_SET(SP, X)

#define ASM_READ_EBP(X) ASM_GET(FP, X)
#define ASM_SET_EBP(X) ASM_SET(FP, X)


#define ASM_RET_REG X30


#define __IRET__ ERET

#ifndef TESTING

#define __CLI__ __do_aarch64_cli()
#define __STI__ __do_aarch64_sti()

C_CODE
__attribute__((__always_inline__))
static inline void __do_aarch64_cli()
{
	asm volatile(" \
	MRS x1, CPSR; \
	ORR x1, x1, #0x80/0x40; \
	MSR cpsr_c, x1");
}

__attribute__((__always_inline__))
static inline void __do_aarch64_sti()
{
	asm volatile(" \
	MRS x1, CPSR; \
	BIC x1, x1, #0x80/0x40; \
	MSR cpsr_c, x1");
}
C_END

#endif // !TESTING

#else

#define ASM_READ_ESP(X) ASM_GET(%%ESP, X)
#define ASM_SET_ESP(X) ASM_SET(%%ESP, X)

#define ASM_READ_EBP(X) ASM_GET(%%EBP, X)
#define ASM_SET_EBP(X) ASM_SET(%%EBP, X)

#define __IRET__ IRET

#ifndef TESTING

#define __CLI__ asm volatile("cli")
#define __STI__ asm volatile("sti")

#endif //!TESTING


#endif

#if !defined(TESTING) && !defined(FREE_TESTING)
	#define __DO_CLI__ __CLI__;
	#define __DO_STI__ __STI__;
#else
	
	C_CODE
	void __testing_signal_cli();
	void __testing_signal_sti();
	C_END
	
	#ifdef __cplusplus
	
	#define __DO_CLI__ ::__testing_signal_cli();
	#define __DO_STI__ ::__testing_signal_sti();
	
	#else
	
	#define __DO_CLI__ __testing_signal_cli();
	#define __DO_STI__ __testing_signal_sti();
	
	#endif //__cplusplus
#endif








#endif