// #include "ATADisk.hh"
// #include <kernel/Memory.h>
// #include <kernel/Memory/PageDir.hh>


// namespace Drivers { namespace ATA {

// 	unsigned char ATADisk::ATA_access(unsigned char direction, unsigned int lba, unsigned char secCount, unsigned short segment_selector, unsigned int seg_off)
// 	{
// 		unsigned char lba_mode;
// 		unsigned char lba_io[6];
// 		unsigned int bus = Device::Channels[(unsigned char)dev->channel].base;
// 		unsigned int words = 256;
// 		unsigned short cyl;
// 		unsigned char head, sector;

// 		Device::Select(dev->channel, dev->drive);
// 		if (lba >= 0x10000000)
// 		{
// 			lba_mode = 2;
// 			lba_io[0] = (lba & 0x000000FF);
// 			lba_io[1] = (lba & 0x0000FF00) >> 8;
// 			lba_io[2] = (lba & 0x00FF0000) >> 16;
// 			lba_io[3] = (lba & 0xFF000000) >> 24;
// 			lba_io[4] = lba_io[5] = 0;
// 			head = 0;
// 		}
// 		else if (dev->capabilities & 0x200)
// 		{
// 			lba_mode = 1;
// 			lba_io[0] = (lba & 0x000000FF);
// 			lba_io[1] = (lba & 0x0000FF00) >> 8;
// 			lba_io[2] = (lba & 0x00FF0000) >> 16;
// 			lba_io[3] = lba_io[4] = lba_io[5] = 0;
// 			head = (lba & 0x0F000000) >> 24;
// 		}
// 		else
// 		{
// 			lba_mode = 0;
// 			sector = (lba % 63);
// 			cyl = (lba + 1 - sector) / (16 * 63);
// 			lba_io[0] = sector;
// 			lba_io[1] = (cyl >> 0) & 0xFF;
// 			lba_io[2] = (cyl >> 8) & 0xFF;
// 			lba_io[3] = lba_io[4] = lba_io[5] = 0;
// 			head = (lba + 1 - sector) % (16 * 63) / (63);
// 		}

// 		while (Device::Read(dev->channel, ATA_REG_STATUS) & ATA_STATE_BUSY) ;

// 		if (lba_mode == 0)
// 		{
			
// 			Device::Write(dev->channel, ATA_REG_DEVICE_SELECT, 0xA0 | (((unsigned char)dev->drive) << 4) | head);
// 		}
// 		else
// 		{
// 			Device::Write(dev->channel, ATA_REG_DEVICE_SELECT, 0xE0 | (((unsigned char)dev->drive) << 4) | head);
// 		}

// 		if (lba_mode == 2)
// 		{
// 			Device::Write(dev->channel, ATA_REG_SECTOR_COUNT_1, 0);
// 			Device::Write(dev->channel, ATA_REG_LBA_3, lba_io[3]);
// 			Device::Write(dev->channel, ATA_REG_LBA_4, lba_io[4]);
// 			Device::Write(dev->channel, ATA_REG_LBA_5, lba_io[5]);
// 		}

// 		Device::Write(dev->channel, ATA_REG_SECTOR_COUNT_0, secCount);
// 		Device::Write(dev->channel, ATA_REG_LBA_LOW, lba_io[0]);
// 		Device::Write(dev->channel, ATA_REG_LBA_MID, lba_io[1]);
// 		Device::Write(dev->channel, ATA_REG_LBA_HIGH, lba_io[2]);

// 		unsigned char cmd;
		

// 		if (direction == 0)
// 		{
// 			if (lba_mode == 0 || lba_mode == 1)
// 			{
// 				cmd = ATA_CMD_READ_PIO;
// 			}
// 			else if (lba_mode == 2)
// 			{
// 				cmd = ATA_CMD_READ_PIO_EXT;
// 			}
// 			else
// 			{
// 				assert(false);
// 			}
// 		}
// 		else
// 		{
// 			if (lba_mode == 0 || lba_mode == 1)
// 			{
// 				cmd = ATA_CMD_WRITE_PIO;
// 			}
// 			else if (lba_mode == 2)
// 			{
// 				cmd = ATA_CMD_WRITE_PIO_EXT;
// 			}
// 			else
// 			{
// 				assert(false);
// 			}
// 		}


// 		Device::Write(dev->channel, ATA_REG_COMMAND, cmd);

// 		if (direction == 0)
// 		{
// 			for (int i = 0; i < secCount; ++i)
// 			{
// 				if (err = Device::Poll(dev->channel, true))
// 				{
// 					return err;
// 				}

// 				ASM_CODE("pushw %es");
// 				ASM_CODE("mov %%ax, %%es" : : "a"(segment_selector));
// 				ASM_CODE("rep insw" : : "c"(words), "d"(bus), "D"(seg_off));
// 				ASM_CODE("popw %es");
// 				seg_off += (words*2);
// 			}
// 		}
// 		else
// 		{
// 			for (int i = 0; i < secCount; ++i)
// 			{
// 				Device::Poll(dev->channel, false);
// 				ASM_CODE("pushw %ds");
// 				ASM_CODE("mov %%ax, %%ds" : : "a"(segment_selector));
// 				ASM_CODE("rep outsw" : : "c"(words), "d"(bus), "S"(seg_off));
// 				ASM_CODE("popw %ds");
// 				seg_off += words*2;
// 			}

// 			Device::Write(dev->channel, ATA_REG_COMMAND, (unsigned char[]) {ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
// 			Device::Poll(dev->channel, false);
// 		}

// 		return 0;
// 	}


	
// 	ATADisk::ATADisk(bool primary, bool master) : ATADisk(new Device(primary ? ATA_PRIMARY : ATA_SECONDARY, master ? ATA_MASTER : ATA_SLAVE)/*&Device::Devices[((int)!primary)*2 + (int)!master]*/)
// 	{
		
// 	}
	
// 	ATADisk::ATADisk(Device* dev) : dev(dev), err(0)
// 	{
// 		dev->init();
// 		Device::Channels[(int)dev->channel].nIEN = 1;
// 		Device::Select(dev->channel, dev->drive);
// 		Device::Write(dev->channel, ATA_REG_CONTROL, 2);
// 	}
	
// 	unsigned char* ATADisk::readSector(const uint32_t lba) const
// 	{
// 		auto buf = reinterpret_cast<unsigned char*>(kmalloc(dev->sector_size, 0, 0x0));
// 		if (!readSector(lba, buf))
// 		{
// 			free(buf);
// 			return nullptr;
// 		}
// 		return buf;
		
// 	}
	
// 	bool ATADisk::readSector(const uint32_t lba, unsigned char* buf) const
// 	{
// 		static_assert(sizeof(void*) == sizeof(unsigned int));
// 		return (const_cast<ATADisk*>(this)->ATA_access(0, lba, 1, 0x10, (addr_t)Kernel::Memory::PageDirectory::Current->physical(buf))) == 0;




// 		Device::Write(dev->channel, ATA_REG_COMMAND, 0);
// 		Device::Write(dev->channel, ATA_REG_CONTROL, 0);

// 		Device::Poll(dev->channel, false);
// 		Device::Select(dev->channel, dev->drive);
// 		Device::Write(dev->channel, ATA_REG_SECTOR_COUNT_0, 1);
// 		Device::Write(dev->channel, ATA_REG_LBA_LOW, lba & 0xFF);
// 		Device::Write(dev->channel, ATA_REG_LBA_MID, (lba & 0xFF00) >> 8);
// 		Device::Write(dev->channel, ATA_REG_LBA_HIGH, (lba & 0xFF0000) >> 16);
// 		Device::Write(dev->channel, ATA_REG_COMMAND, ATA_CMD_READ_PIO);

// 		if (err = Device::Poll(dev->channel, true))
// 		{
// 			return false;
// 			// TODO: Handle errors
// 			assert(NOT_IMPLEMENTED);
// 		}

// 		Device::ReadBuffer(dev->channel, ATA_REG_DATA, reinterpret_cast<uint32_t*>(buf), 512 / sizeof(uint32_t));

// 		return true;

// 		//port_byte_out(, 0);

// 		// TODO
// 		ASSERT(false);
// 	}
	
	
// 	int ATADisk::read(size_t start, size_t len, unsigned char* buf) const
// 	{
// 		assert(start % 512 == 0);
// 		uint32_t sectorStart = start / 512;
// 		uint32_t sectorLen = len / 512;

// 		int readCount = 0;

// 		for (uint32_t i = 0; i < sectorLen; ++i)
// 		{
// 			if (readSector(start + i, buf + (512 * i)))
// 			{
// 				readCount += 512;
// 			}
// 			else
// 			{
// 				break;
// 			}
// 		}

// 		return readCount;
// 	}
	
// 	int ATADisk::write(size_t start, size_t len, const unsigned char* buf)
// 	{
// 		//return dev->ATA_access(true, start, len/512, 1, (addr_t)buf);
		
// 		// TODO
// 		ASSERT(false);
// 	}
	
// 	size_t ATADisk::capacity() const
// 	{
// 		return dev->size*512;
// 	}

// 	const char* ATADisk::getError()
// 	{
// 		if (err == 0)
// 		{
// 			return nullptr;
// 		}

// 		if (err == 1)
// 		{
// 			return "Device Fault";
// 		}
// 		else if (err == 2)
// 		{
// 			unsigned char st = Device::Read(dev->channel, ATA_REG_ERROR);
// 			if (st & ATA_ERROR_BAD_SECTOR)
// 			{
// 				return "Bad Sector";
// 			}
// 			else if (st & ATA_ERROR_UNCORRECTABLE_DATA)
// 			{
// 				return "Uncorrectable Data";
// 			}
// 			else if (st & ATA_ERROR_NO_MEDIA || st & ATA_ERROR_NO_MEDIA_2)
// 			{
// 				return "No Media";
// 			}
// 			else if (st & ATA_ERROR_ID_NOT_FOUND)
// 			{
// 				return "ID Not Found";
// 			}
// 			else if (st & ATA_ERROR_CMD_ABORTED)
// 			{
// 				return "Command Aborted";
// 			}
// 			else if (st & ATA_ERROR_TRACK_0_NOT_FOUND)
// 			{
// 				return "No Media or Media Error";
// 			}
// 			else if (st & ATA_ERROR_NO_ADDR)
// 			{
// 				return "No Address Mark Found";
// 			}
// 			else
// 			{
// 				assert(NOT_IMPLEMENTED);
// 			}
// 		}
// 		else if (err == 3)
// 		{
// 			return "Reads Nothing";
// 		}
// 		else if (err == 4)
// 		{
// 			return "Write Protected";
// 		}
// 		else
// 		{
// 			assert(NOT_IMPLEMENTED);
// 		}
// 	}
	
// }
// }