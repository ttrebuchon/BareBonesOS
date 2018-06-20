#ifndef INCLUDED_ERROR_H
#define INCLUDED_ERROR_H

enum
{
    NOT_IMPLEMENTED = 0x0,
};



#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
[[ noreturn ]]
#endif
void __do_kernel_panic(const char* msg, const char* file, const int line, const char* function);

#ifdef __cplusplus
[[ noreturn ]]
#endif
void __do_kernel_panic_quiet();


#define KPANIC(X) __do_kernel_panic(X, __FILE__, __LINE__, __func__)





#ifdef __cplusplus
}
#endif

#include <Std/assert.h>

#endif