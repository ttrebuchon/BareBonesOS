
//#include <Common.h>
#include <stdint.h>

extern "C" {

void* malloc(unsigned long size)
{
	return malloc(size);
}

void memset(void* ptr, int byte, uint32_t size)
{
	for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
	{
		*d = byte;
	}
}

void* memcpy(void*, const void*, unsigned long)
{
	
}

}

int main()
{
	
}