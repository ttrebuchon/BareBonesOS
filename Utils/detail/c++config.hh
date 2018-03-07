#include <kernel/Error.h>


#ifndef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE

#define  _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A)

#endif


#ifndef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER

#define  _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A)


#endif

#ifndef _GLIBCXX_THROW_OR_ABORT

#ifdef __EXCEPTIONS

#define _GLIBCXX_THROW_OR_ABORT(_EXC) (throw (_EXC))

#else

    #ifndef TESTING
    #define _GLIBCXX_THROW_OR_ABORT(_EXC) (KPANIC(""))
    #else
    #define _GLIBCXX_THROW_OR_ABORT(_EXC) (__builtin_abort())
    #endif

#endif

#endif