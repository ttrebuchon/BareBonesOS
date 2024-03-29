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
#include <sys/types.h>
typedef long __kernel_long_t;
typedef __kernel_long_t __kernel_time_t;
typedef __kernel_off_t __off_t;
typedef __kernel_uid_t __uid_t;
typedef __kernel_long_t __kernel_off_t;
typedef long long __kernel_loff_t;
typedef __kernel_long_t __kernel_time_t;
typedef __kernel_long_t __kernel_clock_t;

typedef int __kernel_timer_t;
typedef int __kernel_clockid_t;
typedef char * __kernel_caddr_t;
typedef unsigned short __kernel_uid16_t;

typedef unsigned short __kernel_gid16_t;
#else
typedef uint64_t    __kernel_time_t;
typedef size_t      __off_t;
typedef size_t      ssize_t;
typedef uint8_t     mode_t;
typedef uint32_t    __uid_t;
typedef uint32_t    gid_t;
#endif

typedef __kernel_time_t __time_t;
typedef __time_t time_t;
typedef __off_t off_t;
typedef __uid_t uid_t;
typedef long pthread_t;

typedef uid_t gsid_t;

#endif