#include "Paging.hh"
#include "kheap.hh"
#include <kernel/Interrupts.h>
#include <drivers/VGA.hh>
#include <kernel/Debug.h>
#include <kernel/utils/Bitset.hh>




extern "C" uint32_t end;


namespace Kernel { namespace Memory
{

	PageDir* kernel_dir;
	PageDir* current_dir;
	Utils::Bitset_Ptr<>* frame_collection;



	

	void* virtual_to_physical(const PageDir* dir, const void* virt_addr)
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
		unsigned int i = 0;
		for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
		{
			kernel_dir->getPage(i, 1);
		}
		TRACE_C("Pages created.\n");
		
		
		
		
		TRACE_C("Allocating frames...\n");
		i = 0;
		while (i < kPlacement + 0x1000)
		{
			kernel_dir->getPage(i, 1)->alloc_frame(0, 0);
			i += 0x1000;
		}
		TRACE_C("Frames Allocated.\n");
		
		TRACE_C("Allocating kheap frames...\n");
		for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
		{
			kernel_dir->getPage(i, 1)->alloc_frame(0, 0);
			//alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
		}
		TRACE_C("kheap frames allocated.\n");
		
		Kernel::Interrupts::register_interrupt_handler(14, page_fault);
		TRACE_C("Page fault handler registered\n");
		
		current_dir = 0x0;
		switch_page_dir(kernel_dir);
		TRACE_C("Page directory switched\n");

		//kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDR, 0, 0);
		//kheap = (KHeap*)kmalloc(sizeof(KHeap), false, 0);
		TRACE("KHeap space allocated\n");
		//new (kheap) KHeap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDR, 0, 0);   
		kheap = new KHeap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDR, 0, 0);
		TRACE_C("Kernel Heap created.\n");
		
		auto tmp_dir = kernel_dir->clone();
		TRACE_C("Kernel page directory cloned.\n");
		
		switch_page_dir(tmp_dir);
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
			phys = (uint32_t)virtual_to_physical(current_dir, &dir->tables);
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

	

	

	

	struct PageDir* create_empty_page_dir(uint32_t* phys)
	{
		struct PageDir* dir = (struct PageDir*)kmalloc(sizeof(struct PageDir), 1, phys);
		kmemset(dir, 0, sizeof(struct PageDir));
		return dir;
	}











	void init_frame_collection(uint32_t size)
	{
		typedef typename Utils::Bitset_Ptr<>::storage Unit_t;
		
		frame_collection = (Utils::Bitset_Ptr<>*)kmalloc(sizeof(Utils::Bitset_Ptr<>), 1, 0);
		auto bits = (Unit_t*)kmalloc(size, 1, 0);
		frame_collection = new (frame_collection) Utils::Bitset_Ptr<>(bits, size/sizeof(Unit_t));
		
		
	}


	int first_frame(uint32_t* index)
	{
		return first_frame_from(index, 0);
	}


	int first_frame_from(uint32_t* index, uint32_t start)
	{
		if (frame_collection->firstFalse(index, start))
		{
			return 1;
		}
		return 0;
	}

	void set_frame(uint32_t addr)
	{
		frame_collection->set(addr, true);
	}

	void clear_frame(uint32_t addr)
	{
		frame_collection->set(addr, false);
	}


	uint32_t check_frame(uint32_t addr)
	{
		return frame_collection->get(addr);
	}

	void page_fault(Registers_t regs)
	{
		Drivers::VGA::Write("PAGE_FAULT\n");
		uint32_t fault_addr;
		asm volatile ("mov %%cr2, %0" : "=r" (fault_addr));
		
		bool present = !(regs.err_code & 0x1);
		bool rw = (regs.err_code & 0x2);
		bool usr = ((regs.err_code & 0x4) != 0);
		bool reserved = ((regs.err_code & 0x8) != 0);
		bool instr_fetch = ((regs.err_code & 0x10) != 0);

		uint32_t upperBits = ((uint32_t)fault_addr) >> 22;
		uint32_t midBits = (((uint32_t)fault_addr) << 10) >> 22;
		
		//TODO
		Drivers::VGA::Write("Virtual Addr: ");
		Drivers::VGA::Write((void*)fault_addr);
		Drivers::VGA::Write("\n");
		Drivers::VGA::Write("Present: ");
		Drivers::VGA::Write(present);
		Drivers::VGA::Write("\nRead Only: ");
		Drivers::VGA::Write(rw);
		Drivers::VGA::Write("\nUser Mode: ");
		Drivers::VGA::Write(usr);
		Drivers::VGA::Write("\nReserved: ");
		Drivers::VGA::Write(reserved);
		Drivers::VGA::Write("\nInstruction Fetch: ");
		Drivers::VGA::Write(instr_fetch);
		Drivers::VGA::Write("\nInstruction Address: ");
		Drivers::VGA::Write((void*)regs.eip);
		Drivers::VGA::Write("\n");
		Drivers::VGA::Write("Physical Address: ");
		Drivers::VGA::Write(virtual_to_physical(current_dir, (void*)fault_addr));
		Drivers::VGA::Write("\n");

		// auto pg = current_dir->getPage(fault_addr, 0);
		// Drivers::VGA::Write("\n\n");
		// Drivers::VGA::Write("Read/Write: ");
		// Drivers::VGA::Write(pg->rw == 1);

		// Drivers::VGA::Write("                                             \n");
		// Drivers::VGA::Write("                                             \r");
		// Drivers::VGA::Write("Upper Bits: ");
		// Drivers::VGA::Write((void*)upperBits);
		// Drivers::VGA::Write("\n");
		// Drivers::VGA::Write("                                             \r");
		// Drivers::VGA::Write("Middle Bits: ");
		// Drivers::VGA::Write((void*)midBits);
		// Drivers::VGA::Write("\n");
		// Drivers::VGA::Write("Combined Bits:                ");
		// Drivers::VGA::Write((void*)((fault_addr >> 12)));
		
		// Drivers::VGA::Write("\nTable Present?   ");
		// Drivers::VGA::Write(current_dir->tables[upperBits].present == 1);
		// Drivers::VGA::Write("\nTable R/W?   ");
		// Drivers::VGA::Write(current_dir->tables[upperBits].rw == 1);
		// Drivers::VGA::Write("\nTable User?   ");
		// Drivers::VGA::Write(current_dir->tables[upperBits].user == 1);
		// Drivers::VGA::Write("\nTable Cache?   ");
		// Drivers::VGA::Write(current_dir->tables[upperBits].cache == 1);
		// Drivers::VGA::Write("\nTable Accessed?   ");
		// Drivers::VGA::Write(current_dir->tables[upperBits].access == 1);
		
		// ASSERT(current_dir->ref_tables[upperBits] != 0);

		// Drivers::VGA::Write("\nPage Present?   ");
		// Drivers::VGA::Write(current_dir->ref_tables[upperBits]->pages[midBits].present == 1);
		
		
		
		
		while (1);
	}



}
}