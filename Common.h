#ifndef INCLUDED_COMMON_H
#define INCLUDED_COMMON_H

#include "kernel/c_cpp.h"


#ifdef __cplusplus

typedef decltype(sizeof(0)) size_t;



#else



#endif

#include <stdint.h>
typedef unsigned char uchar;

#endif

#ifdef __cplusplus
#include <kernel/CppSupport/new.hh>
#endif
