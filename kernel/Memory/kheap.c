#include "Memory.h"

//TODO: Set initial value
uint32_t kPlacement = 0x0;

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
	return tmp;
}