#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H



#ifndef TESTING

#include <drivers/VGA.hh>
#include "Error.h"



#ifdef __cplusplus
extern "C" {
#endif

inline void __ASSERT_TRUE() {}

#define ASSERT(X) do { (X) ? __ASSERT_TRUE() : KPANIC(#X); } while (0)

#ifdef _TRACE

#define TRACE(X) Drivers::VGA::Write(X)
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

#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif



#define ASSERT(X) assert(X)

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

#endif
#endif