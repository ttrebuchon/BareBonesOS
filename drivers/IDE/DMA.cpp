#include "DMA.hh"
#include <drivers/PortIO.h>

namespace Drivers { namespace IDE {
	
	DMADrive::DMADrive(const Channel chan, const Role role) : DMADrive(new Device(chan, role)/*&Device::Devices[2*(int)(chan == Channel::Secondary) + (int)(role == Role::Slave)]*/)
	{}
	
	DMADrive::DMADrive(Device* dev) : Disk(), BMR(), _dir(), _state(false), dev(dev), prdt(nullptr), prdt_phys(0x0)
	{
		prdt = PRDT<10>::Create(&prdt_phys);
		for (auto i = 0; i < PRDT_Size; ++i)
		{
			bufsPhys[i] = 0;
			bufs[i] = nullptr;
			bufsSize[i] = 0;
		}
	}
	
	
	
	void DMADrive::configureForSize(size_t len) const
	{
		auto fullEntries = len / (64*1024);
		for (auto i = 0; i < fullEntries; ++i)
		{
			if (bufsSize[i] < (64*1024))
			{
				bufsSize[i] = 64*1024;
				if (bufs[i] != nullptr)
				{
					free(bufs[i]);
				}
				bufs[i] = (unsigned char*)kmalloc(64*1024, 1, &bufsPhys[i]);
			}
			
			prdt->entries[i].end = 0;
			prdt->entries[i].phys_buf = bufsPhys[i];
			prdt->entries[i].count = 0;
		}
		
		auto last = (len % (64*1024) > 0) ? fullEntries : fullEntries - 1;
		prdt->entries[last].end = 1;
		if (len % (64*1024) > 0)
		{
			prdt->entries[last].count = (len % 64*1024);
			if (bufsSize[last] < (len % 64*1024))
			{
				if (bufs[last] != nullptr)
				{
					free(bufs[last]);
				}
				bufs[last] = (unsigned char*)kmalloc(len % 64*1024, 1, &bufsPhys[last]);
				bufsSize[last] = (len % 64*1024);
			}
			prdt->entries[last].phys_buf = bufsPhys[last];
		}
	}
	
	
	unsigned char* DMADrive::readSector(uint32_t lba, uint32_t len)
	{
		assert(len <= 512);
		auto data = (unsigned char*)kmalloc(len, 0, 0);
		kmemset(data, 0, 512);
		
		if (read(lba, len, data) == 0)
		{
			return data;
		}
		else
		{
			free(data);
			return nullptr;
		}
	}
	
	int DMADrive::read(size_t lba, size_t len, unsigned char* data) const
	{
		/*auto fullEntries = len / (64*1024);
		for (auto i = 0; i < fullEntries; ++i)
		{
			if (bufsSize[i] < (64*1024))
			{
				bufsSize[i] = 64*1024;
				if (bufs[i] != nullptr)
				{
					free(bufs[i]);
				}
				bufs[i] = (unsigned char*)kmalloc(64*1024, 1, &bufsPhys[i]);
			}
			
			prdt->entries[i].end = 0;
			prdt->entries[i].phys_buf = bufsPhys[i];
			prdt->entries[i].count = 0;
		}
		
		auto last = (len % (64*1024) > 0) ? fullEntries : fullEntries - 1;
		prdt->entries[last].end = 1;
		if (len % (64*1024) > 0)
		{
			prdt->entries[last].count = (len % 64*1024);
			if (bufsSize[last] < (len % 64*1024))
			{
				if (bufs[last] != nullptr)
				{
					free(bufs[last]);
				}
				bufs[last] = (unsigned char*)kmalloc(len % 64*1024, 1, &bufsPhys[last]);
				bufsSize[last] = (len % 64*1024);
			}
			prdt->entries[last].phys_buf = bufsPhys[last];
		}
		*/
		
		if (data == nullptr)
		{
			return -1;
		}
		
		configureForSize(len);
		
		
		port_byte_out(dev->BMR + (uint16_t)Register::Command, 0);
		port_long_out(dev->BMR + (uint16_t)Register::PRDT_Addr, prdt_phys);
		port_byte_out(dev->select, (uchar)dev->drive);
		port_byte_out(dev->sectorCount, len / 512 + (int)((len % 512) > 0));
		port_byte_out(dev->lbaLow, lba & 0xFF);
		port_byte_out(dev->lbaMid, (lba >> 8) & 0xFF);
		port_byte_out(dev->lbaHigh, (lba >> 16) & 0xFF);
		port_byte_out(dev->command, (uchar)ATACmd::ReadDMA);
		port_byte_out(dev->BMR + (uint16_t)Register::Command, 0x8 | 0x1);
		
		while (true)
		{
			auto stat = port_byte_in(dev->BMR + (uint16_t)Register::Status);
			auto dstat = port_byte_in(dev->status);
			
			if ((dstat & 0x04) == 0)
			{
				continue;
			}
			
			if ((dstat & ATAState::Busy) == 0)
			{
				break;
			}
		}
		
		
		auto last = (len % (64*1024) > 0) ? (len / (64*1024)) : (len / (64*1024)) - 1;
		
		unsigned char* ptr = data;
		for (int i = 0; i <= last; ++i)
		{
			memcpy(ptr, bufs[i], bufsSize[i]);
			ptr += bufsSize[i];
		}
		return 0;
	}
	
	int DMADrive::write(size_t lba, size_t len, const unsigned char* data)
	{
		configureForSize(len);
		auto last = (len % (64*1024) > 0) ? (len / (64*1024)) : (len / (64*1024)) - 1;
		
		const unsigned char* ptr = data;
		for (int i = 0; i <= last; ++i)
		{
			memcpy(bufs[i], ptr, bufsSize[i]);
			ptr += bufsSize[i];
		}
		
		
		// TODO
		
	}
	
	size_t DMADrive::capacity() const
	{
		return 512*dev->size;
	}
}
}