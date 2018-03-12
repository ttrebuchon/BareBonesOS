//#include <Common.h>
#include <stdint.h>
#include <cstdlib>

extern "C" {

void* kmalloc(unsigned long size, int a, uint64_t* p)
{
	return malloc(size);
}

void kmemset(void* ptr, unsigned char byte, uint32_t size)
{
	for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
	{
		*d = byte;
	}
}

}