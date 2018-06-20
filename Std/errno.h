#ifndef INCLUDED_ERRNO_H
#define INCLUDED_ERRNO_H

#ifndef TESTING

#ifdef __cplusplus
extern "C" {
#endif

extern volatile int* __errno(void) __attribute__((__const__));

extern __thread int errno;

#ifdef __cplusplus
}
#endif


#define ENOENT 2

#endif

#endif