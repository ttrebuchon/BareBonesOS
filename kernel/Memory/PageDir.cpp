#include "PageDir.hh"
#include "Paging.hh"
#include <kernel/Debug.h>
#include "kheap.hh"



extern "C" void copy_page_physical(uint32_t, uint32_t);
extern "C" void flush_tlb_page(addr_t addr)
{
	asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

namespace Kernel { namespace Memory {

	// Page::Page() :	present(0), rw(0), user(0), reserved(0), accessed(0), dirty(0),
	// 				reserved2(0), avail(0), frame(0)
	// {

	// }

    // void Page::alloc_frame(bool is_kernel, bool is_writeable)
	// {
	// 	if (frame != 0)
	// 	{
	// 		return;
	// 	}
		
		
	// 	uint32_t index;
	// 	ASSERT(first_frame(&index));
	// 	set_frame(index);
	// 	present = 1;
	// 	rw = (is_writeable == 1) ? 1 : 0;
	// 	user = (is_kernel == 1) ? 0 : 1;
	// 	frame = index;
	// }

	// void Page::free_frame()
	// {
	// 	if (frame)
	// 	{
	// 		clear_frame(frame);
	// 		frame = 0x0;
	// 	}
	// }
	
	// bool Page::map(addr_t phys, bool writeable, bool kernel, bool overwrite)
	// {
	// 	if (frame != 0 && !overwrite)
	// 	{
	// 		return false;
	// 	}
		
	// 	present = 0;
	// 	rw = writeable;
	// 	user = !kernel;
	// 	frame = phys / 0x1000;
	// 	present = 1;
	// 	return true;
	// }




    // PageTable* PageTable::clone(addr_t* phys) const
	// {
	// 	//PageTable* dest = (struct PageTable*)kmalloc(sizeof(struct PageTable), 1, phys);
	// 	PageTable* dest = new PageTable();
	// 	if (phys != nullptr)
	// 	{
	// 		*phys = (addr_t)virtual_to_physical(current_dir, dest);
	// 		ASSERT(*phys != 0);
	// 	}
		
	// 	//kmemset(dest, 0, sizeof(struct PageTable));
		
	// 	for (int i = 0; i < 1024; ++i)
	// 	{
	// 		if (!pages[i].frame) continue;
			
	// 		dest->pages[i].alloc_frame(0,0);
			
	// 		if (pages[i].present)
	// 		{
	// 			dest->pages[i].present = 1;
	// 		}
	// 		if (pages[i].rw)
	// 		{
	// 			dest->pages[i].rw = 1;
	// 		}
	// 		if (pages[i].user)
	// 		{
	// 			dest->pages[i].user = 1;
	// 		}
	// 		if (pages[i].accessed)
	// 		{
	// 			dest->pages[i].accessed = 1;
	// 		}
	// 		if (pages[i].dirty)
	// 		{
	// 			dest->pages[i].dirty = 1;
	// 		}
	// 		copy_page_physical(pages[i].frame*0x1000, dest->pages[i].frame*0x1000);
	// 	}
		
	// 	return dest;
	// }





    // Page* PageDir::getPage(uint32_t addr, bool create)
	// {
	// 	//ASSERT(addr != 0xf0000000);
	// 	 addr /= 0x1000;
	// 	uint32_t tableIndex = addr / 1024;
	// 	ASSERT(tableIndex < 1024);

	// 	struct PageTable* entry;
	// 	if (ref_tables[tableIndex])
	// 	{
	// 		entry = ref_tables[tableIndex];
	// 		//return &tables[tableIndex]->pages[addr % 1024];
	// 	}
	// 	else if (create)
	// 	{
	// 		addr_t phys;
	// 		//ref_tables[tableIndex] = new PageTable();
	// 		ref_tables[tableIndex] = (struct PageTable*)kmalloc(sizeof(struct PageTable), 1, &phys);
	// 		kmemset(ref_tables[tableIndex], 0, sizeof(struct PageTable));
	// 		new (ref_tables[tableIndex]) PageTable();
	// 		addr_t t;
	// 		if (current_dir == nullptr)
	// 		{
	// 			t = (addr_t)ref_tables[tableIndex];
	// 		}
	// 		else
	// 		{
	// 			t = (addr_t)virtual_to_physical(kernel_dir, ref_tables[tableIndex]);
	// 		}
	// 		ASSERT(t != 0);
	// 		//Might need to get rid of the >> 12 bit?
	// 		if (t != 0)
	// 		{
	// 			ASSERT(phys == t);
	// 		}
	// 		tables[tableIndex].frame = phys >> 12;
	// 		tables[tableIndex].present = 1;
	// 		tables[tableIndex].rw = 1;
	// 		tables[tableIndex].user = 1;
	// 		tables[tableIndex].page_size = 0;
	// 		tables[tableIndex].global = 0;
	// 		tables[tableIndex].w_through = 1;
	// 		tables[tableIndex].cache = 1;
	// 		entry = ref_tables[tableIndex];
	// 	}
	// 	else
	// 	{
	// 		return 0x0;
	// 	}
		
	// 	return &entry->pages[addr % 1024];
		
	// }



    // PageDir* PageDir::clone() const
	// {
	// 	addr_t phys = 0;
	// 	PageDir* dest = (struct PageDir*)kmalloc(sizeof(struct PageDir),1,&phys);
    //     //PageDir* dest = new PageDir();
	// 	ASSERT(dest != 0);
	// 	//ASSERT(phys != 0);
	// 	kmemset(dest, 0, sizeof(struct PageDir));
		
	// 	addr_t physOffset = (addr_t)&dest->tables - (addr_t)dest;
		
	// 	//dest->physicalAddress = physOffset + phys;
		
	// 	for (int i = 0; i < 1024; ++i)
	// 	{
	// 		if (!ref_tables[i]) continue;
			
	// 		if (kernel_dir->ref_tables[i] == ref_tables[i])
	// 		{
	// 			dest->tables[i] = tables[i];
	// 			dest->ref_tables[i] = ref_tables[i];
	// 		}
	// 		else
	// 		{
	// 			// DEBUG
	// 			ASSERT(0);
				
				
	// 			addr_t phys;
    //             dest->ref_tables[i] = ref_tables[i]->clone(&phys);
	// 			addr_t phys2 = (addr_t)virtual_to_physical(this, dest->ref_tables[i]);
	// 			ASSERT(phys == phys2);
				
	// 			// DEBUG
	// 			ASSERT(phys != phys2);
				
	// 			dest->tables[i].frame = phys >> 12;
	// 			dest->tables[i].user = tables[i].user;
	// 			dest->tables[i].rw = tables[i].rw;
	// 			dest->tables[i].present = tables[i].present;
	// 		}
	// 	}
	// 	return dest;
	// }
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	PageDirectory* PageDirectory::Current = nullptr;
	
	
	PageDirectory::PageDirectory() : dir(nullptr), tables(), dir_phys(nullptr)
	{
		addr_t phys;
		dir = (_Dir*)kmalloc(sizeof(_Dir), PAGE_SIZE, &phys);
		dir_phys = (void*)phys;
		kmemset(dir, 0, sizeof(_Dir));
		kmemset(tables, 0, sizeof(Table*)*1024);
		// if (Current)
		// {
		// 	dir_phys = Current->getPhysicalAddress(&dir);
		// 	ASSERT(dir_phys != nullptr);
		// }
		// else
		// {
		// 	dir_phys = &dir;
		// }
	}
	
	PageDirectory::Page* PageDirectory::at(const void* const p, bool create) noexcept
	{
		const addr_t a = (addr_t)p;
		const size_t tableIndx = GetTableIndex(p);
		if (!tables[tableIndx])
		{
			if (!create)
			{
				return nullptr;
			}
			auto tbl = this->table(p, true);
		}
		
		return &tables[tableIndx]->at(GetPageIndex(p));
	}
	
	const PageDirectory::Page* PageDirectory::at(const void* const p) const noexcept
	{
		const size_t tableIndx = GetTableIndex(p);
		if (!tables[tableIndx])
		{
			return nullptr;
		}
		else
		{
			return &tables[tableIndx]->at(GetPageIndex(p));
		}
	}

	PageDirectory::Page* PageDirectory::at(const addr_t p, bool create) noexcept
	{
		return at((const void*)p, create);
	}
	
	const PageDirectory::Page* PageDirectory::at(const addr_t p) const noexcept
	{
		return at((const void*)p);
	}
	
	PageDirectory::Table* PageDirectory::table(const size_t n, bool create) noexcept
	{
		ASSERT(n < 1024);
		if (!tables[n])
		{
			if (!create)
			{
				return nullptr;
			}
			
			tables[n] = new Table(dir->tables[n]);
		}
		
		return tables[n];
	}
	
	PageDirectory::Table* PageDirectory::table(const void* const p, bool create) noexcept
	{
		const size_t n = GetTableIndex(p);
		if (!tables[n])
		{
			if (!create)
			{
				return nullptr;
			}
			
			tables[n] = new Table(dir->tables[n]);
		}
		
		return tables[n];
	}
	
	const PageDirectory::Table* PageDirectory::table(const size_t n) const noexcept
	{
		ASSERT(n < 1024);
		if (!tables[n])
		{
			return nullptr;
		}
		
		return tables[n];
	}
	
	const PageDirectory::Table* PageDirectory::table(const void* const p) const noexcept
	{
		const size_t n = GetTableIndex(p);
		if (!tables[n])
		{
			return nullptr;
		}
		
		return tables[n];
	}
	
	void* PageDirectory::getPhysicalAddress(const void* const p) const noexcept
	{
		const auto pg = at(p);
		if (!pg)
		{
			return nullptr;
		}
		if (!pg->present())
		{
			return nullptr;
		}
		
		return (void*)(((addr_t)pg->frame())+(((addr_t)p) & 0xfff));
	}

	void PageDirectory::switch_to() noexcept
	{
		PageDirectory::Current = this;
		switch_page_dir(dir_phys);
	}

	bool PageDirectory::flush() const noexcept
	{
		ASSERT(Current == this);
		if (Current != this)
		{
			return false;
		}
		else
		{	asm volatile("	movl %0, %%ecx" : : "r"(dir_phys));
			asm volatile("	movl %ecx, %cr3");
			// asm volatile("	movl %cr3, %eax ; \
			// 				movl %eax, %cr3");
			return true;
		}
	}

	bool PageDirectory::map(addr_t virt, addr_t phys, size_t len, bool writeable, bool kernel_only, bool overwrite) noexcept
	{
		bool success = true;
		for (size_t i = 0; i < len && success; i += PAGE_SIZE)
		{
			auto pg = at(virt + i, true);
			ASSERT(pg);
			if (pg->present() && !overwrite)
			{
				TRACE("Was present!\n");
				return false;
			}
			pg->frame((void*)(phys+i));
			ASSERT(pg->frame() == (void*)(phys+i));
			pg->read_write(writeable);
			pg->supervisor(kernel_only);
			pg->present(true);
		}
		ASSERT(success);
		return success;
	}

	bool PageDirectory::map(addr_t virt, size_t len, bool writeable, bool kernel_only) noexcept
	{
		bool success = true;
		for (size_t i = 0; i < len && success; i += PAGE_SIZE)
		{
			auto pg = at(virt + i, true);
			ASSERT(pg);
			success &= pg->allocate(writeable, kernel_only);
		}
		return success;
	}
	
	PageDirectory::Page& PageDirectory::operator[](const void* const p) noexcept
	{
		auto t = table(p, true);
		return t->at(GetPageIndex(p));
	}

	PageDirectory* PageDirectory::clone(PageDirectory* linkWith) const noexcept
	{
		// TODO
		ASSERT(false);
	}





	
	
	
	PageDirectory::Table::Table(_Table& t) noexcept : table(&t), pages(), _pages(nullptr), _pages_phys(nullptr)
	{
		addr_t phys;
		auto ptr = kmalloc(sizeof(_Pages), PAGE_SIZE, &phys);
		ASSERT(phys != 0);
		ASSERT(ptr != nullptr);
		_pages_phys = (void*)phys;
		kmemset(ptr, 0, sizeof(_Pages));
		
		_pages = (_Pages*)ptr;
		
		for (auto i = 0; i < 1024; ++i)
		{
			pages[i].page = &_pages->pages[i];
		}
		
		table->frame = (((addr_t)_pages_phys) >> 12);
		table->rw = 1;
		table->user = 1;
		table->cacheDisabled = 0;
		table->w_through = 1;
		table->present = 1;
	}
	
	PageDirectory::Page& PageDirectory::Table::operator[](size_t index) noexcept
	{
		return pages[index];
	}
	
	PageDirectory::Page& PageDirectory::Table::at(size_t index) noexcept
	{
		return pages[index];
	}
	
	const PageDirectory::Page& PageDirectory::Table::at(size_t index) const noexcept
	{
		return pages[index];
	}
	
	PageDirectory::_Table* PageDirectory::Table::operator->() noexcept
	{
		return table;
	}
	
	const PageDirectory::_Table* PageDirectory::Table::operator->() const noexcept
	{
		return table;
	}
	
	PageDirectory::_Table& PageDirectory::Table::operator*() noexcept
	{
		return *table;
	}
	
	const PageDirectory::_Table& PageDirectory::Table::operator*() const noexcept
	{
		return *table;
	}









	
	
	PageDirectory::Page::Page() : page(nullptr)
	{
		
	}
	
	PageDirectory::Page::Page(_Page& p) : page(&p)
	{
		
	}
	
	PageDirectory::_Page& PageDirectory::Page::operator*() noexcept
	{
		return *page;
	}
	
	PageDirectory::_Page* PageDirectory::Page::operator->() noexcept
	{
		return page;
	}
	
	const PageDirectory::_Page& PageDirectory::Page::operator*() const noexcept
	{
		return *page;
	}
	
	const PageDirectory::_Page* PageDirectory::Page::operator->() const noexcept
	{
		return page;
	}
	
	PageDirectory::Page& PageDirectory::Page::operator=(const void* const p) noexcept
	{
		frame(p);
		if (!page->present)
		{
			page->present = 1;
		}
		return *this;
	}
	
	void* PageDirectory::Page::frame() const noexcept
	{
		return (void*)(((addr_t)page->frame) << 12);
	}
	
	void PageDirectory::Page::frame(const void* const p) noexcept
	{
		page->frame = ((addr_t)p) >> 12;
	}

	bool PageDirectory::Page::read_write() const noexcept
	{
		return page->rw;
	}

	void PageDirectory::Page::read_write(const bool nValue) noexcept
	{
		page->rw = nValue;
	}

	bool PageDirectory::Page::supervisor() const noexcept
	{
		return !page->user;
	}

	void PageDirectory::Page::supervisor(const bool nValue) noexcept
	{
		page->user = !nValue;
	}
	
	bool PageDirectory::Page::present() const noexcept
	{
		return page->present;
	}
	
	void PageDirectory::Page::present(bool b) noexcept
	{
		page->present = b;
	}

	void PageDirectory::Page::flush() const noexcept
	{
		// ? May need the virtual address mapped to by the page, or the physical address. We'll see.
		flush_tlb_page((addr_t)this->page);
	}

	bool PageDirectory::Page::allocate(bool is_writeable, bool kernel_only) noexcept
	{
		uint32_t index;
		ASSERT(first_frame(&index));
		set_frame(index);
		
		page->rw = (is_writeable == 1) ? 1 : 0;
		page->user = (kernel_only == 1) ? 0 : 1;
		page->frame = index;
		page->present = 1;
		flush();
		return true;
	}

}
}