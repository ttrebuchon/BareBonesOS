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

#define TRACE(X) do { ::Drivers::VGA::Write(__FILE__"::"); ::Drivers::VGA::Write((uint32_t)__LINE__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(__func__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(X); ::Drivers::VGA::Write("\n"); } while (false)
#define TRACE_C(X) c_vga_write(X)

#else

#define TRACE(X) 
#define TRACE_C(X) 

#endif

#ifdef __cplusplus
}
#endif


//Are testing
#else

#ifdef __cplusplus
extern "C" {
#endif



//#define ASSERT(X) assert(X)

#ifdef _TRACE

#define TRACE(X) 
#define TRACE_C(X) 

#else

#define TRACE(X) 
#define TRACE_C(X) 

#endif

#ifdef __cplusplus
}
#endif

#endif //ifndef TESTING

#define ASSERT(x) assert(x)

#endif