#ifndef INCLUDED_ERROR_H
#define INCLUDED_ERROR_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif

void __do_kernel_panic(const char* msg, const char* file, const int line, const char* function);


#define KPANIC(X) __do_kernel_panic(X, __FILE__, __LINE__, __func__)






#ifdef __cplusplus
}
#endif

#endif