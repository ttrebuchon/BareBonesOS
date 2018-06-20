
#include <kernel/Memory.h>


extern "C" {
	
	void* kmremap(void* old_addr, size_t oSize, size_t nSize, long unsigned int flags)
    {
        // TODO
        assert(false);
    }

    int kmunmap(void* addr, size_t len)
    {
        // TODO
        assert(false);
    }

    void* kmmap(void* addr, size_t len, int prot, int flags, int fileDescriptor, off_t off)
    {
        // TODO
        assert(false);
    }
	
	
	
}