#include "MultiBoot.hh"
#include "multiboot.h"

namespace boot
{
	MultiBoot* mboot = nullptr;
	
	
	
	
	
	MultiBoot::MultiBoot(const multiboot* ptr) noexcept : _avail_mem(0), ptr(ptr), mmap(nullptr), mmap_len(0), free_ranges(nullptr), free_ranges_len(0)
	{
		// mmap is enabled
		if (ptr->flags & (1 << 6))
		{
			mmap = (const mmap_type*)(void*)(addr_t)(ptr->mmap_addr);
			mmap_len = ptr->mmap_length;
			
			assert(mmap != nullptr);
			
			auto it = mmap;
			while (it < (void*)((addr_t)mmap + mmap_len))
			{
				if (it->type == 1)
				{
					++free_ranges_len;
				}
				it = (mmap_type*)((addr_t)it + it->size);
			}
			
			free_ranges = new const mmap_type*[free_ranges_len];
			
			
			size_t i = 0;
			it = mmap;
			while (it < (void*)((addr_t)mmap + mmap_len))
			{
				if (it->type == 1)
				{
					free_ranges[i++] = it;
				}
				it = (mmap_type*)((addr_t)it + it->size);
			}
		}
		
	}
	
	
	size_t MultiBoot::available_memory() const noexcept
	{
		if (free_ranges_len == 0)
		{
			return 0;
		}
		
		if (_avail_mem != 0)
		{
			return _avail_mem;
		}
		
		for (size_t i = 0; i < free_ranges_len; ++i)
		{
			uint64_t start = free_ranges[i]->base_addr;
			uint64_t end = start + free_ranges[i]->len;
			
			end -= (end % PAGE_SIZE);
			if (start % PAGE_SIZE != 0)
			{
				start -= (start % PAGE_SIZE);
				start += PAGE_SIZE;
			}
			
			assert((end - start) % PAGE_SIZE == 0);
			
			_avail_mem += end - start;
		}
		
		return _avail_mem;
	}
	
	auto MultiBoot::mmap_next(const mmap_type* e) const noexcept -> const mmap_type*
	{
		if (e != nullptr)
		{
			auto addr = (addr_t)e;
			addr += e->size;
			if (addr >= ptr->mmap_addr + ptr->mmap_length)
			{
				assert(e->size > 0);
				auto it = (const mmap_type*)addr;
				return it;
			}
		}
		return nullptr;
	}
	
}