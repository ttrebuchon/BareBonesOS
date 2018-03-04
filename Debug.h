// #ifndef INCLUDED_DEBUG_H
// #define INCLUDED_DEBUG_H

// //Includes and whatnot





// #endif

// #ifdef __cplusplus
// extern "C" {
// #endif


// #ifdef DEBUG

// #ifndef INCLUDED_DEBUG_H_ENABLED
// #define INCLUDED_DEBUG_H_ENABLED
// #undef INCLUDED_DEBUG_H_DISABLED
// #undef kassert

// //TODO: kassert
// #define kassert(X) do {} while (1);

// #endif
// #else

// #ifndef INCLUDED_DEBUG_H_DISABLED
// #define INCLUDED_DEBUG_H_DISABLED
// #undef INCLUDED_DEBUG_H_ENABLED
// #undef kassert


// #define kassert(X) 

// #endif

// #ifdef __cplusplus
// }
// #endif

// #endif
#include "kernel/Debug.h"
#define kassert(x) ASSERT(x)