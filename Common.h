#ifndef INCLUDED_COMMON_H
#define INCLUDED_COMMON_H



#include "kernel/c_cpp.h"

#include <stdint.h>
#include <float.h>
#include <iso646.h>
#include <limits.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdnoreturn.h>
typedef unsigned char uchar;

#include "Env.h"

#if __P_ARCH__ == 64
typedef uint64_t addr_t;
typedef uint64_t uword_t;
typedef int64_t sword_t;
#else
typedef uint32_t addr_t;
typedef uint32_t uword_t;
typedef int32_t sword_t;
#endif

#include "Types.h"

#define likely(X) __builtin_expect((X), 1)
#define unlikely(X) __builtin_expect((X), 0)

#ifdef __cplusplus
#include <kernel/CppSupport/new.hh>
//#include <new>
//#ifndef TESTING

typedef unsigned long size_t;
typedef decltype(sizeof(0)) size_t;

//#endif




#define C_CODE extern "C" {
#define C_END }

#define C_namespace(X) namespace X {
#define C_namespace_e(X) }

#define IF_CPP_ELSE(X, Y) X
#define IF_CPP(X) X
#define IF_C(X) 
#define _NOEX noexcept



	#if __cplusplus >= 201703L
		#define IFCPP17(X) X
	#else
		#define IFCPP17(X)
	#endif


#else

//#ifndef TESTING
typedef unsigned long size_t;
//#endif


#define C_CODE
#define C_END

#define C_namespace(X)
#define C_namespace_e(X)

#define IF_CPP_ELSE(X, Y) Y
#define IF_CPP(X)
#define IF_C(X) X
#define _NOEX 

#define IFCPP17(X)

#endif



#ifdef __cplusplus
	
	#include <Utils/constexpr_string.hh>
	
	#define __YEAR__ (::Utils::compile::constexpr_launder<::Utils::compile::cstring(__DATE__).substr<7>().parse_int()>::value)
	
	#define __DAY__ (::Utils::compile::constexpr_launder<::Utils::compile::cstring(__DATE__).substr<4, 2>().parse_int()>::value)
	
#endif

#ifndef TESTING

/* GCC can always grok prototypes.  For C++ programs we add throw()
   to help it optimize the function calls.  But this works only with
   gcc 2.8.x and egcs.  For gcc 3.2 and up we even mark C functions
   as non-throwing using a function attribute since programs can use
   the -fexceptions options for C code as well.  */
# if !defined __cplusplus
#  define __THROW	__attribute__ ((__nothrow__ __LEAF))
#  define __THROWNL	__attribute__ ((__nothrow__))
#  define __NTH(fct)	__attribute__ ((__nothrow__ __LEAF)) fct
# else
#   define __THROW	throw ()
#   define __THROWNL	throw ()
#   define __NTH(fct)	__LEAF_ATTR fct throw ()
# endif

#ifndef __nonnull
# define __nonnull(params) __attribute__ ((__nonnull__ params))
#endif

#endif



#ifdef __cplusplus
#include <kernel/CppSupport/new.hh>
#endif

#include <kernel/Timer.h>
#include <kernel/MetaInfo.h>
#include <kernel/Debug.h>
#include <kernel/Interrupts.h>

#endif //INCLUDED...