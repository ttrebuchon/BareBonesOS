#ifndef INCLUDED_TSS_H
#define INCLUDED_TSS_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t reserved1  : 16;
    uint32_t LINK       : 16;

    uint32_t ESP0       : 32;

    uint32_t reserved2  : 16;
    uint32_t SS0        : 16;
    
    uint32_t ESP1       : 16;

    uint32_t reserved3  : 16;
    uint32_t SS1        : 16;

    uint32_t ESP2       : 32;

    uint32_t reserved4  : 16;
    uint32_t SS2        : 16;

    uint32_t CR3        : 32;

    uint32_t EIP        : 32;

    uint32_t EFLAGS     : 32;

    uint32_t EAX        : 32;

    uint32_t ECX        : 32;

    uint32_t EDX        : 32;

    uint32_t EBX        : 32;

    uint32_t ESP        : 32;

    uint32_t EBP        : 32;

    uint32_t ESI        : 32;

    uint32_t EDI        : 32;

    uint32_t reserved5  : 16;
    uint32_t ES         : 16;

    uint32_t reserved6  : 16;
    uint32_t CS         : 16;

    uint32_t reserved7  : 16;
    uint32_t SS         : 16;

    uint32_t reserved8  : 16;
    uint32_t DS         : 16;

    uint32_t reserved9  : 16;
    uint32_t FS         : 16;

    uint32_t reserved10 : 16;
    uint32_t GS         : 16;

    uint32_t reserved11 : 16;
    uint32_t LDTR       : 16;

    uint32_t IOPB_off   : 16;
    uint32_t reserved12 : 16;

} TaskStateSegment_x86_t;

extern TaskStateSegment_x86_t TSS;


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
static_assert(sizeof(TaskStateSegment_x86_t) == 104);
#endif

#endif