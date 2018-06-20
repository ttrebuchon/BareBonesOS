#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H

#include "Error.h"

#ifndef TESTING

#include <drivers/VGA.hh>
#include <kernel/Debug/Trace.h>



#ifdef __cplusplus
extern "C" {
#endif

inline void __ASSERT_TRUE() {}

//#define assert(X) do { (X) ? __ASSERT_TRUE() : KPANIC(#X); } while (0)

#ifdef _TRACE

    // #ifdef __cplusplus

    //     #define TRACE(X) do { ::Drivers::VGA::Write(__FILE__"::"); ::Drivers::VGA::Write((uint32_t)__LINE__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(__func__); ::Drivers::VGA::Write("::"); ::Drivers::VGA::Write(X); ::Drivers::VGA::Write("\n"); } while (false)

    // #else

    //     #define TRACE(X) c_vga_write(X)

    // #endif

	#define TRACE(X) do { __do_trace(__FILE__, __LINE__, __func__, X); } while (false)

#else

#define TRACE(X) 

#endif

#ifdef __cplusplus
}
#endif


//Are testing
#else

#include "MetaInfo.h"

#ifdef __cplusplus
extern "C" {
#endif





#ifdef _TRACE

#define TRACE(X) STACK(X)

#else

#define TRACE(X) 

#endif

#ifdef __cplusplus
}
#endif

#endif //ifndef TESTING



#endif