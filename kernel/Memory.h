
#ifdef __cplusplus
    #include "Memory/kheap.hh"

#else
    void* kmalloc(uint32_t size, int align, uint32_t* phys);

    void kmemset(void*, unsigned char, uint32_t);

    void* malloc(size_t);
    void free(void*);
#endif

#include "Memory/memcpy.h"

