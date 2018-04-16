#include "kheap.hh"
#include "kheap_c.h"


extern "C"
{
	void* malloc(size_t size)
	{
		return kmalloc(((size + 7) / 8) * 8, 0, 0);
	}
	
	void free(void* ptr)
	{
		kfree(ptr);
	}
	
	void* realloc(void* ptr, size_t nsize)
	{
		return krealloc(ptr, nsize);
	}
	
	void* memset(void* ptr, int c, size_t n)
	{
		return kmemset(ptr, c, n);
	}
	
	void* mremap(void* old_addr, size_t osize, size_t nsize, long unsigned int flags)
	{
		return kmremap(old_addr, osize, nsize, flags);
	}
	
	int munmap(void* addr, size_t len)
	{
		return kmunmap(addr, len);
	}
	
	void* mmap(void* addr, size_t len, int prot, int flags, int fileDesc, off_t off)
	{
		return kmmap(addr, len, prot, flags, fileDesc, off);
	}
	
	uid_t geteuid()
	{
		// TODO
		ASSERT(false);
	}
}