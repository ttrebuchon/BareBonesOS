#include <kernel/Error.h>


#ifndef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE

#define  _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A)

#endif


#ifndef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER

#define  _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A)


#endif

#ifdef _THROW_OR_ABORT
#undef __THROW_OR_ABORT

#endif



#ifdef __EXCEPTIONS

#ifdef throw
#undef throw
#endif

#define __THROW_OR_ABORT(_EXC) throw  (_EXC)

#else

    #ifndef TESTING
    #define __THROW_OR_ABORT(_EXC) KPANIC("")
    #else
    #define __THROW_OR_ABORT(_EXC) __builtin_abort()
    #endif

#endif