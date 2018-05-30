#define protected public
#include "Paging.hh"
#include "kheap.hh"
#include <kernel/Interrupts.h>
#include <drivers/VGA.hh>
#include <kernel/Debug.h>
#include <Utils/Bitset.hh>
#include "Heaps/DumbHeap.hh"
#include "PhysicalMemory.hh"
#include "Managers/Basic_Physical.hh"




extern "C" uint32_t end;


namespace Kernel { namespace Memory
{

	PageDirectory* kernel_dir;
	// PageDir* current_dir;
	#ifdef DEBUG_IDENTITY_DIR
	PageDirectory* identity_dir;
	#endif
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
		irq_guard irq_lock;
		//asm volatile("cli");

		uint32_t mem_end = 0xF0000000;
		
		TRACE("Initializing frame collection...\n");
		init_frame_collection(mem_end/0x1000);
		TRACE("Frame collection initialized.\n");
		
		
		PhysicalMemory::Use<Basic_Physical>();
		
		
		#ifdef DEBUG_IDENTITY_DIR
		TRACE("Allocating identity_dir...\n");
		addr_t iphys;
		identity_dir = create_empty_page_dir(&iphys);
		TRACE("identity_dir allocated.\n");
		
		ASSERT(iphys == (uint32_t)&identity_dir->tables);
		Drivers::VGA::Write("Identity_Dir Physical Address: ");
		c_vga_write_addr(identity_dir);
		Drivers::VGA::Write("\n");
		
		for (addr_t q = 0; q < 0xFFFFF; ++q)
		{
			uint64_t q2 = q;
			q2 *= 0x1000;
			ASSERT(q*0x1000 == q2);
			auto pg = identity_dir->getPage(q*0x1000, true);
			ASSERT(pg->map(q*0x1000, true, false, true));
			
		}
		
		TRACE("identity_dir initialization complete.\n");
		#endif
		
		
		kernel_dir = new PageDirectory();
		TRACE("kernel_dir allocated.\n");

		TRACE("Getting kheap pages...\n");
		unsigned int i = 0;
		for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
		{
			ASSERT(kernel_dir->at(i, true) != nullptr);
			// kernel_dir->getPage(i, 1);
		}
		TRACE("Pages created.\n");

		
		
		// TRACE("Allocating kernel_dir...\n");
		// addr_t phys;
		// kernel_dir = (struct PageDir*)kmalloc(sizeof(struct PageDir), 1, &phys);
		// TRACE("kernel_dir allocated.\n");
		// kmemset(kernel_dir, 0, sizeof(struct PageDir));
		// for (int i = 0; i < sizeof(PageDir); ++i)
		// {
		// 	ASSERT(((unsigned char*)kernel_dir)[i] == 0);
		// }
		// ASSERT(phys == (uint32_t)&kernel_dir->thisPhysical());
		
		// //kernel_dir->physicalAddress = (uint32_t)kernel_dir->physicalTables;
		
		// TRACE("Getting kheap pages...\n");
		// unsigned int i = 0;
		// for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
		// {
		// 	kernel_dir->getPage(i, 1);
		// }
		// TRACE("Pages created.\n");
		
		// KHeap* kheap_tmp = (KHeap*)kmalloc(sizeof(KHeap), true, 0);
		// kmemset(kheap_tmp, 0, sizeof(KHeap));
		Heap* dheap_tmp = (Heap*)kmalloc(sizeof(DumbHeap), PAGE_SIZE, 0);
		kmemset(dheap_tmp, 0, sizeof(DumbHeap));
		
		
		TRACE("Allocating frames...\n");
		i = 0;
		while (i < kPlacement + 0x1000)
		{
			kernel_dir->at(i, true)->allocate(true, false);
			// kernel_dir->getPage(i, 1)->alloc_frame(0, 0);
			i += 0x1000;
		}
		TRACE("Frames Allocated.\n");

		

		for (i = 0; i < 0xE1000000; i += 0x1000)
		{
			
		}
		
		TRACE("Allocating kheap frames...\n");
		for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
		{
			auto pg = kernel_dir->at(i, true);
			ASSERT(pg);
			ASSERT(!pg->present());
			pg->allocate(true, false);
			ASSERT(pg->present());
			ASSERT(pg->frame() != 0);
			// kernel_dir->getPage(i, 1)->alloc_frame(0, 0);
			//alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
		}
		TRACE("kheap frames allocated.\n");
		
		Kernel::Interrupts::register_interrupt_handler(14, page_fault);
		TRACE("Page fault handler registered\n");
		
		addr_t sanity_checker = (addr_t)&mem_end;
		ASSERT(kernel_dir->dir == kernel_dir->dir_phys);
		TRACE("Switching page directories...");
		PageDirectory::Current = 0x0;
		#ifdef DEBUG_IDENTITY_DIR
		//switch_page_dir(identity_dir);
		identity_dir->switch_to();
		ASSERT(identity_dir == PageDirectory::Current);
		#else
		kernel_dir->switch_to();
		//switch_page_dir(kernel_dir);
		ASSERT(kernel_dir == PageDirectory::Current);
		#endif
		TRACE("Page directory switched\n");

		ASSERT(sanity_checker == (addr_t)&mem_end);
		ASSERT(mem_end == 0xF0000000);
		
		ASSERT(kernel_dir->dir == kernel_dir->thisPhysical());

		
		
		//TRACE("KHeap space allocated\n");
		
		new (dheap_tmp) DumbHeap(KHEAP_START, KHEAP_MAX_ADDR, 0, 0);
		kheap = dheap_tmp;

		
		//new (kheap_tmp) KHeap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDR, 0, 0);
		// kheap = kheap_tmp;
		TRACE("Kernel Heap created.\n");
		
		// auto tmp_dir = kernel_dir->clone(kernel_dir);
		// TRACE("Kernel page directory cloned.\n");
		//kmalloc(16384, 1, nullptr);
		
		#ifndef DEBUG_IDENTITY_DIR
		// tmp_dir->switch_to();
		// TRACE("Switched page directory to clone.");
		#else
		ASSERT(virtual_to_physical(current_dir, identity_dir) != 0x0);
		switch_page_dir(identity_dir);
		TRACE("Switched to identity directory\n");
		#endif
		
		//asm volatile("sti");
		asm volatile("nop");
	}

	void switch_page_dir(void* tables_phys)
	{
		Kernel::Interrupts::irq_guard lock;
		TRACE("CHANGING PAGE DIRECTORY\n");
		addr_t phys = (addr_t)tables_phys;
		// if (PageDirectory::Current == 0)
		// {
		// 	phys = (uint32_t)&dir->tables;
		// }
		// else
		// {
		// 	phys = (uint32_t)virtual_to_physical(PageDirectory::Current, &dir->tables);
		// }
		ASSERT(tables_phys != 0);
		// PageDirectory::Current = dir;
		// asm volatile ("mov %0, %%cr3":: "r"(dir->physicalAddress));
		
		asm volatile ("mov %0, %%cr3":: "r"(phys));
		uint32_t cr0;
		asm volatile ("mov %%cr0, %0" : "=r"(cr0));
		cr0 |= 0x80000000;
		__sync_synchronize();
		asm volatile ("mov %0, %%cr0":: "r"(cr0));
	}

	

	

	

	struct PageDir* create_empty_page_dir(addr_t* phys)
	{
		struct PageDir* dir = (struct PageDir*)kmalloc(sizeof(struct PageDir), PAGE_SIZE, phys);
		kmemset(dir, 0, sizeof(struct PageDir));
		return dir;
	}











	void init_frame_collection(uint32_t size)
	{
		typedef typename Utils::Bitset_Ptr<>::storage Unit_t;
		
		frame_collection = (Utils::Bitset_Ptr<>*)kmalloc(sizeof(Utils::Bitset_Ptr<>), PAGE_SIZE, 0);
		auto bits = (Unit_t*)kmalloc(size, PAGE_SIZE, 0);
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
		uint32_t fault_addr;
		asm volatile ("mov %%cr2, %0" : "=r" (fault_addr));
		Drivers::VGA::Write("PAGE_FAULT\n");
		
		bool present = ((regs.err_code & 0x1) != 0);
		bool rw = ((regs.err_code & 0x2) != 0);
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
		Drivers::VGA::Write(PageDirectory::Current->physical((void*)fault_addr));
		Drivers::VGA::Write("\n");

		Drivers::VGA::Write("Upper Bits: ");
		Drivers::VGA::Write((void*)upperBits);
		Drivers::VGA::Write("\nMiddle Bits: ");
		Drivers::VGA::Write((void*)midBits);
		Drivers::VGA::Write("\n");

		// auto pgEntry = &PageDirectory::Current->ref_tables[upperBits]->pages[midBits];
		// Drivers::VGA::Write("Entry Info: \n");
		// //Drivers::VGA::Write(pgEntry->present == 1);
		// Drivers::VGA::Write(Kernel::Memory::current_dir->tables[upperBits].present == 1);

		// Drivers::VGA::Write("\nPhys Tables Addr: ");
		// addr_t cr3;
		// asm volatile("mov %%cr3, %0" : "=r"(cr3));
		// Drivers::VGA::Write((void*)cr3);
		// Drivers::VGA::Write("\n");
		// Drivers::VGA::Write((void*)virtual_to_physical(PageDirectory::Current, (void*)0xefc05000));

		// uint32_t page_dir_index = ((uint32_t)0xefc05000)/(1024*4096); // >> 22
		// uint32_t page_table_index = (((uint32_t)0xefc05000)/4096)&0x3ff;
		// uint32_t frame_offset = ((uint32_t)0xefc05000)&0xfff;
		// Drivers::VGA::Write("\n");
		// Drivers::VGA::Write(Kernel::Memory::current_dir->tables[page_dir_index].present == 1);
		// Drivers::VGA::Write("\n");
		// Drivers::VGA::Write((void*)Kernel::Memory::current_dir->tables[page_dir_index].frame);
		// Drivers::VGA::Write("\n");
		// Drivers::VGA::Write(Kernel::Memory::current_dir->ref_tables[page_dir_index]->pages[page_table_index].present == 1);
		// Drivers::VGA::Write("\n");
		// Drivers::VGA::Write((void*)Kernel::Memory::current_dir->ref_tables[page_dir_index]->pages[page_table_index].frame);

		// // auto pg = current_dir->getPage(fault_addr, 0);
		// // Drivers::VGA::Write("\n\n");
		// // Drivers::VGA::Write("Read/Write: ");
		// // Drivers::VGA::Write(pg->rw == 1);

		// // Drivers::VGA::Write("                                             \n");
		// // Drivers::VGA::Write("                                             \r");
		// // Drivers::VGA::Write("Upper Bits: ");
		// // Drivers::VGA::Write((void*)upperBits);
		// // Drivers::VGA::Write("\n");
		// // Drivers::VGA::Write("                                             \r");
		// // Drivers::VGA::Write("Middle Bits: ");
		// // Drivers::VGA::Write((void*)midBits);
		// // Drivers::VGA::Write("\n");
		// // Drivers::VGA::Write("Combined Bits:                ");
		// // Drivers::VGA::Write((void*)((fault_addr >> 12)));
		
		// // Drivers::VGA::Write("\nTable Present?   ");
		// // Drivers::VGA::Write(current_dir->tables[upperBits].present == 1);
		// // Drivers::VGA::Write("\nTable R/W?   ");
		// // Drivers::VGA::Write(current_dir->tables[upperBits].rw == 1);
		// // Drivers::VGA::Write("\nTable User?   ");
		// // Drivers::VGA::Write(current_dir->tables[upperBits].user == 1);
		// // Drivers::VGA::Write("\nTable Cache?   ");
		// // Drivers::VGA::Write(current_dir->tables[upperBits].cache == 1);
		// // Drivers::VGA::Write("\nTable Accessed?   ");
		// // Drivers::VGA::Write(current_dir->tables[upperBits].access == 1);
		
		// // ASSERT(current_dir->ref_tables[upperBits] != 0);

		// // Drivers::VGA::Write("\nPage Present?   ");
		// // Drivers::VGA::Write(current_dir->ref_tables[upperBits]->pages[midBits].present == 1);
		
		
		
		
		while (1);
	}



}
}