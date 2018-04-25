#include <Common.h>


extern "C" {
	addr_t kPlacement = 0x1000;
	
    void* kmalloc(uint32_t size, int align, addr_t* phys)
    {
        if (false)
        {
            assert(false);
        }
        else
        {
        	Kernel::irq_guard lock;
            if (align != 0 && (kPlacement & 0xFFFFF000))
            {
                kPlacement &= 0xFFFFF000;
                kPlacement += 0x1000;
            }
            if (phys)
            {
                *phys = kPlacement;
            }
            addr_t tmp = kPlacement;
            kPlacement += size;
            return (void*)tmp;
        }
    }

    void kfree(void* p)
    {
        
    }
    
    void* krealloc(void* ptr, size_t nsize)
    {
    	return nullptr;
    }

    void* kmemset(void* ptr, int c, size_t size)
    {
        for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
        {
            *d = c;
        }
        return ptr;
    }

    
}