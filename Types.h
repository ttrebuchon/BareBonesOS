#ifndef INCLUDED_TYPES_H
#define INCLUDED_TYPES_H

#include <stdint.h>
#include <float.h>
#include <iso646.h>
#include <limits.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdnoreturn.h>


#if defined(TESTING) || defined(FREE_TESTING)
typedef long __kernel_long_t;
typedef __kernel_long_t __kernel_time_t;
#else
typedef uint64_t __kernel_time_t;
#endif

typedef __kernel_time_t __time_t;
typedef __time_t time_t;



#endif