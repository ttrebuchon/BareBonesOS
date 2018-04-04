
#include <Common.h>
#include <stdint.h>
#include <sys/mman.h>
#include <kernel/Memory.h>

extern "C" {

/*void* malloc(unsigned long size)
{
	return malloc(size);
}

void* realloc(void* ptr, size_t n)
{
	return realloc(ptr, n);
}

void* mremap(void* old_addr, size_t osize, size_t nsize, long unsigned int flags)
{
	
}

int munmap(void* ptr, size_t len)
{
	return kmunmap(ptr, len);
}

void* memset(void* ptr, int byte, size_t size)
{
	for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
	{
		*d = byte;
	}
	return ptr;
}

void* memcpy(void*, const void*, unsigned long)
{
	
}*/

void abort()
{
	ASSERT(false);
}

}

int main()
{
	void *p1, *p2;
	static_assert(PAGE_SIZE == 4096);
}