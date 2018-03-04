#ifndef INCLUDED_COMMON_H
#define INCLUDED_COMMON_H

#include "kernel/c_cpp.h"

#include <stdint.h>
typedef unsigned char uchar;
typedef uint64_t addr_t;

#ifdef __cplusplus

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