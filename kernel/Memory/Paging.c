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
	page->rw = (is_writeable == 1) ? 1 : 0;
	page->user = (is_kernel == 1) ? 0 : 1;
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

void* virtual_to_physical(struct PageDir* dir, void* virt_addr)
{
	uint32_t page_dir_index = ((uint32_t)virt_addr)/(1024*4096); // >> 22
	uint32_t page_table_index = (((uint32_t)virt_addr)/4096)&0x3ff;
	uint32_t frame_offset = ((uint32_t)virt_addr)&0xfff;

	ASSERT((((uint32_t)virt_addr) >> 22) == page_dir_index);
	ASSERT(((((uint32_t)virt_addr) >> 12) & 0x3ff) == page_table_index);
	ASSERT((((uint32_t)virt_addr) & 0xfff) == frame_offset);

	if (!dir->ref_tables[page_dir_index])
	{
		return 0x0;
	}
	
	struct PageTable* table = dir->ref_tables[page_dir_index];
	if (!table->pages[page_table_index].present)
	{
		return 0x0;
	}

	uint32_t t = table->pages[page_table_index].frame;
	t = (t << 12) + frame_offset;
	return (void*)t;
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
	uint32_t phys;
	kernel_dir = (struct PageDir*)kmalloc(sizeof(struct PageDir), 1, &phys);
	TRACE_C("kernel_dir allocated.\n");
	kmemset(kernel_dir, 0, sizeof(struct PageDir));
	ASSERT(phys == (uint32_t)&kernel_dir->tables);
	
	//kernel_dir->physicalAddress = (uint32_t)kernel_dir->physicalTables;
	
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
	
	current_dir = 0x0;
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
	uint32_t phys;
	if (current_dir == 0)
	{
		phys = (uint32_t)&dir->tables;
	}
	else
	{
		phys = virtual_to_physical(current_dir, &dir->tables);
	}
	ASSERT(phys != 0);
	current_dir = dir;
	// asm volatile ("mov %0, %%cr3":: "r"(dir->physicalAddress));
	
	asm volatile ("mov %0, %%cr3":: "r"(phys));
	uint32_t cr0;
	asm volatile ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0":: "r"(cr0));
}

struct Page* get_page(uint32_t addr, int create, struct PageDir* dir)
{
	//ASSERT(addr != 0xf0000000);
	addr /= 0x1000;
	uint32_t tableIndex = addr / 1024;


	struct PageTable* entry;
	if (dir->ref_tables[tableIndex])
	{
		entry = dir->ref_tables[tableIndex];
		//return &dir->tables[tableIndex]->pages[addr % 1024];
	}
	else if (create)
	{
		uint32_t phys;
		dir->ref_tables[tableIndex] = (struct PageTable*)kmalloc(sizeof(struct PageTable), 1, &phys);
		kmemset(dir->ref_tables[tableIndex], 0, sizeof(struct PageTable));
		uint32_t t = (uint32_t)virtual_to_physical(kernel_dir, dir->ref_tables[tableIndex]);
		//Might need to get rid of the >> 12 bit?
		c_vga_write_addr((void*)t);
		c_vga_write("\n");
		c_vga_write_addr((void*)phys);
		c_vga_write("\n");
		//ASSERT(t >> 12 == phys);
		if (t != 0)
		{
			ASSERT(phys == t);
		}
		dir->tables[tableIndex].frame = phys >> 12;
		dir->tables[tableIndex].present = 1;
		dir->tables[tableIndex].rw = 1;
		dir->tables[tableIndex].user = 1;
		dir->tables[tableIndex].page_size = 0;
		entry = dir->ref_tables[tableIndex];
	}
	else
	{
		return 0x0;
	}
	
	return &entry->pages[addr % 1024];
	
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
	ASSERT(sizeof(struct PageDir_Entry) == 4);
	ASSERT(sizeof(struct PageDir) == 8192);
	ASSERT(dest != 0);
	ASSERT(phys != 0);
	kmemset(dest, 0, sizeof(struct PageDir));
	
	uint32_t physOffset = (uint32_t)&dest->tables - (uint32_t)dest;
	
	//dest->physicalAddress = physOffset + phys;
	
	for (int i = 0; i < 1024; ++i)
	{
		if (!src->ref_tables[i]) continue;
		
		if (kernel_dir->ref_tables[i] == src->ref_tables[i])
		{
			dest->tables[i] = src->tables[i];
			dest->ref_tables[i] = src->ref_tables[i];
		}
		else
		{
			ASSERT(0);
			uint32_t phys;
			dest->ref_tables[i] = clone_table(src->ref_tables[i], &phys);
			uint32_t phys2 = (uint32_t)virtual_to_physical(src, dest->ref_tables[i]);
			ASSERT(phys == phys2);
			ASSERT(phys != phys2);
			dest->tables[i].frame = phys >> 12;
			dest->tables[i].user = src->tables[i].user;
			dest->tables[i].rw = src->tables[i].rw;
			dest->tables[i].present = src->tables[i].present;
		}
	}
	return dest;
}


