#include "Basic_Physical.hh"



namespace Kernel { namespace Memory
{
	Basic_Physical::Basic_Physical(frames_type& f) noexcept : PMM(f)
	{
		
	}
	
	
	addr_t Basic_Physical::reserve(size_t& size, addr_t hint) noexcept
	{
		size_t count = size / PhysicalMemory::PageSize;
		
		if (size % PhysicalMemory::PageSize != 0)
		{
			count += 1;
			size = count * PhysicalMemory::PageSize;
		}
		uint32_t index;
		if (frames.firstFalse(&index, hint / PhysicalMemory::PageSize))
		{
			bool OK = true;
			for (uint32_t i = 1; i < count && OK; ++i)
			{
				OK &= frames[index+i];
			}
			if (!OK)
			{
				return reserve(size, (((addr_t)index)+count)*PhysicalMemory::PageSize);
			}
			
			for (uint32_t i = 0; i < count; ++i)
			{
				frames[i+index] = true;
			}
			return ((addr_t)index)*PhysicalMemory::PageSize;
		}
		else
		{
			TRACE("Couldn't find free physical memory!");
			TRACE_VAL(frames.size());
			TRACE_VAL((void*)(uintptr_t)frames.size());
			TRACE("Size: ");
			TRACE(frames.size());
		}
		
		
		
		assert(false);
	}
	
	addr_t Basic_Physical::reserve(size_t& size) noexcept
	{
		return reserve(size, 0);
	}
	
	size_t Basic_Physical::release(addr_t loc, size_t _count) noexcept
	{
		auto index = loc / PhysicalMemory::PageSize;
		auto count = _count / PhysicalMemory::PageSize;
		size_t released = 0;
		if (_count % PhysicalMemory::PageSize)
		{
			++count;
		}
		if (count > 0)
		{
			for (size_t i = 0; i < count; ++i)
			{
				if (frames[i+index])
				{
					frames[i+index] = false;
					++released;
				}
			}
		}
		
		
		return released*PhysicalMemory::PageSize;
	}
}
}