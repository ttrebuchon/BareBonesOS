#include "PageDir.hh"
#include "Paging.hh"
#include <kernel/Debug.h>
#include "kheap.hh"

extern "C" void copy_page_physical(uint32_t, uint32_t);

namespace Kernel { namespace Memory {

    void Page::alloc_frame(bool is_kernel, bool is_writeable)
	{
		if (frame != 0)
		{
			return;
		}
		
		
		uint32_t index;
		ASSERT(first_frame(&index));
		set_frame(index);
		present = 1;
		rw = (is_writeable == 1) ? 1 : 0;
		user = (is_kernel == 1) ? 0 : 1;
		frame = index;
	}

	void Page::free_frame()
	{
		if (frame)
		{
			clear_frame(frame);
			frame = 0x0;
		}
	}




    PageTable* PageTable::clone(uint32_t* phys) const
	{
		//PageTable* dest = (struct PageTable*)kmalloc(sizeof(struct PageTable), 1, phys);
		PageTable* dest = new PageTable();
		if (phys != nullptr)
		{
			*phys = (uint32_t)virtual_to_physical(current_dir, dest);
			ASSERT(*phys != 0);
		}
		
		//kmemset(dest, 0, sizeof(struct PageTable));
		
		for (int i = 0; i < 1024; ++i)
		{
			if (!pages[i].frame) continue;
			
			dest->pages[i].alloc_frame(0,0);
			
			if (pages[i].present)
			{
				dest->pages[i].present = 1;
			}
			if (pages[i].rw)
			{
				dest->pages[i].rw = 1;
			}
			if (pages[i].user)
			{
				dest->pages[i].user = 1;
			}
			if (pages[i].accessed)
			{
				dest->pages[i].accessed = 1;
			}
			if (pages[i].dirty)
			{
				dest->pages[i].dirty = 1;
			}
			copy_page_physical(pages[i].frame*0x1000, dest->pages[i].frame*0x1000);
		}
		
		return dest;
	}





    Page* PageDir::getPage(uint32_t addr, bool create)
	{
		//ASSERT(addr != 0xf0000000);
		addr /= 0x1000;
		uint32_t tableIndex = addr / 1024;
		ASSERT(tableIndex < 1024);

		struct PageTable* entry;
		if (ref_tables[tableIndex])
		{
			entry = ref_tables[tableIndex];
			//return &tables[tableIndex]->pages[addr % 1024];
		}
		else if (create)
		{
			uint32_t phys;
			//ref_tables[tableIndex] = new PageTable();
			ref_tables[tableIndex] = (struct PageTable*)kmalloc(sizeof(struct PageTable), 1, &phys);
			kmemset(ref_tables[tableIndex], 0, sizeof(struct PageTable));
			uint32_t t;
			if (current_dir == nullptr)
			{
				t = (uint32_t)ref_tables[tableIndex];
			}
			else
			{
				t = (uint32_t)virtual_to_physical(kernel_dir, ref_tables[tableIndex]);
			}
			ASSERT(t != 0);
			//Might need to get rid of the >> 12 bit?
			if (t != 0)
			{
				ASSERT(phys == t);
			}
			tables[tableIndex].frame = phys >> 12;
			tables[tableIndex].present = 1;
			tables[tableIndex].rw = 1;
			tables[tableIndex].user = 1;
			tables[tableIndex].page_size = 0;
			tables[tableIndex].global = 0;
			tables[tableIndex].w_through = 1;
			tables[tableIndex].cache = 1;
			entry = ref_tables[tableIndex];
		}
		else
		{
			return 0x0;
		}
		
		return &entry->pages[addr % 1024];
		
	}



    PageDir* PageDir::clone() const
	{
		uint32_t phys = 0;
		PageDir* dest = (struct PageDir*)kmalloc(sizeof(struct PageDir),1,&phys);
        //PageDir* dest = new PageDir();
		ASSERT(dest != 0);
		//ASSERT(phys != 0);
		kmemset(dest, 0, sizeof(struct PageDir));
		
		uint32_t physOffset = (uint32_t)&dest->tables - (uint32_t)dest;
		
		//dest->physicalAddress = physOffset + phys;
		
		for (int i = 0; i < 1024; ++i)
		{
			if (!ref_tables[i]) continue;
			
			if (kernel_dir->ref_tables[i] == ref_tables[i])
			{
				dest->tables[i] = tables[i];
				dest->ref_tables[i] = ref_tables[i];
			}
			else
			{
				ASSERT(0);
				uint32_t phys;
                dest->ref_tables[i] = ref_tables[i]->clone(&phys);
				uint32_t phys2 = (uint32_t)virtual_to_physical(this, dest->ref_tables[i]);
				ASSERT(phys == phys2);
				ASSERT(phys != phys2);
				dest->tables[i].frame = phys >> 12;
				dest->tables[i].user = tables[i].user;
				dest->tables[i].rw = tables[i].rw;
				dest->tables[i].present = tables[i].present;
			}
		}
		return dest;
	}

}
}