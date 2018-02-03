#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#include <Common.h>


#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

#ifdef __cplusplus
extern "C" {
#endif


extern uint32_t kPlacement;

EXTERN_C void* kmalloc(uint32_t size, int align, uint32_t* phys);

EXTERN_C void kmemset(void*, unsigned char, uint32_t);

EXTERN_C void* malloc(size_t);
EXTERN_C void free(void*);


struct KHeapHeader
{
    uint32_t magic;
    uint8_t is_hole;
    uint32_t size;
};

struct KHeapFooter
{
      uint32_t magic;
      struct KHeapHeader* header;
};


struct KHeap;

struct KHeap* create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);
void* _alloc(uint32_t size, uint8_t page_align, struct KHeap*);
void _free(void*, struct KHeap*);


#ifdef __cplusplus
}
#endif


#endif