#include "Paging.h"
#include <kernel/utils/Bitset.hh>
#include "kheap.h"
#include <drivers/VGA.hh>

Utils::Bitset_Ptr<>* frame_collection;

extern "C" {

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
		c_vga_write("PAGE_FAULT\n");
		uint32_t fault_addr;
		asm volatile ("mov %%cr2, %0" : "=r" (fault_addr));
		
		bool present = !(regs.err_code & 0x1);
		bool rw = (regs.err_code & 0x2 != 0);
		bool usr = (regs.err_code & 0x4 != 0);
		bool reserved = (regs.err_code & 0x8 != 0);
		bool instr_fetch = (regs.err_code & 0x10 != 0);
		
		//TODO
		Drivers::VGA::Write("Addr: ");
		Drivers::VGA::Write((void*)fault_addr);
		Drivers::VGA::Write("\n");
		Drivers::VGA::Write("Present: ");
		Drivers::VGA::Write(present);
		Drivers::VGA::Write("\nRead/Write: ");
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
		
		while (1);
	}
}