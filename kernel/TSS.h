#ifndef INCLUDED_TSS_H
#define INCLUDED_TSS_H

#include "c_cpp.h"
#include <stdint.h>

#ifdef __cplusplus
namespace Kernel {
#endif

struct TSS
{
    CPP_STATIC TSS* myTSS;
    uint8_t someNum;
};


#ifdef __cplusplus
}
#endif

#endif