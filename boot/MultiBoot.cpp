#include "MultiBoot.hh"
#include "multiboot.h"

namespace boot
{
	MultiBoot* mboot = nullptr;
	
	#ifdef PAGE_SIZE
	#undef PAGE_SIZE
	#endif
	#define PAGE_SIZE 0x1000
	
	
	
	MultiBoot::MultiBoot(const multiboot* ptr) noexcept : _avail_mem(0), ptr(ptr), mmap(nullptr), mmap_len(0), free_ranges(nullptr), free_ranges_len(0)
	{
		if (ptr->flags & 1)
		{
			TRACE("Bit 0 is set!");
			TRACE("lower: ");
			TRACE(ptr->mem_lower);
			TRACE("upper: ");
			TRACE(ptr->mem_upper);
		}


		// mmap is enabled
		if (ptr->flags & (1 << 6))
		{
			mmap = (const mmap_type*)(void*)(addr_t)(ptr->mmap_addr);
			mmap_len = ptr->mmap_length;
			
			assert(mmap != nullptr);
			
			auto it = mmap;
			while (it)
			{
				if (it->type == 1)
				{
					++free_ranges_len;
				}
				it = mmap_next(it);
			}
			
			free_ranges = new const mmap_type*[free_ranges_len];
			
			size_t i = 0;
			it = mmap;
			while (it && i < free_ranges_len)
			{
				if (it->type == 1)
				{
					free_ranges[i++] = it;
				}
				it = mmap_next(it);
			}
		}
		
	}
	
	void MultiBoot::use_basic_availability() noexcept
	{
		assert(hasFlag(0));
		
		if (free_ranges)
		{
			delete[] free_ranges;
		}
		
		addr_t low_start = PAGE_SIZE;
		addr_t low_end = ((addr_t)ptr->mem_lower)*1024;
		if (low_end % PAGE_SIZE != 0)
		{
			low_end = (low_end/PAGE_SIZE)*PAGE_SIZE;
		}
		addr_t high_start = 1024*1024;
		addr_t high_end = ((addr_t)ptr->mem_upper)*1024;
		if (high_end % PAGE_SIZE != 0)
		{
			high_end = (high_end/PAGE_SIZE)*PAGE_SIZE;
		}
		
		_avail_mem = (high_end - high_start) + (low_end - low_start);
		
		auto it = mmap;
		for (auto it = mmap; it != nullptr; it = mmap_next(it))
		{
			if (it->type == 1)
			{
				continue;
			}
			
			if (it->base_addr < (addr_t)low_end)
			{
				if (it->base_addr+it->len <= (addr_t)low_end)
				{
					_avail_mem -= it->len;
				}
				else
				{
					_avail_mem -= ((addr_t)low_end - it->base_addr);
				}
			}
			else if (it->base_addr < (addr_t)high_start)
			{
				if (it->base_addr+it->len > (addr_t)high_start)
				{
					_avail_mem -= ((it->base_addr+it->len) - (addr_t)high_start);
				}
			}
			else if (it->base_addr + it->len <= (addr_t)high_end)
			{
				_avail_mem -= it->len;
			}
			else
			{
				_avail_mem -= ((addr_t)high_end - it->base_addr);
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
		
		if (_avail_mem <= 0)
		{
			if (hasFlag(0))
			{
				const_cast<MultiBoot*>(this)->use_basic_availability();
			}
		}
		
		return _avail_mem;
	}
	
	auto MultiBoot::mmap_next(const mmap_type* e) const noexcept -> const mmap_type*
	{
		if (e != nullptr)
		{
			auto addr = (addr_t)e;
			addr += e->size + sizeof(e->size);
			if (addr < ptr->mmap_addr + ptr->mmap_length)
			{
				assert(e->size > 0);
				auto it = (const mmap_type*)addr;
				return it;
			}
		}
		return nullptr;
	}
	
	bool MultiBoot::hasFlag(const int16_t index) const noexcept
	{
		return (ptr->flags & (1 << index)) > 0;
	}
}