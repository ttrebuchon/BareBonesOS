#ifndef INCLUDED_REGISTERS_H
#define INCLUDED_REGISTERS_H

#include <kernel/c_cpp.h>
#include <stdint.h>
#include <Types.h>
#include <Env.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __Registers
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} Registers_t;

#ifdef __cplusplus
}
#endif

C_CODE

#if __P_ARCH__ == 64
typedef uint64_t register_t;
#elif __P_ARCH__ == 32
typedef uint32_t register_t;
#else
#error Unknown architecture
#endif

typedef struct
{
	#if defined(__ENV_x86__)
		
		register_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		
	#elif defined(__ENV_AARCH64__)
		
		//#error Need to define AARCH64 registers
		register_t x0;
		register_t x1;
		register_t x2;
		register_t x3;
		register_t x4;
		register_t x5;
		register_t x6;
		register_t x7;
		register_t x8;
		register_t x9;
		register_t x10;
		register_t x11;
		register_t x12;
		register_t x13;
		register_t x14;
		register_t x15;
		register_t x16;
		register_t x17;
		register_t x18;
		register_t x19;
		register_t x20;
		register_t x21;
		register_t x22;
		register_t x23;
		register_t x24;
		register_t x25;
		register_t x26;
		register_t x27;
		register_t x28;
		register_t x29;
		register_t x30; // <- LR
		
		
		register_t SP;
		register_t SP_EL1, SP_EL2, SP_EL3;
		
		register_t ELR_EL1, ELR_EL2, EPR_EL3;
		uint32_t SPSR_EL1, SPSR_EL2, SPSR_EL3;
		register_t pc;
	#elif defined(__VS_CODE__)

	#else
		
		#error Unknown architecture
		
	#endif
}
__attribute__((__packed__))
registers_t;

#ifdef __ENV_x86__
IF_CPP(static_assert(offsetof(registers_t, eax) > 0));
IF_CPP(static_assert(offsetof(registers_t, edi) == 0));
#endif

void register_copy(volatile registers_t*, const volatile registers_t*);



C_END

#ifdef __cplusplus
	typedef register_t(registers_t::*register_ptr);
	
	struct register_pointers
	{
	#if defined(__ENV_x86__)
		
		constexpr static register_ptr r0 = &registers_t::eax;
		constexpr static register_ptr r1 = &registers_t::ebx;
		
	#elif defined(__ENV_AARCH64__)
		
		constexpr static register_ptr r0 = &registers_t::x0;
		constexpr static register_ptr r1 = &registers_t::x1;

	#elif defined(__VS_CODE__)
		
	#else
	
		#error Unknown architecture
		
	#endif
	};
	
#endif // __cplusplus

#endif