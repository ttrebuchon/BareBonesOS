#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H

#include "Error.h"

#ifndef TESTING

#include <drivers/VGA.hh>



#ifdef __cplusplus
extern "C" {
#endif

inline void __ASSERT_TRUE() {}

//#define ASSERT(X) do { (X) ? __ASSERT_TRUE() : KPANIC(#X); } while (0)

#ifdef _TRACE

    #ifdef __cplusplus

        #define TRACE(X) do { ::Drivers::VGA::Write(__FILE__"::"); ::Drivers::VGA::Write((uint32_t)__LINE__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(__func__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(X); ::Drivers::VGA::Write("\n"); } while (false)
        
        #define TRACE_VAL(X) do { ::Drivers::VGA::Write(__FILE__"::"); ::Drivers::VGA::Write((uint32_t)__LINE__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(__func__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(#X " = "); ::Drivers::VGA::Write(X); ::Drivers::VGA::Write("\n"); } while (false)

    #else

        #define TRACE(X) c_vga_write(X)
        #define TRACE_VAL(X) do { c_vga_write(#X " = "); c_vga_write(X); } while (false)

    #endif

#else // _TRACE

#define TRACE(X) 

#endif // !_TRACE

#ifdef __cplusplus
}
#endif


//Are testing
#else // _TESTING


#ifdef __cplusplus
#include <iostream>
#endif

#include "MetaInfo.h"





//#define ASSERT(X) assert(X)

#ifdef _TRACE

#ifdef __cplusplus
	
	#ifdef __USE_MEM_POOL__
		#include <sstream>
		#include <kernel/Memory/Allocators/cross_processor_allocator.hh>
		#define TRACE(X) do { \
			::std::basic_stringstream<char, ::std::char_traits<char>, ::Kernel::Memory::cross_proc_allocator<char>> ss; \
			ss << __FILE__"::" << __LINE__ << "::" << __func__ << "::" << X; \
			::std::cout << ss.str().c_str() << std::endl; \
		} \
		while (false)
		
		
		#define __TRACEV_INNER(V, STR) do { \
			::std::basic_stringstream<char, ::std::char_traits<char>, ::Kernel::Memory::cross_proc_allocator<char>> ss; \
			ss << __FILE__"::" << __LINE__ << "::" << __func__ << "::" << (STR) << " = " << (V); \
			::std::cout << ss.str().c_str() << std::endl; \
		} \
		while (false)
		
		
		#define TRACE_VAL(X) __TRACEV_INNER(X, #X)
		
		#define TRACE_VAL_HX(X) __TRACEV_INNER((void*)(uintptr_t)(X), #X)
		
	#else
		
		#define TRACE(X) (std::cout << __FILE__"::" << __LINE__ << "::" << __func__ << "::" << (X) << std::endl);
		
		#define __TRACEV_INNER(V, STR) (std::cout << __FILE__"::" << __LINE__ << "::" << __func__ << "::" << (STR) << " = " << (V) << std::endl);
		
		#define TRACE_VAL(X) __TRACEV_INNER(X, #X)
		
		#define TRACE_VAL_HX(X) __TRACEV_INNER((void*)(uintptr_t)(X), #X)
		
//STACK(X)
	
	#endif // __USE_MEM_POOL

#else // !__cplusplus

#define TRACE(X) STACK(X)

#endif // !__cplusplus

#else // _TRACE

#define TRACE(X) 
#define TRACE_VAL(X) 
#define TRACE_VAL_HX(X)

#endif // !_TRACE



#endif //ifndef TESTING

#define ASSERT(x) assert(x)

#endif