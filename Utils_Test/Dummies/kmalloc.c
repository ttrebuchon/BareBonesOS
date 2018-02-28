#include <Common.h>

void* kmalloc(size_t size, int a, addr_t* p)
{
	return malloc(size);
}