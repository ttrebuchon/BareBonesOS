//#include <Common.h>
#include <stdint.h>
#include <cstdlib>

extern "C" {
	
	uint32_t kPlacement;

void* kmalloc(unsigned long size, int a, uint64_t* p)
{
	return malloc(size);
}

void kfree(void* ptr)
{
	free(ptr);
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
	return realloc(ptr, nsize);
}

}