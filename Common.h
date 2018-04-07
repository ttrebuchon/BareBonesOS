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
typedef uint64_t addr_t;

#include "Types.h"

#define likely(X) __builtin_expect((X), 1)
#define unlikely(X) __builtin_expect((X), 0)

#ifdef __cplusplus
#include <new>
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

#define IFCPP17(X)

#endif

#endif

#ifdef __cplusplus
#include <kernel/CppSupport/new.hh>
#endif

#include <kernel/Timer.h>
#include <kernel/MetaInfo.h>
#include <kernel/Debug.h>