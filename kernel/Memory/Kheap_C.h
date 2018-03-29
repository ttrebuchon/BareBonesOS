#ifndef INCLUDED_KHEAP_C_H
#define INCLUDED_KHEAP_C_H

#include <Common.h>

#ifdef __cplusplus
extern "C" {
#endif

void* kmalloc(uint32_t size, int align, addr_t* phys);
void kfree(void*);
void* kmemset(void*, int, size_t);
void* krealloc(void*, size_t);
void* kmremap(void* old_addr, size_t osize, size_t nsize, long unsigned int flags);
int kmunmap(void* addr, size_t len);
void* kmmap(void* addr, size_t len, int prot, int flags, int fileDesc, off_t off);




void* malloc(size_t);
void free(void*);
void* realloc(void*, size_t);
void* memset(void*, int, size_t);

// Expand memory to nsize, if (flags & MREMAP_MAYMOVE), function may remap
void* mremap(void* old_addr, size_t osize, size_t nsize, long unsigned int flags);

// Remove any mappings touched by the range [addr, addr + len]
int munmap(void* addr, size_t len);

// 
void* mmap(void* addr, size_t len, int prot, int flags, int fileDesc, off_t off);

// Get effective user id of process
uid_t geteuid();

// Gets page size
int getpagesize();






#ifndef MREMAP_FIXED
	#define MREMAP_FIXED 1
#endif

#ifndef MREMAP_MAYMOVE
	#define MREMAP_MAYMOVE 2
#endif

#ifdef __cplusplus
}
#endif

#endif