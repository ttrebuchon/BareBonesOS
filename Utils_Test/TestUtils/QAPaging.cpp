#include "QAPaging.hh"
#include "QADrive.hh"
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <boot/MultiBoot.hh>
#include <kernel/Memory/PageDir.hh>




int QA::Paging::phys_fd = -1;
QAPageConfig* QA::Paging::config = nullptr;



void QA::Paging::Initialize() noexcept
{
	if (phys_fd != -1)
	{
		return;
	}
	
	delete QA::QADrive(filename, physical_size);
	
	phys_fd = ::open(filename, O_RDWR | O_APPEND);
	assert(phys_fd >= 0);
	
	
	
	
	auto mb = (multiboot*)malloc(sizeof(multiboot));
	mb->flags |= (1 << 6);
	
	
	auto dzero = ::open("/dev/zero", O_RDWR | O_APPEND);
	auto map = (multiboot_mmap_t*)::mmap((void*)0xC0000000, sizeof(multiboot_mmap_t), PROT_READ | PROT_WRITE, MAP_PRIVATE, dzero, 0);
	assert(map != MAP_FAILED);
	
	
	map->size = sizeof(multiboot_mmap_t);
	map->base_addr = 0x0;
	map->len = (uint64_t)physical_size;
	map->type = 1;
	
	int res = ::close(dzero);
	assert(res == 0);
	assert(map->type == 1);
	
	mb->mmap_addr = (uint32_t)(uint64_t)(void*)map;
	
	assert((void*)(uint64_t)mb->mmap_addr == map);
	mb->mmap_length = sizeof(multiboot_mmap_t);
	
	boot::mboot = new boot::MultiBoot(mb);
	
}


using Paging = QA::Paging;

bool Paging::map(const void* linear, const void* phys) noexcept
{
	linear = (void*)(((uintptr_t)linear / 4096)*4096);
	phys = (void*)(((uintptr_t)phys / 4096)*4096);
	
	
	auto map_res = ::mmap((void*)linear, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE, phys_fd, (size_t)phys);
	
	return (map_res != MAP_FAILED);
}

bool Paging::map(void* linear, void* phys, size_t len) noexcept
{
	len = (len % 4096 == 0 ? 0 : 1) + len/4096;
	void* linear_end = (void*)((uintptr_t)linear + len);
	if ((uintptr_t)linear_end % 4096)
	{
		linear_end = (void*)((uintptr_t)linear_end + 4096 - ((uintptr_t)linear_end % 4096));
	}
	
	linear = (void*)(((uintptr_t)linear / 4096)*4096);
	phys = (void*)(((uintptr_t)phys / 4096)*4096);
	
	
	
	for (void *lit = linear, *pit = phys; lit < linear_end; lit = (void*)((uintptr_t)lit + 4096), pit = (void*)((uintptr_t)pit + 4096))
	{
		if (!map(lit, pit))
		{
			return false;
		}
	}
	
	return true;
	
	
	/*auto map_res = ::mmap(linear, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, phys_fd, (size_t)phys);
	
	return (map_res != MAP_FAILED);*/
}

bool Paging::unmap(void* linear) noexcept
{
	assert(linear);
	linear = (void*)(((uintptr_t)linear / 4096)*4096);
	auto res = ::munmap(linear, 4096);
	return (res == 0);
}




namespace Kernel::Memory
{
	void PageDirectory::Page::frame(const void* const p) noexcept
	{
		if (present() && table()->dir() == PageDirectory::Current)
		{
			TRACE("Mapping...");
			Paging::unmap(virtual_address());
			if (p)
			{
				Paging::map(virtual_address(), p);
			}
		}
		page->frame = ((addr_t)p) >> 12;
		
	}
	
	
	void PageDirectory::switch_to() noexcept
	{
		auto old = PageDirectory::Current;
		if (old == this)
		{
			return;
		}
		
		PageDirectory::Current = this;
		
		for (int i = 0; i < 1024; ++i)
		{
			auto tbl = this->table(i);
			if (!tbl)
			{
				continue;
			}
			for (int j = 0; j < 1024; ++j)
			{
				auto pg = &tbl->at(j);
				if (pg)
				{
					if (pg->present())
					{
						pg->present(true);
					}
				}
			}
		}
	}
	
	void PageDirectory::Page::present(bool b) noexcept
	{
		assert(table());
		assert(table()->dir());
		if (table()->dir() == PageDirectory::Current)
		{
			if (!b)
			{
				if (virtual_address() && frame())
				{
					Paging::unmap(virtual_address());
				}
			}
			else if (virtual_address() && frame())
			{
				Paging::map(virtual_address(), frame());
			}
		}
		page->present = b;
	}
}