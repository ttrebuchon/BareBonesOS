// #ifndef INCLUDED_PTHREAD_TYPES_H
// #define INCLUDED_PTHREAD_TYPES_H

// #include <Common.h>

// C_CODE


// typedef struct
// {
//     uint32_t flags;
//     void* stack_base;
//     size_t stack_size;
//     size_t guard_size;
//     int32_t sched_policy;
//     int32_t __sched_priority;


//     char __reserved[16];
// } pthread_attr_t;

// typedef struct
// {
//     int value;
//     char __reserved[36];


// } pthread_mutex_t;

// typedef struct
// {
//     int value;
//     char __reserved[44];


// } pthread_cond_t;


// C_END
// #endif

/* Copyright (C) 2002-2007, 2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_PTHREADTYPES_H
#define _BITS_PTHREADTYPES_H	1

//#include <endian.h>

#define __SIZEOF_PTHREAD_ATTR_T 36
#define __SIZEOF_PTHREAD_MUTEX_T 24
#define __SIZEOF_PTHREAD_MUTEXATTR_T 4
#define __SIZEOF_PTHREAD_COND_T 48
#define __SIZEOF_PTHREAD_COND_COMPAT_T 12
#define __SIZEOF_PTHREAD_CONDATTR_T 4
#define __SIZEOF_PTHREAD_RWLOCK_T 32
#define __SIZEOF_PTHREAD_RWLOCKATTR_T 8
#define __SIZEOF_PTHREAD_BARRIER_T 20
#define __SIZEOF_PTHREAD_BARRIERATTR_T 4


/* Thread identifiers.  The structure of the attribute type is not
   exposed on purpose.  */
//typedef unsigned long int pthread_t;

#include <Common.h>

#ifdef TESTING
#define PTHREAD_N(X) osthread_##X
#define PTHREAD_N2(Y, X) Y##osthread_##X
#else
#define PTHREAD_N(X) pthread_##X
#define PTHREAD_N2(Y, X) Y##pthread_##X
#endif


union PTHREAD_N2(_,attr_t)//_pthread_attr_t
{
  char __size[__SIZEOF_PTHREAD_ATTR_T];
  long int __align;
};
#if !defined(__have_pthread_attr_t) && !defined(TESTING)
typedef union PTHREAD_N2(_,attr_t) PTHREAD_N(attr_t);
# define __have_pthread_attr_t	1
#endif




typedef struct PTHREAD_N2(__,internal_slist)
{
	struct PTHREAD_N2(__,internal_slist)* __next;
} PTHREAD_N2(__,slist_t);


/* Data structures for mutex handling.  The structure of the attribute
   type is not exposed on purpose.  */
typedef union
{
	struct PTHREAD_N2(__,mutex_s)
  {
    int __lock;
    unsigned int __count;
    int __owner;
    /* KIND must stay at this position in the structure to maintain
       binary compatibility.  */
    int __kind;
    unsigned int __nusers;
    __extension__ union
    {
      int __spins;
      PTHREAD_N2(__, slist_t) __list;
      //__pthread_slist_t __list;
    };
  } __data;
  char __size[__SIZEOF_PTHREAD_MUTEX_T];
  long int __align;
} pthread_mutex_t;
//} pthread_mutex_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_MUTEXATTR_T];
  long int __align;
} PTHREAD_N(mutexattr_t);
//} pthread_mutexattr_t;


/* Data structure for conditional variable handling.  The structure of
   the attribute type is not exposed on purpose.  */
typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[__SIZEOF_PTHREAD_COND_T];
  __extension__ long long int __align;
} PTHREAD_N(cond_t);
//} pthread_cond_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_CONDATTR_T];
  long int __align;
} PTHREAD_N(condattr_t);
//} pthread_condattr_t;


/* Keys for thread-specific data */
//typedef unsigned int pthread_key_t;

typedef unsigned int PTHREAD_N(key_t);


/* Once-only execution */
//typedef int pthread_once_t;

typedef int PTHREAD_N(once_t);


#if defined __USE_UNIX98 || defined __USE_XOPEN2K
/* Data structure for read-write lock variable handling.  The
   structure of the attribute type is not exposed on purpose.  */
typedef union
{
  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;
#if __BYTE_ORDER == __BIG_ENDIAN
    unsigned char __pad1;
    unsigned char __pad2;
    unsigned char __shared;
    /* FLAGS must stay at this position in the structure to maintain
       binary compatibility.  */
    unsigned char __flags;
#else
    /* FLAGS must stay at this position in the structure to maintain
       binary compatibility.  */
    unsigned char __flags;
    unsigned char __shared;
    unsigned char __pad1;
    unsigned char __pad2;
#endif
    pthread_t __writer;
  } __data;
  char __size[__SIZEOF_PTHREAD_RWLOCK_T];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_RWLOCKATTR_T];
  long int __align;
} pthread_rwlockattr_t;
#endif


#ifdef __USE_XOPEN2K
/* POSIX spinlock data type.  */
typedef volatile int pthread_spinlock_t;


/* POSIX barriers data type.  The structure of the type is
   deliberately not exposed.  */
typedef union
{
  char __size[__SIZEOF_PTHREAD_BARRIER_T];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_BARRIERATTR_T];
  int __align;
} pthread_barrierattr_t;
#endif


#endif	/* bits/pthreadtypes.h */