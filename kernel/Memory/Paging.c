#include "Paging.h"
#include "kheap.h"


void alloc_frame(struct Page* page, int is_kernel, int is_writeable)
{
	if (page->frame != 0)
	{
		return;
	}
	
	
	uint32_t index;
	if (first_frame(&index))
	{
		set_frame(index);
		page->present = 1;
		page->rw = is_writeable ? 1 : 0;
		page->user = is_kernel ? 0 : 1;
		page->frame = index;
	}
}

void free_frame(struct Page* page)
{
	if (page->frame)
	{
		clear_frame(page->frame);
		page->frame = 0x0;
	}
}




struct PageDir* kernel_dir;



void init_paging()
{
	uint32_t mem_end = 0x1000000;
	
	
	init_frame_collection(mem_end/0x1000);
	
	
	kernel_dir = (struct PageDir*)kmalloc(sizeof(struct PageDir), 1, 0);
	kmemset(kernel_dir, 0, sizeof(struct PageDir));
	
	
	
	unsigned int i = 0;
	while (i < kPlacement)
	{
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
		i += 0x1000;
	}
	
	register_interrupt_handler(14, page_fault);
	
	switch_page_dir(kernel_dir);
}

void switch_page_dir(struct PageDir* dir)
{
	asm volatile ("mov %0, %%cr3":: "r"(&dir->physicalTables));
	uint32_t cr0;
	asm volatile ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0":: "r"(cr0));
}

struct Page* get_page(uint32_t addr, int create, struct PageDir* dir)
{
	addr /= 0x1000;
	uint32_t tableIndex = addr / 1024;
	
	if (dir->tables[tableIndex])
	{
		return &dir->tables[tableIndex]->pages[addr % 1024];
	}
	else if (create)
	{
		uint32_t phys;
		dir->tables[tableIndex] = (struct PageTable*)kmalloc(sizeof(struct PageTable), 1, &phys);
		kmemset(dir->tables[tableIndex], 0, sizeof(struct PageTable));
		dir->physicalTables[tableIndex] = phys | 0x7;
		return &dir->tables[tableIndex]->pages[addr % 1024];
	}
	
	return 0x0;
	
}