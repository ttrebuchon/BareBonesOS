#include "kheap.h"
#ifndef INCLUDED_KHEAP_HH
#define INCLUDED_KHEAP_HH

#include <kernel/utils/OrderedArray.hh>

struct HeapComp
{
    bool operator()(const struct KHeapHeader* left, const struct KHeapHeader* right) const
    {
        return left->size < right->size;
    }
};

extern "C" struct KHeap
{
    Utils::OrderedArray<struct KHeapHeader*, HeapComp> index;
    uint32_t startAddr;
    uint32_t endAddr;
    uint32_t addrMax;
    uint8_t supervisor;
    uint8_t readonly;
};

#endif