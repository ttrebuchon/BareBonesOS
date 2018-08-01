#include "Placeholder.hh"


namespace Kernel::FS
{
	PlaceholderDevice::PlaceholderDevice(bool block, uint32_t major, uint32_t minor) : major(major), minor(minor), block(block)
	{
		
	}
	
	
	
	bool PlaceholderDevice::identifiers(bool* block, uint32_t* driver, uint32_t* device) const noexcept
	{
		if (block)
		{
			*block = this->block;
		}
		
		if (driver)
		{
			*driver = major;
		}
		
		if (device)
		{
			*device = minor;
		}
		
		return true;
	}
	
	uint64_t PlaceholderDevice::read(uint64_t, uint64_t, uint8_t*)
	{
		return 0;
	}
	
	uint64_t PlaceholderDevice::write(uint64_t, uint64_t, const uint8_t*)
	{
		return 0;
	}
}