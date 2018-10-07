#include "Driver.hh"
#include <drivers/Driver.hh>
#include <drivers/Device.hh>

namespace Kernel::FS
{
	
	DriverTarget::DriverTarget(const Drivers::Driver* driver, Drivers::Device* dev, const uint32_t minor) noexcept
			: DeviceTarget(),
			driver(driver),
			target(dev),
			minor(minor)
	{
		assert(driver);
		assert(target);
	}
	
	
	bool DriverTarget::identifiers(bool* block, uint32_t* driver, uint32_t* device) const noexcept
	{
		if (block)
		{
			*block = !target->is_char();
		}
		
		if (driver)
		{
			*driver = this->driver->major();
		}
		
		if (device)
		{
			*device = minor;
		}
		
		return true;
	}
	
	uint64_t DriverTarget::read(uint64_t off, uint64_t len, uint8_t* buf)
	{
		return target->read(off, len, buf);
	}
	
	uint64_t DriverTarget::write(uint64_t off, uint64_t len, const uint8_t* buf)
	{
		return target->write(off, len, buf);
	}
	
	Utils::shared_ptr<DriverTarget> DriverTarget::Make(const Drivers::Driver* driver, const uint32_t minor)
	{
		if (driver)
		{
			auto dev = driver->device(minor);
			if (dev)
			{
				return Utils::shared_ptr<DriverTarget>(new DriverTarget(driver, dev, minor));
			}
		}
		
		return nullptr;
	}
}