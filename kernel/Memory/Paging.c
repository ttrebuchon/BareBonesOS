#include "Paging.h"
#include "kheap.h"
#include <kernel/Interrupts.h>
#include <drivers/VGA.hh>
#include <kernel/Debug.h>

extern void copy_page_physical(uint32_t, uint32_t);

void alloc_frame(struct Page* page, int is_kernel, int is_writeable)
{
	if (page->frame != 0)
	{
		return;
	}
	
	
	uint32_t index;
	ASSERT(first_frame(&index));
	set_frame(index);
	page->present = 1;
	page->rw = is_writeable ? 1 : 0;
	page->user = is_kernel ? 0 : 1;
	page->frame = index;
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
struct PageDir* current_dir;


extern uint32_t end;
void init_paging()
{
	uint32_t mem_end = 0x1000000;
	
	TRACE_C("Initializing frame collection...\n");
	init_frame_collection(mem_end/0x1000);
	TRACE_C("Frame collection initialized.\n");
	
	TRACE_C("Allocating kernel_dir...\n");
	kernel_dir = (struct PageDir*)kmalloc(sizeof(struct PageDir), 1, 0);
	TRACE_C("kernel_dir allocated.\n");
	kmemset(kernel_dir, 0, sizeof(struct PageDir));
	
	
	kernel_dir->physicalAddress = (uint32_t)kernel_dir->physicalTables;
	
	TRACE_C("Getting kheap pages...\n");
	int i = 0;
	for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
	{
		get_page(i, 1, kernel_dir);
	}
	TRACE_C("Pages created.\n");
	
	
	
	
	TRACE_C("Allocating frames...\n");
	i = 0;
	while (i < kPlacement + 0x1000)
	{
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
		i += 0x1000;
	}
	TRACE_C("Frames Allocated.\n");
	
	TRACE_C("Allocating kheap frames...\n");
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
	{
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
	}
	TRACE_C("kheap frames allocated.\n");
	
	register_interrupt_handler(14, page_fault);
	TRACE_C("Page fault handler registered\n");
	
	switch_page_dir(kernel_dir);
	TRACE_C("Page directory switched\n");
	
	kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDR, 0, 0);
	TRACE_C("Kernel Heap created.\n");
	
	current_dir = clone_page_dir(kernel_dir);
	TRACE_C("Kernel page directory cloned.\n");

	switch_page_dir(current_dir);
	TRACE_C("Switched page directory.\n");
}

void switch_page_dir(struct PageDir* dir)
{
	current_dir = dir;
	asm volatile ("mov %0, %%cr3":: "r"(dir->physicalAddress));
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

static struct PageTable* clone_table(struct PageTable* src, uint32_t* phys)
{
	struct PageTable* dest = (struct PageTable*)kmalloc(sizeof(struct PageTable), 1, phys);
	
	kmemset(dest, 0, sizeof(struct PageTable));
	
	for (int i = 0; i < 1024; ++i)
	{
		if (!src->pages[i].frame) continue;
		
		alloc_frame(&dest->pages[i],0,0);
		
		if (src->pages[i].present)
		{
			dest->pages[i].present = 1;
		}
		if (src->pages[i].rw)
		{
			dest->pages[i].rw = 1;
		}
		if (src->pages[i].user)
		{
			dest->pages[i].user = 1;
		}
		if (src->pages[i].accessed)
		{
			dest->pages[i].accessed = 1;
		}
		if (src->pages[i].dirty)
		{
			dest->pages[i].dirty = 1;
		}
		copy_page_physical(src->pages[i].frame*0x1000, dest->pages[i].frame*0x1000);
	}
	
	return dest;
}

struct PageDir* clone_page_dir(struct PageDir* src)
{
	uint32_t phys = 0;
	struct PageDir* dest = (struct PageDir*)kmalloc(sizeof(struct PageDir),1,&phys);
	ASSERT(sizeof(struct PageDir) == 8196);
	ASSERT(dest != 0);
	ASSERT(phys != 0);
	kmemset(dest, 0, sizeof(struct PageDir));
	
	uint32_t physOffset = (uint32_t)dest->physicalTables - (uint32_t)dest;
	
	dest->physicalAddress = physOffset + phys;
	
	for (int i = 0; i < 1024; ++i)
	{
		if (!src->tables[i]) continue;
		
		if (kernel_dir->tables[i] == src->tables[i])
		{
			dest->tables[i] = src->tables[i];
			dest->physicalTables[i] = src->physicalTables[i];
		}
		else
		{
			uint32_t phys;
			dest->tables[i] = clone_table(src->tables[i], &phys);
			dest->physicalTables[i] = phys | 0x07;
		}
	}
	return dest;
}


void page_fault(Registers_t regs)
{
	c_vga_write("PAGE_FAULT\n");
	uint32_t fault_addr;
	asm volatile ("mov %%cr2, %0" : "=r" (fault_addr));
	
	int present = !(regs.err_code & 0x1);
	int rw = regs.err_code & 0x2;
	int usr = regs.err_code & 0x4;
	int reserved = regs.err_code & 0x8;
	int instr_fetch = regs.err_code & 0x10;
	
	//TODO
	
	while (1);
}