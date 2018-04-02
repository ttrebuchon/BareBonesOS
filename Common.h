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



#else

//#ifndef TESTING
typedef unsigned long size_t;
//#endif


#endif

#endif

#ifdef __cplusplus
#include <kernel/CppSupport/new.hh>
#endif

#include <kernel/Timer.h>
#include <kernel/MetaInfo.h>
#include <kernel/Debug.h>