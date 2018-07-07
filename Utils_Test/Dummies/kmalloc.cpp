//#include <Common.h>
#include <stdint.h>
#include <cstdlib>
#include "../TestUtils/MemTracker.hh"
#include <kernel/Memory/Heap.hh>

Kernel::Memory::Heap* kheap = nullptr;

extern "C" {
	
	uint32_t kPlacement;
	

void* kmalloc(unsigned long size, int a, addr_t* p)
{
	if (a == 1)
	{
		a = sizeof(void*);
	}
	if (!kheap)
	{
		auto ptr = malloc(size);
		if (p)
		{
			*p = (addr_t)ptr;
		}
		return ptr;
	}
	else
	{
		auto ptr = kheap->alloc(size, a);
		assert(size == 0 || ptr);
		if (a > 0)
		{
			assert((addr_t)ptr % a == 0);
		}
		return ptr;
	}
}

void kfree(void* ptr)
{
	if (!kheap)
	{
		free(ptr);
	}
	else
	{
		kheap->free(ptr);
	}
}

void kmemset(void* ptr, unsigned char byte, uint32_t size)
{
	for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
	{
		*d = byte;
	}
}

void* krealloc(void* ptr, size_t nsize)
{
	if (!kheap)
	{
		return realloc(ptr, nsize);
	}
	else
	{
		return kheap->realloc(ptr, nsize);
	}
}

}