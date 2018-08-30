#ifndef INCLUDED_ERRNO_H
#define INCLUDED_ERRNO_H

#include <Common.h>



#ifdef __cplusplus
extern "C" {
#endif

extern volatile int* __kerrno(void) __attribute__((__const__));

extern __thread int kerrno;

#ifndef TESTING

#define errno kerrno

extern const char* strerror(int err);
extern int strerror_r(int errnum, char* buf, size_t buf_len);


// Invalid argument
#define EINVAL 1

// No such file or directory
#define ENOENT 2

// Illegal byte sequence
#define EILSEQ 3

// Operation in progress
#define EINPROGRESS 4

// Interrupted function
#define EINTR 5

// Is a directory
#define EISDIR 6

// File name too long
#define ENAMETOOLONG 7

// No buffer space available
#define ENOBUFS 8

// No such device
#define ENODEV 9

// I/O error
#define EIO 10

// No locks available
#define ENOLCK 11

// Not enough space
#define ENOMEM 12

// No space left on device
#define ENOSPC 13

// Function not supported
#define ENOSYS 14

// Not a directory
#define ENOTDIR 15

// State not recoverable
#define ENOTRECOVERABLE 16

// Not supported
#define ENOTSUP 17

// Value too large for data type
#define EOVERFLOW 18

// Operation not permitted
#define EPERM 19

// Result too large
#define ERANGE 20

// Bad address
#define EFAULT 21


#endif // !TESTING



#define ECOUNT_ERR 81
#define EMAX_ERR 85

extern int set_error(int no, const char* msg);
#define tset_error(NO, MSG) TRACE(MSG); \
set_error(NO, MSG);

extern const char* kstrerror(int err);
extern int kstrerror_r(int errnum, char* buf, size_t buf_len);

extern const char* ___errs[];



#ifdef __cplusplus
}
#endif



#endif