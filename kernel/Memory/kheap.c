#include "kheap.h"

extern uint32_t end;
uint32_t kPlacement = (uint32_t)&end;

void* kmalloc(uint32_t size, int align, uint32_t* phys)
{
	if (align && (kPlacement & 0xFFFFF000))
	{
		kPlacement &= 0xFFFFF000;
		kPlacement += 0x1000;
	}
	if (phys)
	{
		*phys = kPlacement;
	}
	uint32_t tmp = kPlacement;
	kPlacement += size;
	return (void*)tmp;
}

void kmemset(void* ptr, unsigned char byte, uint32_t size)
{
	for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
	{
		*d = byte;
	}
}