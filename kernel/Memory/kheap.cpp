#include "kheap.hh"
#include "Paging.hh"


extern uint32_t end;
uint32_t kPlacement = (uint32_t)&end;

extern Kernel::Memory::PageDir* Kernel::Memory::kernel_dir;



extern "C" {
    void* kmalloc(uint32_t size, int align, uint32_t* phys)
    {
        if (Kernel::Memory::kheap != 0)
        {
            void* addr = Kernel::Memory::kheap->alloc(size, align);
            //void* addr = _alloc(size, (uint8_t)align, kheap);
            if (phys != 0)
            {
                Kernel::Memory::Page* page = Kernel::Memory::kernel_dir->getPage((uint32_t)addr, 0);
                *phys = page->frame*0x1000 + ((uint32_t)addr&0xFFF);
            }
            return addr;
        }
        else
        {
            if (align == 1 && (kPlacement & 0xFFFFF000))
            {
                kPlacement &= 0xFFFFF000;
                kPlacement += 0x1000;
            }
            if (phys)
            {
                *phys = kPlacement;
            }
            uint32_t tmp = kPlacement;
            kPlacement += size;
            return (void*)tmp;
        }
    }

    void free(void* p)
    {
        Kernel::Memory::kheap->free(p);
        //_free(p, kheap);
    }

    void kmemset(void* ptr, unsigned char byte, uint32_t size)
    {
        for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
        {
            *d = byte;
        }
    }
}

namespace Kernel { namespace Memory {

    Kernel::Memory::KHeap* kheap = 0;

    int32_t KHeap::find_smallest_hole(uint32_t size, bool pageAlign)
    {
        uint32_t i;
        for (i = 0; i < index.size(); ++i)
        {
            const KHeapHeader* header = index[i];
            if (!pageAlign)
            {
                uint32_t loc = (addr_t)header;
                int32_t offset = 0;
                if (((loc + sizeof(struct KHeapHeader)) & 0xFFFFF000) != 0)
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
        if (i == index.size())
        {
            return -1;
        }
        else
        {
            return i;
        }
    }


    KHeap::KHeap(uint32_t start, uint32_t end, uint32_t max, uint8_t sup, uint8_t readonly) : index((void*)start, HEAP_INDEX_SIZE)
    {
        //KHeap* heap = (KHeap*)kmalloc(sizeof(KHeap), false, 0x0);
        //TRACE("Heap memory allocated.\n");
        
        ASSERT(start % 0x1000 == 0);
        ASSERT(end % 0x1000 == 0);
        
        //new (&heap->index) Utils::OrderedArray<KHeapHeader*, HeapComp>((void*)start, HEAP_INDEX_SIZE);
        //TRACE("Index constructed.\n");

        ASSERT(index.size() == 0);
        ASSERT(&index[0] == (void*)start);

        start += sizeof(KHeapHeader*)*HEAP_INDEX_SIZE;

        if ((start & 0xFFFFF000) != 0)
        {
            start &= 0xFFFFF000;
            start += 0x1000;
        }

        TRACE("Setting up heap fields...\n");

        _startAddr = start;
        _endAddr = end;
        addrMax = max;
        supervisor = sup;
        readonly = readonly;

        TRACE("Creating initial hole...\n");
        KHeapHeader* hole = (KHeapHeader*)start;
        TRACE("Hole address identified.\n");
        hole->size = end - start;
        TRACE("Hole size set.\n");
        hole->magic = HEAP_MAGIC;
        TRACE("Hole magic set.\n");
        hole->is_hole = 1;
        TRACE("Hole identified as a hole.\n");
        index.insert(hole);
        TRACE("Initial hole inserted into index.\n");
    }

    void KHeap::expand(uint32_t nSize)
    {

        ASSERT(nSize > _endAddr - _startAddr);

        if ((nSize & 0xFFFFF000) != 0)
        {
            nSize &= 0xFFFFF000;
            nSize += 0x1000;
        }

        ASSERT(_startAddr + nSize <= addrMax);

        uint32_t oSize = _endAddr - _startAddr;
        uint32_t i = oSize;
        while (i < nSize)
        {
            kernel_dir->getPage(_startAddr+i, 1)->alloc_frame((supervisor) ? 1 : 0, readonly ? 0 : 1);
            i += 0x1000;
        }
        _endAddr = _startAddr + nSize;
    }

    uint32_t KHeap::contract(uint32_t nSize)
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

        uint32_t oSize = _endAddr - _startAddr;
        for (uint32_t i = oSize; i > nSize; i -= 0x1000)
        {
            kernel_dir->getPage(_startAddr+i, 0)->free_frame();
        }
        _endAddr = _startAddr + nSize;
        return nSize;
    }

    void* KHeap::alloc(uint32_t size, bool page_align)
    {
        uint32_t nSize = size + sizeof(KHeapHeader) + sizeof(KHeapFooter);
        int32_t it = find_smallest_hole(nSize, page_align);

        if (it == -1)
        {
            uint32_t oldLen = _endAddr - _startAddr;
            uint32_t old_endAddr = _endAddr;

            expand(oldLen + nSize);
            uint32_t newLen = _endAddr - _startAddr;

            it = 0;
            int32_t idx = -1;
            uint32_t value = 0x0;
            while (it < (int32_t)index.size())
            {
                uint32_t tmp = (addr_t)index[it];
                if (tmp > value)
                {
                    value = tmp;
                    idx = it;
                }
                ++it;
            }

            if (idx == -1)
            {
                KHeapHeader* head = (KHeapHeader*)old_endAddr;
                head->magic = HEAP_MAGIC;
                head->size = newLen - oldLen;
                head->is_hole = true;
                KHeapFooter* foot = (KHeapFooter*)(old_endAddr + head->size - sizeof(KHeapFooter));
                foot->magic = HEAP_MAGIC;
                foot->header = head;
                index.insert(head);
            }
            else
            {
                auto head = index[idx];
                head->size += newLen - oldLen;
                
                auto foot = (KHeapFooter*)((addr_t)head + head->size - sizeof(KHeapFooter));
                foot->magic = HEAP_MAGIC;
                foot->header = head;
            }

            return alloc(size, page_align);
        }

        KHeapHeader* origHoleHead = (KHeapHeader*)index[it];
        uint32_t origHolePos = (addr_t)origHoleHead;
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
            index.erase(it);
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

            auto holeFoot = (KHeapFooter*)((addr_t)holeHeader + origHoleSize - nSize - sizeof(KHeapFooter));
            if ((addr_t)holeFoot < _endAddr)
            {
                holeFoot->magic = HEAP_MAGIC;
                holeFoot->header = holeHeader;
            }

            index.insert(holeHeader);
        }
        ASSERT(((KHeapFooter*)((addr_t)blockHead + blockHead->size - sizeof(KHeapFooter)))->magic == HEAP_MAGIC);
        return (void*)((addr_t)blockHead + sizeof(KHeapHeader));
    }

    void KHeap::free(void* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }

        auto head = (KHeapHeader*)((addr_t)ptr - sizeof(KHeapHeader));
        auto foot = (KHeapFooter*)((addr_t)head + head->size - sizeof(KHeapFooter));

        ASSERT(head->magic == HEAP_MAGIC);
        ASSERT(foot->magic == HEAP_MAGIC);

        bool add = true;

        auto testFoot = (KHeapFooter*)((addr_t)ptr - sizeof(KHeapFooter));
        if (testFoot->magic == HEAP_MAGIC && testFoot->header->is_hole == 1)
        {
            uint32_t cachedSize = head->size;
            head = testFoot->header;
            foot->header = head;
            head->size += cachedSize;
            add = false;
        }

        auto testHead = (KHeapHeader*)((addr_t)foot + sizeof(KHeapFooter));
        if (testHead->magic == HEAP_MAGIC && testHead->is_hole)
        {
            head->size += testHead->size;
            testFoot = (KHeapFooter*)((addr_t)testHead + testHead->size - sizeof(KHeapFooter));
            foot = testFoot;

            for (uint32_t it = 0; it < index.size(); ++it)
            {
                if (index[it] != testHead)
                {
                    index.erase(it);
                    break;
                }
            }
        }
        
        
        if ((addr_t)foot + sizeof(KHeapFooter) == _endAddr)
        {
            uint32_t oldLen = _endAddr - _startAddr;
            uint32_t newLen = contract((addr_t)head - _startAddr);
            
            if (head->size - (oldLen - newLen) > 0)
            {
                head->size -= oldLen - newLen;
                foot = (KHeapFooter*)((addr_t)head + head->size - sizeof(KHeapFooter));
                foot->magic = HEAP_MAGIC;
                foot->header = head;
            }
            else
            {
                for (uint32_t it = 0; it < index.size(); ++it)
                {
                    if (index[it] == testHead)
                    {
                        index.erase(it);
                        break;
                    }
                }
            }
            
        }
        
        if (add)
        {
            index.insert(head);
        }
    }

    uint32_t KHeap::endAddr() const
    {
        return _endAddr;
    }

    uint32_t KHeap::startAddr() const
    {
        return _startAddr;
    }

}
}
