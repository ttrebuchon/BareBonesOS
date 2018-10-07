#include "DeviceTarget.hh"
#include "Placeholder.hh"
#include "Driver.hh"
#include <drivers/Driver.hh>
#include <drivers/Device.hh>

#define CHAR_MEM 0x1
#define CHAR_PSEUDO_TERM 0x2
#define CHAR_PSEUDO_TERM_IN 0x3
#define CHAR_SERIAL_CONSOLE 0x4
#define CHAR_PRINTER 0x6
#define CHAR_VIRTUAL_CONSOLE_TERM 0x7
#define CHAR_MISC 0xA
#define CHAR_INPUT 0xD
#define CHAR_SOUND 0xE
#define CHAR_GENERIC_SCSI 0x15
#define CHAR_USB 0xB4



#define BLOCK_FILE_DESC 0x2
#define BLOCK_SDA 0x8
#define BLOCK_ROM 0xB
#define BLOCK_SDB 0x41
#define BLOCK_SDC 0x42



#define CHAR_MEM_NULL 0x3
#define CHAR_MEM_ZERO 0x5


namespace Kernel::FS
{
	bool DeviceTarget::identifiers(bool* block, dev_t* dev) const noexcept
	{
		if (dev)
		{
			return identifiers(block, &dev->major, &dev->minor);
		}
		else
		{
			return identifiers(block, nullptr, nullptr);
		}
	}
	
	
	class driver_dev_target : public DeviceTarget
	{
		private:
		uint32_t maj, min;
		bool blk;
		Drivers::Device* dev;
		
		public:
		
		driver_dev_target(Drivers::Device* dev, bool blk, uint32_t maj, uint32_t min) noexcept : maj(maj), min(min), blk(blk), dev(dev)
		{
			assert(dev);
			
		}
		
		
		virtual bool identifiers(bool* block, uint32_t* driver, uint32_t* device) const noexcept
		{
			if (block)
			{
				*block = blk;
			}
			
			if (driver)
			{
				*driver = maj;
			}
			
			if (device)
			{
				*device = min;
			}
			
			return true;
		}
		
		virtual uint64_t read(uint64_t start, uint64_t off, uint8_t* buf)
		{
			return dev->read(start, off, buf);
		}
		
		virtual uint64_t write(uint64_t start, uint64_t off, const uint8_t* buf)
		{
			return dev->write(start, off, buf);
		}
	};
	
	Utils::shared_ptr<DeviceTarget> DeviceTarget::find_target(Filesystem*, DeviceTargetType type, const uint32_t major, const uint32_t minor) noexcept
	{
		if (major == 0)
		{
			return nullptr;
		}
		
		auto driver = Drivers::DriverManager::at(major, (type == DeviceTargetType::Block));
		if (driver)
		{
			assert(driver->major() == major);
			auto targ = DriverTarget::Make(driver, minor);
			if (targ)
			{
				return targ;
			}
		}
		
		// WORK IN PROGRESS
		/*auto driver = Drivers::DriverManager::at(major);
		if (driver)
		{
			assert(driver->major() == major);
			TRACE_VAL(major);
			auto dev = driver->device(minor);
			if (!dev)
			{
				return nullptr;
			}
			
			return new driver_dev_target(dev, (type == DeviceTargetType::Block), major, minor);
			assert(NOT_IMPLEMENTED);
		}
		else
		{
			TRACE_VAL(major);
			TRACE_VAL(minor);
			if (major > 1)
			{
			return new PlaceholderDevice((type == DeviceTargetType::Block), major, minor);
			}
			assert(NOT_IMPLEMENTED);
		}*/
		
		return Utils::make_shared<PlaceholderDevice>((type == DeviceTargetType::Block), major, minor);
		//return new PlaceholderDevice((type == DeviceTargetType::Block), major, minor);
		
		
		return nullptr;
		assert(NOT_IMPLEMENTED);
	}
	
	Utils::shared_ptr<DeviceTarget> DeviceTarget::find_target(Filesystem* fs, DeviceTargetType type, const dev_t dev) noexcept
	{
		return find_target(fs, type, dev.major, dev.minor);
	}
}