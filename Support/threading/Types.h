#ifndef INCLUDED_SUPPORT_THREADING_TYPES_H
#define INCLUDED_SUPPORT_THREADING_TYPES_H

#include <Common.h>



#if __P_ARCH__ == 32

#define __SIZEOF_THREAD_ATTR_T 36
#define __SIZEOF_THREAD_MUTEX_T 24
#define __SIZEOF_THREAD_MUTEXATTR_T 4
#define __SIZEOF_THREAD_COND_T 48
#define __SIZEOF_THREAD_COND_COMPAT_T 12
#define __SIZEOF_THREAD_CONDATTR_T 4
#define __SIZEOF_THREAD_RWLOCK_T 32
#define __SIZEOF_THREAD_RWLOCKATTR_T 8
#define __SIZEOF_THREAD_BARRIER_T 20
#define __SIZEOF_THREAD_BARRIERATTR_T 4

#else


#define __SIZEOF_THREAD_ATTR_T 36
#define __SIZEOF_THREAD_MUTEX_T 32
#define __SIZEOF_THREAD_MUTEXATTR_T 8
#define __SIZEOF_THREAD_COND_T 48
#define __SIZEOF_THREAD_COND_COMPAT_T 12
#define __SIZEOF_THREAD_CONDATTR_T 4
#define __SIZEOF_THREAD_RWLOCK_T 32
#define __SIZEOF_THREAD_RWLOCKATTR_T 8
#define __SIZEOF_THREAD_BARRIER_T 20
#define __SIZEOF_THREAD_BARRIERATTR_T 4


#endif

C_CODE
START_NS(Support)
START_NS(threading)


union thread_attr_t
{
  char __size[__SIZEOF_THREAD_ATTR_T];
  long int __align;
} __attribute__((__packed__));
#ifndef __have_thread_attr_t
typedef union thread_attr_t thread_attr_t;
# define __have_thread_attr_t	1
#endif

typedef struct __thread_internal_slist
{
  struct __thread_internal_slist *__next;
} __thread_slist_t;


/* Data structures for mutex handling.  The structure of the attribute
   type is not exposed on purpose.  */
typedef union
{
  struct __thread_mutex_s
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
      __thread_slist_t __list;
    };
  } __attribute__((__packed__)) __data;
  char __size[__SIZEOF_THREAD_MUTEX_T];
  long int __align;
} __attribute__((__packed__)) thread_mutex_t;

typedef union
{
  char __size[__SIZEOF_THREAD_MUTEXATTR_T];
  long int __align;
} __attribute__((__packed__)) thread_mutexattr_t;


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
  char __size[__SIZEOF_THREAD_COND_T];
  __extension__ long long int __align;
} thread_cond_t;

typedef union
{
  char __size[__SIZEOF_THREAD_CONDATTR_T];
  long int __align;
} thread_condattr_t;


/* Keys for thread-specific data */
typedef unsigned int thread_key_t;


/* Once-only execution */
typedef int thread_once_t;

#ifdef __cplusplus
static_assert(sizeof(thread_attr_t) == __SIZEOF_THREAD_ATTR_T);
static_assert(sizeof(thread_mutex_t) == __SIZEOF_THREAD_MUTEX_T);
static_assert(sizeof(thread_mutexattr_t) == __SIZEOF_THREAD_MUTEXATTR_T);
static_assert(sizeof(thread_cond_t) == __SIZEOF_THREAD_COND_T);
#endif


END_NS // (threading)
END_NS // (Support)

C_END

#endif