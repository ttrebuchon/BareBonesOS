#include "kheap.h"
#include "Paging.h"

extern uint32_t end;
uint32_t kPlacement = (uint32_t)&end;

extern struct PageDir* kernel_dir;

struct KHeap* kheap = 0;

void* kmalloc(uint32_t size, int align, uint32_t* phys)
{
	if (kheap != 0)
	{
		void* addr = _alloc(size, (uint8_t)align, kheap);
		if (phys != 0)
		{
			struct Page* page = get_page((uint32_t)addr, 0, kernel_dir);
			*phys = page->frame*0x1000 + (uint32_t)addr&0xFFF;
		}
		return addr;
	}
	else
	{
		if (align == 1 && (kPlacement & 0xFFFFF000))
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
}

void free(void* p)
{
	_free(p, kheap);
}

void kmemset(void* ptr, unsigned char byte, uint32_t size)
{
	for (unsigned char* d = (unsigned char*)ptr; d < (unsigned char*)ptr + size; ++d)
	{
		*d = byte;
	}
}