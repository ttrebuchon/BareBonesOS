#include "DeviceTarget.hh"
#include "Placeholder.hh"

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
	
	
	DeviceTarget* DeviceTarget::find_target(Filesystem*, DeviceTargetType type, const uint32_t major, const uint32_t minor) noexcept
	{
		if (major == 0)
		{
			return nullptr;
		}
		/*TRACE_VAL(major);
		TRACE_VAL(minor);*/
		
		return new PlaceholderDevice((type == DeviceTargetType::Block), major, minor);
		
		
		return nullptr;
		assert(NOT_IMPLEMENTED);
	}
	
	DeviceTarget* DeviceTarget::find_target(Filesystem* fs, DeviceTargetType type, const dev_t dev) noexcept
	{
		return find_target(fs, type, dev.major, dev.minor);
	}
}