#ifndef INCLUDED_KHEAP_HH
#define INCLUDED_KHEAP_HH

#include <Common.h>
#include <kernel/Debug.h>
#include <Utils/OrderedArray.hh>
#include "kheap_c.h"

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000
#define KHEAP_MAX_ADDR    0xCFFFF000

extern "C" {
    extern uint32_t kPlacement;


    /*void* kmalloc(uint32_t size, int align, addr_t* phys);
    void kfree(void*);

    void kmemset(void*, unsigned char, uint32_t);

    void* malloc(size_t);
    void free(void*);
    void* krealloc(void*, size_t);
    void* realloc(void*, size_t);*/

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
}

namespace Kernel { namespace Memory {

    struct HeapComp
    {
        bool operator()(const struct KHeapHeader* left, const struct KHeapHeader* right) const
        {
            return left->size < right->size;
        }
    };

    class KHeap
    {
        private:
        Utils::OrderedArray<struct KHeapHeader*, HeapComp> index;
        uint32_t _startAddr;
        uint32_t _endAddr;
        uint32_t addrMax;
        uint8_t supervisor;
        uint8_t readonly;

        void expand(uint32_t nSize);
        uint32_t contract(uint32_t nSize);
        int32_t find_smallest_hole(uint32_t size, bool pageAlign);

        public:
        KHeap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);

        void* alloc(uint32_t size, bool page_align);
        void free(void*);
        void* realloc(void*, size_t);

        uint32_t endAddr() const;
        uint32_t startAddr() const;
    };

    extern KHeap* kheap;


    KHeap* create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);
    void* _alloc(uint32_t size, uint8_t page_align, KHeap*);
    void _free(void*, KHeap*);

}
}

#endif