#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#include <kernel/c_cpp.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


extern uint32_t kPlacement;

EXTERN_C void* kmalloc(uint32_t size, int align, uint32_t* phys);

EXTERN_C void kmemset(void*, unsigned char, uint32_t);

#ifdef __cplusplus
}
#endif


#endif