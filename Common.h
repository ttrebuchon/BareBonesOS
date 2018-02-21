#ifndef INCLUDED_COMMON_H
#define INCLUDED_COMMON_H

#include "kernel/c_cpp.h"

#include <stdint.h>
typedef unsigned char uchar;
typedef uint32_t addr_t;

#ifdef __cplusplus

typedef decltype(sizeof(0)) size_t;



#else
typedef uint32_t size_t;


#endif

#endif

#ifdef __cplusplus
#include <kernel/CppSupport/new.hh>
#endif

#include <kernel/Timer.h>