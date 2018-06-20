#include "PhysicalMemory.hh"
#include "Managers/PMM.hh"
#include <Utils/vector>
#include <boot/MultiBoot.hh>
#include <boot/multiboot.h>
#include "kheap.hh"

namespace Kernel { namespace Memory
{
	
	bool PhysicalMemory::changing = false;
	//typename PhysicalMemory::frames_type PhysicalMemory::frames;
	typename PhysicalMemory::frames_type PhysicalMemory::frames(nullptr, 0);
	
	PMM* PhysicalMemory::under = nullptr;
	
	
	void PhysicalMemory::Initialize() noexcept
	{
		if (!frames.empty())
		{
			return;
		}
		
		if (!boot::mboot)
		{
			return;
		}
		
		size_t avail = boot::mboot->available_memory();
		
		size_t segs_len;
		auto segs = boot::mboot->free_segments(segs_len);
		assert(segs != nullptr);
		assert(segs_len > 0);
		auto lastSeg = segs[segs_len-1];
		uint64_t max_addr = lastSeg->base_addr;
		max_addr += lastSeg->len;
		assert(max_addr > 0);
		
		auto lastPg = max_addr/PageSize;
		
		frames.resize(lastPg + 1);
		frames.setAll(false);
		
		uint32_t tmp = kPlacement/PageSize + 1;
		for (uint32_t i = 0; i < tmp; ++i)
		{
			frames[i] = true;
		}
		
		size_t mmap_len;
		auto mmap = boot::mboot->all_segments(mmap_len);
		
		assert(mmap != nullptr);
		while (mmap != nullptr)
		{
			auto start = mmap->base_addr;
			auto end = mmap->len + start;
			start /= PageSize;
			
			if (end % PageSize != 0)
			{
				end /= PageSize;
				++end;
			}
			else
			{
				end /= PageSize;
			}
			
			if (start > lastPg)
			{
				break;
			}
			
			if (end > tmp)
			{
				if (mmap->type != 1)
				{
					for (auto j = start; j < end && j <= lastPg; ++j)
					{
						frames[j] = true;
					}
				}
			}
			mmap = boot::mboot->mmap_next(mmap);
		}
	}
	
	
	
	
	addr_t PhysicalMemory::reserve(size_t& sz) noexcept
	{
		return reserveFirstFree(sz);
	}
	
	addr_t PhysicalMemory::reserveFirstFree(size_t& sz) noexcept
	{
		irq_guard lock;
		assert(under);
		
		if (changing)
		{
			// ...
		}
		assert(!changing);
		changing = true;
		
		
		// ...
		addr_t a = under->reserve(sz);
		
		
		changing = false;
		
		return a;
	}
	
	
	bool PhysicalMemory::release(addr_t a, size_t sz) noexcept
	{
		irq_guard lock;
		assert(under);
		
		if (changing)
		{
			// ...
		}
		assert(!changing);
		changing = true;
		
		
		// ...
		bool b = under->release(a, sz);
		
		
		changing = false;
		
		return b;
	}
}
}