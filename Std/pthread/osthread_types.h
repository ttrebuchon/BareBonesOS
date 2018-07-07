#ifndef INCLUDED_PTHREAD_OSTHREAD_TYPES_H
#define INCLUDED_PTHREAD_OSTHREAD_TYPES_H




#include <Common.h>

#include "osthread_mutex.h"



typedef union _osthread_attr_t
{
  //char __size[__SIZEOF_PTHREAD_ATTR_T];
  long int __align;
} osthread_attr_t;




typedef struct __osthread_internal_slist
{
	struct __osthread_internal_slist* __next;
} __osthread_slist_t;


/* Data structures for mutex handling.  The structure of the attribute
   type is not exposed on purpose.  */
/*typedef union
{
	struct __osthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;
    // KIND must stay at this position in the structure to maintain
    // binary compatibility.
    int __kind;
    unsigned int __nusers;
    __extension__ union
    {
      int __spins;
      __osthread_slist_t __list;
      //__osthread_slist_t __list;
    };
  } __data;
  //char __size[__SIZEOF_PTHREAD_MUTEX_T];
  long int __align;
} osthread_mutex_t;*/

typedef union
{
  //char __size[__SIZEOF_PTHREAD_MUTEXATTR_T];
  int __kind;
  long int __align;
} osthread_mutexattr_t;
//} osthread_mutexattr_t;


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
  //char __size[__SIZEOF_PTHREAD_COND_T];
  __extension__ long long int __align;
} osthread_cond_t;
//} osthread_cond_t;

typedef union
{
  //char __size[__SIZEOF_PTHREAD_CONDATTR_T];
  long int __align;
} osthread_condattr_t;
//} osthread_condattr_t;


/* Keys for thread-specific data */
//typedef unsigned int osthread_key_t;

typedef unsigned int osthread_key_t;


/* Once-only execution */
//typedef int osthread_once_t;

typedef int osthread_once_t;


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
    osthread_t __writer;
  } __data;
  //char __size[__SIZEOF_PTHREAD_RWLOCK_T];
  long int __align;
} osthread_rwlock_t;

typedef union
{
  //char __size[__SIZEOF_PTHREAD_RWLOCKATTR_T];
  long int __align;
} osthread_rwlockattr_t;
#endif


#ifdef __USE_XOPEN2K
/* POSIX spinlock data type.  */
typedef volatile int osthread_spinlock_t;


/* POSIX barriers data type.  The structure of the type is
   deliberately not exposed.  */
typedef union
{
  //char __size[__SIZEOF_PTHREAD_BARRIER_T];
  long int __align;
} osthread_barrier_t;

typedef union
{
  //char __size[__SIZEOF_PTHREAD_BARRIERATTR_T];
  int __align;
} osthread_barrierattr_t;
#endif


#endif