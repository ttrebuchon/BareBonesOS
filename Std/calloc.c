#include <locale.h>
#include <stdlib.h>

void* calloc(size_t num, size_t size)
{
	void* ptr = malloc(num*size);
	memset(ptr, 0, num*size);
	return ptr;
}