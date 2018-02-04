#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

inline void __ASSERT_TRUE() {}

#define ASSERT(X) do { (X) ? __ASSERT_TRUE() : KPANIC(#X); } while (0)

#ifdef __cplusplus
}
#endif


#endif