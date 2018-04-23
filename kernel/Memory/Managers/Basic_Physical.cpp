#include "Basic_Physical.hh"



namespace Kernel { namespace Memory
{
	Basic_Physical::Basic_Physical(frames_type& f) noexcept : PMM(f)
	{
		
	}
	
	
	addr_t Basic_Physical::reserve(size_t& size, addr_t hint) noexcept
	{
		size_t count = size / PAGE_SIZE;
		
		if (size % PAGE_SIZE != 0)
		{
			count += 1;
			size = count * PAGE_SIZE;
		}
		uint32_t index;
		if (frames.firstFalse(&index, hint / PAGE_SIZE))
		{
			bool OK = true;
			for (uint32_t i = 1; i < count && OK; ++i)
			{
				OK &= frames[index+i];
			}
			if (!OK)
			{
				return reserve(size, (((addr_t)index)+count)*PAGE_SIZE);
			}
			
			for (uint32_t i = 0; i < count; ++i)
			{
				frames[i+index] = true;
			}
			return ((addr_t)index)*PAGE_SIZE;
		}
		
		assert(false);
	}
	
	addr_t Basic_Physical::reserve(size_t& size) noexcept
	{
		return reserve(size, 0);
	}
	
	size_t Basic_Physical::release(addr_t loc, size_t _count) noexcept
	{
		auto index = loc / PAGE_SIZE;
		auto count = _count / PAGE_SIZE;
		size_t released = 0;
		if (_count % PAGE_SIZE)
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
		
		
		return released*PAGE_SIZE;
	}
}
}