#include "IDEDisk.hh"
#include <kernel/Memory.h>


namespace Drivers { namespace IDE {
	
	IDEDisk::IDEDisk(bool primary, bool master) : IDEDisk(&Device::Devices[((int)!primary)*2 + (int)!master])
	{}
	
	IDEDisk::IDEDisk(Device* dev) : dev(dev)
	{
		
	}
	
	unsigned char* IDEDisk::readSector(const uint32_t lba) const
	{
		auto buf = reinterpret_cast<unsigned char*>(malloc(dev->sector_size));
		if (!readSector(lba, buf))
		{
			free(buf);
			return nullptr;
		}
		return buf;
		
	}
	
	bool IDEDisk::readSector(const uint32_t lba, unsigned char* buf) const
	{
		Device::write(dev->channel, Register::Command, 0);
		//port_byte_out(, 0);
	}
	
	
	int IDEDisk::read(size_t start, size_t len, unsigned char* buf) const
	{
		//return dev->ATA_access(false, start, len/512, 1, (addr_t)buf);
	}
	
	int IDEDisk::write(size_t start, size_t len, const unsigned char* buf)
	{
		//return dev->ATA_access(true, start, len/512, 1, (addr_t)buf);
	}
	
	size_t IDEDisk::capacity() const
	{
		return dev->size*512;
	}
	
}
}