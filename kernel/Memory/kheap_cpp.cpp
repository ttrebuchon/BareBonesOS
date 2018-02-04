#include "kheap.h"
#include "Paging.h"
#include <kernel/utils/OrderedArray.hh>
#include <kernel/Debug.h>

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



static int32_t find_smallest_hole(uint32_t size, bool pageAlign, struct KHeap* heap)
{
    uint32_t i;
    for (i = 0; i < heap->index.size(); ++i)
    {
        const KHeapHeader* header = heap->index[i];
        if (!pageAlign)
        {
            uint32_t loc = (uint32_t)header;
            int32_t offset = 0;
            if (loc + sizeof(struct KHeapHeader) & 0xFFFFF000 != 0)
            {
                offset = 0x1000 - (loc+sizeof(struct KHeapHeader))%0x1000;
            }
            int32_t holeSize = (int32_t)header->size - offset;
            if (holeSize >= (int32_t)size)
            {
                break;
            }
        }
        else if (header->size >= size)
        {
            break;
        }
    }
    if (i == heap->index.size())
    {
        return -1;
    }
    else
    {
        return i;
    }
}


EXTERN_C struct KHeap* create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t sup, uint8_t readonly)
{
    KHeap* heap = (KHeap*)kmalloc(sizeof(KHeap), false, 0x0);
    
    ASSERT(start % 0x1000 == 0);
    ASSERT(end % 0x1000 == 0);
    
    new (&heap->index) Utils::OrderedArray<KHeapHeader*, HeapComp>((void*)start, HEAP_INDEX_SIZE);
    ASSERT(heap->index.size() == 0);
    ASSERT(&heap->index[0] == (void*)start);

    start += sizeof(KHeapHeader*)*HEAP_INDEX_SIZE;

    if (start & 0xFFFFF000 != 0)
    {
        start &= 0xFFFFF000;
        start += 0x1000;
    }

    heap->startAddr = start;
    heap->endAddr = end;
    heap->addrMax = max;
    heap->supervisor = sup;
    heap->readonly = readonly;

    KHeapHeader* hole = (KHeapHeader*)start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = 1;
    heap->index.insert(hole);
    return heap;
}

static void expand(uint32_t nSize, KHeap* heap)
{

    ASSERT(nSize > heap->endAddr - heap->startAddr);

    if (nSize & 0xFFFFF000 != 0)
    {
        nSize &= 0xFFFFF000;
        nSize += 0x1000;
    }

    ASSERT(heap->startAddr + nSize <= heap->addrMax);

    uint32_t oSize = heap->endAddr - heap->startAddr;
    uint32_t i = oSize;
    while (i < nSize)
    {
        alloc_frame(get_page(heap->startAddr+i, 1, kernel_dir), (heap->supervisor) ? 1 : 0, heap->readonly ? 0 : 1);
        i += 0x1000;
    }
    heap->endAddr = heap->startAddr + nSize;
}

static uint32_t contract(uint32_t nSize, KHeap* heap)
{
    if (nSize & 0x1000)
    {
        nSize &= 0x1000;
        nSize += 0x1000;
    }

    if (nSize < HEAP_MIN_SIZE)
    {
        nSize = HEAP_MIN_SIZE;
    }

    uint32_t oSize = heap->endAddr - heap->startAddr;
    for (uint32_t i = oSize; i > nSize; i -= 0x1000)
    {
        free_frame(get_page(heap->startAddr+i, 0, kernel_dir));
    }
    heap->endAddr = heap->startAddr + nSize;
    return nSize;
}

EXTERN_C void* _alloc(uint32_t size, uint8_t page_align, struct KHeap* heap)
{
    uint32_t nSize = size + sizeof(KHeapHeader) + sizeof(KHeapFooter);
    int32_t it = find_smallest_hole(nSize, page_align, heap);

    if (it == -1)
    {
        uint32_t oldLen = heap->endAddr - heap->startAddr;
        uint32_t oldEndAddr = heap->endAddr;

        expand(oldLen + nSize, heap);
        uint32_t newLen = heap->endAddr - heap->startAddr;

        it = 0;
        size_t idx = -1;
        uint32_t value = 0x0;
        while (it < heap->index.size())
        {
            uint32_t tmp = (uint32_t)heap->index[it];
            if (tmp > value)
            {
                value = tmp;
                idx = it;
            }
            ++it;
        }

        if (idx == -1)
        {
            KHeapHeader* head = (KHeapHeader*)oldEndAddr;
            head->magic = HEAP_MAGIC;
            head->size = newLen - oldLen;
            head->is_hole = true;
            KHeapFooter* foot = (KHeapFooter*)(oldEndAddr + head->size - sizeof(KHeapFooter));
            foot->magic = HEAP_MAGIC;
            foot->header = head;
            heap->index.insert(head);
        }
        else
        {
            auto head = heap->index[idx];
            head->size += newLen - oldLen;
            
            auto foot = (KHeapFooter*)((uint32_t)head + head->size - sizeof(KHeapFooter));
            foot->magic = HEAP_MAGIC;
            foot->header = head;
        }

        return _alloc(size, page_align, heap);
    }

    KHeapHeader* origHoleHead = (KHeapHeader*)heap->index[it];
    uint32_t origHolePos = (uint32_t)origHoleHead;
    uint32_t origHoleSize = origHoleHead->size;

    if (origHoleSize - nSize < sizeof(KHeapHeader)+sizeof(KHeapFooter))
    {
        size += origHoleSize-nSize;
        nSize = origHoleSize;
    }

    if (page_align && origHolePos & 0xFFFFF000)
    {
        uint32_t newLoc = origHolePos + 0x1000 - (origHoleSize&0xFFF) - sizeof(KHeapHeader);
        KHeapHeader* holeHead = (KHeapHeader*)origHolePos;
        holeHead->size = 0x1000 - (origHolePos&0xFFF) - sizeof(KHeapHeader);
        holeHead->magic = HEAP_MAGIC;
        holeHead->is_hole = true;
        KHeapFooter* holeFoot = (KHeapFooter*)((uint32_t)newLoc - sizeof(KHeapFooter));
        holeFoot->magic = HEAP_MAGIC;
        holeFoot->header = holeHead;
        origHolePos = newLoc;
        origHoleSize -= holeHead->size;
    }
    else
    {
        heap->index.erase(it);
    }

    KHeapHeader* blockHead = (KHeapHeader*)origHolePos;
    blockHead->magic = HEAP_MAGIC;
    blockHead->is_hole = false;
    blockHead->size = nSize;

    KHeapFooter* blockFoot = (KHeapFooter*)(origHolePos + sizeof(KHeapHeader) + size);
    blockFoot->magic = HEAP_MAGIC;
    blockFoot->header = blockHead;

    if (origHoleSize - nSize > 0)
    {
        KHeapHeader* holeHeader = (KHeapHeader*)(origHolePos + sizeof(KHeapHeader) + size + sizeof(KHeapFooter));
        holeHeader->magic = HEAP_MAGIC;
        holeHeader->is_hole = true;
        holeHeader->size = origHoleSize - nSize;

        auto holeFoot = (KHeapFooter*)((uint32_t)holeHeader + origHoleSize - nSize - sizeof(KHeapFooter));
        if ((uint32_t)holeFoot < heap->endAddr)
        {
            holeFoot->magic = HEAP_MAGIC;
            holeFoot->header = holeHeader;
        }

        heap->index.insert(holeHeader);
    }
    ASSERT(((KHeapFooter*)((uint32_t)blockHead + blockHead->size - sizeof(KHeapFooter)))->magic == HEAP_MAGIC);
    return (void*)((uint32_t)blockHead + sizeof(KHeapHeader));
}

EXTERN_C void _free(void* ptr, struct KHeap* heap)
{
    if (ptr == nullptr)
    {
        return;
    }

    auto head = (KHeapHeader*)((uint32_t)ptr - sizeof(KHeapHeader));
    auto foot = (KHeapFooter*)((uint32_t)head + head->size - sizeof(KHeapFooter));

    ASSERT(head->magic == HEAP_MAGIC);
    ASSERT(foot->magic == HEAP_MAGIC);

    bool add = true;

    auto testFoot = (KHeapFooter*)((uint32_t)ptr - sizeof(KHeapFooter));
    if (testFoot->magic == HEAP_MAGIC && testFoot->header->is_hole == 1)
    {
        uint32_t cachedSize = head->size;
        head = testFoot->header;
        foot->header = head;
        head->size += cachedSize;
        add = false;
    }

    auto testHead = (KHeapHeader*)((uint32_t)foot + sizeof(KHeapFooter));
    if (testHead->magic == HEAP_MAGIC && testHead->is_hole)
    {
        head->size += testHead->size;
        testFoot = (KHeapFooter*)((uint32_t)testHead + testHead->size - sizeof(KHeapFooter));
        foot = testFoot;

        for (uint32_t it = 0; it < heap->index.size(); ++it)
        {
            if (heap->index[it] != testHead)
            {
                heap->index.erase(it);
                break;
            }
        }
    }
    
    
    if ((uint32_t)foot + sizeof(KHeapFooter) == heap->endAddr)
    {
    	uint32_t oldLen = heap->endAddr - heap->startAddr;
    	uint32_t newLen = contract((uint32_t)head - heap->startAddr, heap);
    	
    	if (head->size - (oldLen - newLen) > 0)
    	{
    		head->size -= oldLen - newLen;
    		foot = (KHeapFooter*)((uint32_t)head + head->size - sizeof(KHeapFooter));
    		foot->magic = HEAP_MAGIC;
    		foot->header = head;
    	}
    	else
    	{
    		for (uint32_t it = 0; it < heap->index.size(); ++it)
    		{
    			if (heap->index[it] == testHead)
    			{
    				heap->index.erase(it);
    				break;
    			}
    		}
    	}
    	
    }
    
    if (add)
    {
    	heap->index.insert(head);
    }
}