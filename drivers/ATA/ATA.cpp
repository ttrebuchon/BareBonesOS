#include "ATA.hh"
#include <drivers/PortIO.h>
#include <drivers/PCI.hh>
#include <kernel/Debug.h>
#include <drivers/IDE/IDE_Symbols.h>
#include <drivers/COM.h>


// https://wiki.osdev.org/IDE


namespace Drivers { namespace ATA
{


	





	ATADevice::ATADevice(IDE::ChannelRegister_t* channel, unsigned char role) : IDE::IDEDevice(channel, role)
	{
		this->type = IDE::Interface::ATA;
	}


	unsigned char ATADevice::access(const bool write, uint32_t LBA, unsigned char sector_count, unsigned short segment_selector, uint32_t segment_offset)
	{
		unsigned char LBA_mode;
		unsigned char LBA_io[6];
		unsigned int words = 256;
		unsigned short cyl, i;
		unsigned char head, sect, err = 0;
		uint32_t bus = channel->base;

		channel->select(this->role);
		channel->poll(false);


		if (LBA >= 0x10000000)
		{
			LBA_mode = 2;
			LBA_io[0] = (LBA & 0x000000FF);
			LBA_io[1] = (LBA & 0x0000FF00) >> 8;
			LBA_io[2] = (LBA & 0x00FF0000) >> 16;
			LBA_io[3] = (LBA & 0xFF000000) >> 24;
			LBA_io[4] = LBA_io[5] = 0;
			head = 0;
		}
		else if (features & 0x200)
		{
			LBA_mode = 1;
			LBA_io[0] = (LBA & 0x000000FF);
			LBA_io[1] = (LBA & 0x0000FF00) >> 8;
			LBA_io[2] = (LBA & 0x00FF0000) >> 16;
			LBA_io[3] = LBA_io[4] = LBA_io[5] = 0;
			head = (LBA & 0x0F000000) >> 24;
		}
		else
		{
			LBA_mode = 0;
			sect = (LBA % 63);
			cyl = (LBA + 1 - sect) / (16 * 63);
			LBA_io[0] = sect;
			LBA_io[1] = (cyl >> 0) & 0xFF;
			LBA_io[2] = (cyl >> 8) & 0xFF;
			LBA_io[3] = LBA_io[4] = LBA_io[5] = 0;
			head = (LBA + 1 - sect) % (16 * 63) / (63);
		}

		while (channel->read(IDE_REG_STATUS) & IDE_STATE_BUSY) ;

		unsigned short slave_bit = (this->role == IDE_SLAVE);
		if (LBA_mode == 0)
		{
			
			channel->write(IDE_REG_DEVICE_SELECT, 0xA0 | (slave_bit << 4) | head);
		}
		else
		{
			channel->write(IDE_REG_DEVICE_SELECT, 0xE0 | (slave_bit << 4) | head);
		}

		if (LBA_mode == 2)
		{
			channel->write(ATA_REG_SECTOR_COUNT_1, 0);
			channel->write(ATA_REG_LBA_3, LBA_io[3]);
			channel->write(ATA_REG_LBA_4, LBA_io[4]);
			channel->write(ATA_REG_LBA_5, LBA_io[5]);
		}

		channel->write(ATA_REG_SECTOR_COUNT_0, sector_count);
		channel->write(ATA_REG_LBA_LOW, LBA_io[0]);
		channel->write(ATA_REG_LBA_MID, LBA_io[1]);
		channel->write(ATA_REG_LBA_HIGH, LBA_io[2]);

		unsigned char cmd;
		

		if (!write)
		{
			if (LBA_mode == 0 || LBA_mode == 1)
			{
				cmd = ATA_CMD_READ_PIO;
			}
			else if (LBA_mode == 2)
			{
				cmd = ATA_CMD_READ_PIO_EXT;
			}
			else
			{
				assert(false);
				__builtin_unreachable();
			}
		}
		else
		{
			if (LBA_mode == 0 || LBA_mode == 1)
			{
				cmd = ATA_CMD_WRITE_PIO;
			}
			else if (LBA_mode == 2)
			{
				cmd = ATA_CMD_WRITE_PIO_EXT;
			}
			else
			{
				assert(false);
				__builtin_unreachable();
			}
		}

		channel->write(IDE_REG_COMMAND, cmd);

		if (!write)
		{
			for (i = 0; i < sector_count; ++i)
			{
				if (err == channel->poll(true))
				{
					return err;
				}

				ASM_CODE("pushw %%es; \
				mov %%ax, %%es; \
				rep insw; \
				popw %%es" : : "a"(segment_selector), "c"(words), "d"(bus), "D"(segment_offset) : "memory");
				segment_offset += (words*2);
			}
		}
		else
		{
			for (i = 0; i < sector_count; ++i)
			{
				channel->poll(false);
				ASM_CODE("pushw %%ds; \
				mov %%ax, %%ds; \
				rep outsw; \
				popw %%ds" : : "a"(segment_selector), "c"(words), "d"(bus), "S"(segment_offset));
				segment_offset += (words*2);
			}

			constexpr static unsigned char flush_options[] = {ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT};

			channel->write(IDE_REG_COMMAND, flush_options[LBA_mode]);
			channel->poll(false);
		}
		return 0;

		
	}



	unsigned char ATADevice::access(const bool write, uint32_t LBA, unsigned char sector_count, uint8_t* buf)
	{
		assert(buf);

		unsigned char LBA_mode;
		unsigned char LBA_io[6];
		unsigned int words = 256;
		unsigned short cyl, i;
		unsigned char head, sect, err = 0;
		uint32_t bus = channel->base;

		channel->select(this->role);
		channel->poll(false);


		if (LBA >= 0x10000000)
		{
			LBA_mode = 2;
			LBA_io[0] = (LBA & 0x000000FF);
			LBA_io[1] = (LBA & 0x0000FF00) >> 8;
			LBA_io[2] = (LBA & 0x00FF0000) >> 16;
			LBA_io[3] = (LBA & 0xFF000000) >> 24;
			LBA_io[4] = LBA_io[5] = 0;
			head = 0;
		}
		else if (features & 0x200)
		{
			LBA_mode = 1;
			LBA_io[0] = (LBA & 0x000000FF);
			LBA_io[1] = (LBA & 0x0000FF00) >> 8;
			LBA_io[2] = (LBA & 0x00FF0000) >> 16;
			LBA_io[3] = LBA_io[4] = LBA_io[5] = 0;
			head = (LBA & 0x0F000000) >> 24;
		}
		else
		{
			LBA_mode = 0;
			sect = (LBA % 63);
			cyl = (LBA + 1 - sect) / (16 * 63);
			LBA_io[0] = sect;
			LBA_io[1] = (cyl >> 0) & 0xFF;
			LBA_io[2] = (cyl >> 8) & 0xFF;
			LBA_io[3] = LBA_io[4] = LBA_io[5] = 0;
			head = (LBA + 1 - sect) % (16 * 63) / (63);
		}

		while (channel->read(IDE_REG_STATUS) & IDE_STATE_BUSY) ;

		unsigned short slave_bit = (this->role == IDE_SLAVE);
		if (LBA_mode == 0)
		{
			
			channel->write(IDE_REG_DEVICE_SELECT, 0xA0 | (slave_bit << 4) | head);
		}
		else
		{
			channel->write(IDE_REG_DEVICE_SELECT, 0xE0 | (slave_bit << 4) | head);
		}

		if (LBA_mode == 2)
		{
			channel->write(ATA_REG_SECTOR_COUNT_1, 0);
			channel->write(ATA_REG_LBA_3, LBA_io[3]);
			channel->write(ATA_REG_LBA_4, LBA_io[4]);
			channel->write(ATA_REG_LBA_5, LBA_io[5]);
		}

		channel->write(ATA_REG_SECTOR_COUNT_0, sector_count);
		channel->write(ATA_REG_LBA_LOW, LBA_io[0]);
		channel->write(ATA_REG_LBA_MID, LBA_io[1]);
		channel->write(ATA_REG_LBA_HIGH, LBA_io[2]);

		unsigned char cmd;
		

		if (!write)
		{
			if (LBA_mode == 0 || LBA_mode == 1)
			{
				cmd = ATA_CMD_READ_PIO;
			}
			else if (LBA_mode == 2)
			{
				cmd = ATA_CMD_READ_PIO_EXT;
			}
			else
			{
				assert(false);
				__builtin_unreachable();
			}
		}
		else
		{
			if (LBA_mode == 0 || LBA_mode == 1)
			{
				cmd = ATA_CMD_WRITE_PIO;
			}
			else if (LBA_mode == 2)
			{
				cmd = ATA_CMD_WRITE_PIO_EXT;
			}
			else
			{
				assert(false);
				__builtin_unreachable();
			}
		}

		channel->write(IDE_REG_COMMAND, cmd);
		COM_init(COM1_PORT);

		if (!write)
		{
			for (i = 0; i < sector_count; ++i)
			{
				if ((err = channel->poll(true)))
				{
					return err;
				}
				char debugChar = ('0' + err);
				COM_write(COM1_PORT, debugChar);

				for (int j = 0; j < 256; ++j)
				{
					uint16_t word = port_word_in(bus + IDE_REG_DATA);
					*(uint16_t*)(buf + j*2) = word;
				}
				buf += 512;
			}
		}
		else
		{
			for (i = 0; i < sector_count; ++i)
			{
				channel->poll(false);


				for (int j = 0; 256; ++j)
				{
					port_word_out(bus + IDE_REG_DATA, *(uint16_t*)(buf + j*2));
				}

				buf += 512;
			}

			constexpr static unsigned char flush_options[] = {ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT};

			channel->write(IDE_REG_COMMAND, flush_options[LBA_mode]);
			channel->poll(false);
		}
		return 0;
	}



	int ATADevice::read_sector(uint32_t LBA, uint8_t* buf)
	{
		return this->access(false, LBA, 1, buf);
	}
	
}
}



// namespace Drivers { namespace ATA {
	
// 	IDE::ChannelRegister_t Device::Channels[2];
// 	//Device Device::Devices[4];
// 	bool Device::_initted = false;
// 	static uchar buf[2048];
// 	uint32_t Device::BAR0;
// 	uint32_t Device::BAR1;
// 	uint32_t Device::BAR2;
// 	uint32_t Device::BAR3;
// 	uint32_t Device::BAR4;

// 	static IDE::ChannelRegister_t& get_channel(const unsigned char channel)
// 	{
// 		assert(channel == ATA_PRIMARY || channel == ATA_SECONDARY);
// 		return (channel == ATA_PRIMARY ? Device::Channels[0] : Device::Channels[1]);
// 	}

// 	unsigned char Device::Read(const unsigned char channel, const uint16_t reg)
// 	{
// 		unsigned char result = 0;
// 		IDE::ChannelRegister_t& chan = get_channel(channel);
		
// 		if (reg > 0x07 && reg < 0x0C)
// 		{
// 			Write(channel, ATA_REG_CONTROL, 0x80 | chan.nIEN);
// 		}
		
// 		if (reg < 0x08)
// 		{
// 			result = port_byte_in(chan.base + reg - 0x00);
// 		}
// 		else if (reg < 0x0C)
// 		{
// 			result = port_byte_in(chan.base + reg - 0x06);
// 		}
// 		else if (reg < 0x0E)
// 		{
// 			result = port_byte_in(chan.ctrl + reg - 0x0A);
// 		}
// 		else if (reg < 0x16)
// 		{
// 			result = port_byte_in(chan.bmide + reg - 0x0E);
// 		}
// 		else
// 		{
// 			ASSERT(false);
// 		}
		
// 		if (reg > 0x07 && reg < 0x0C)
// 		{
// 			Write(channel, ATA_REG_CONTROL, chan.nIEN);
// 		}
		
// 		return result;
// 	}
	
	
// 	void Device::Write(const unsigned char channel, const uint16_t reg, unsigned char data)
// 	{
// 		IDE::ChannelRegister_t& chan = get_channel(channel);
		
// 		if (reg > 0x07 && reg < 0x0C)
// 		{
// 			Write(channel, ATA_REG_CONTROL, 0x80 | chan.nIEN);
// 		}

// 		if (reg < 0x08)
// 		{
// 			port_byte_out(chan.base + reg - 0x00, data);
// 		}
// 		else if (reg < 0x0C)
// 		{
// 			port_byte_out(chan.base + reg - 0x06, data);
// 		}
// 		else if (reg < 0x0E)
// 		{
// 			port_byte_out(chan.ctrl + reg - 0x0A, data);
// 		}
// 		else if (reg < 0x16)
// 		{
// 			port_byte_out(chan.bmide + reg - 0x0E, data);
// 		}
		
// 		if (reg > 0x07 && reg < 0x0C)
// 		{
// 			Write(channel, ATA_REG_CONTROL, chan.nIEN);
// 		}
// 	}
	
	
// 	unsigned char Device::Poll(const unsigned char chan, const bool advCheck)
// 	{
// 		for (int i = 0; i < 4; ++i)
// 		{
// 			Read(chan, ATA_REG_ALT_STATUS);
// 		}
		
// 		while (Read(chan, ATA_REG_STATUS) & ATA_STATE_BUSY) ;
		
// 		if (advCheck)
// 		{
// 			unsigned char state = Read(chan, ATA_REG_STATUS);
			
// 			if (state & ATA_STATE_ERROR)
// 			{
// 				return 2;
// 			}
			
// 			if (state & ATA_STATE_FAULT)
// 			{
// 				return 1;
// 			}
			
// 			if ((state & ATA_STATE_REQUEST_READY) == 0)
// 			{
// 				return 3;
// 			}
// 		}
		
// 		return 0;
// 	}

// 	void Device::Select(const unsigned char channel, const unsigned char role)
// 	{
// 		Device::Write(channel, ATA_REG_DEVICE_SELECT, role);
// 	}
	
// 	void Device::ReadBuffer(const unsigned char channel, const uint16_t reg, uint32_t* buf, uint32_t dwordCount)
// 	{
// 		IDE::ChannelRegister_t& chan = get_channel(channel);
		
// 		if (reg > 0x07 && reg < 0x0C)
// 		{
// 			Write(channel, ATA_REG_CONTROL, 0x80 | chan.nIEN);
// 		}
// 		uint16_t esOld = 0;
// 		uint16_t ds = 0;
// 		#ifndef TESTING
// 		asm volatile ("movw %%es, %%bx" : "=b"(esOld));
// 		asm volatile ("movw %%ds, %%ax" : "=a"(ds));
// 		asm volatile ("movw %%bx, %%es" : : "b"(ds));
// 		#endif
// 		if (reg < 0x08)
// 		{
// 			// for (int i = 0; i < dwordCount*2; ++i)
// 			// {
// 			// 	((uint16_t*)buf)[i] = port_word_in(Channels[(uchar)channel].base + (uint16_t)reg - 0x00);
// 			// }
// 			insl(chan.base + reg - 0x00, buf, dwordCount);
// 		}
// 		else if (reg < 0x0C)
// 		{
// 			insl(chan.base + reg - 0x06, buf, dwordCount);
// 		}
// 		else if (reg < 0x0E)
// 		{
// 			insl(chan.base + reg - 0x0A, buf, dwordCount);
// 		}
// 		else if (reg < 0x16)
// 		{
// 			insl(chan.base + reg - 0xE, buf, dwordCount);
// 		}
// 		#ifndef TESTING
// 		asm volatile ("movw %%bx, %%es" : : "b"(esOld));
// 		#endif
// 		if (reg > 0x07 && reg < 0x0C)
// 		{
// 			Write(channel, ATA_REG_CONTROL, chan.nIEN);
// 		}
// 	}

// 	void Device::Initialize()
// 	{
// 		if (_initted)
// 		{
// 			return;
// 		}

// 		auto pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::ATA, PCIType::Null);

// 		if (pciDev == PCI::NULL_DEVICE)
// 		{
// 			assert(false);
// 			pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::PIIX4, PCIType::Null);
// 		}

// 		ASSERT(pciDev != PCI::NULL_DEVICE);

// 		auto bar0 = pciDev.read(PCIRegister::BAR0) + 0x1F0;
// 		auto bar1 = pciDev.read(PCIRegister::BAR1) + 0x1F0;
// 		auto bar2 = pciDev.read(PCIRegister::BAR2) + 0x1F0;
// 		auto bar3 = pciDev.read(PCIRegister::BAR3) + 0x1F0;
// 		auto bar4 = pciDev.read(PCIRegister::BAR4);

// 		// bar0 = 0x1F0;
// 		// bar1 = 0x3F6;
// 		// bar2 = 0x170;
// 		// bar3 = 0x376;

// 		ASSERT(bar0 == 0x1F0);

// 		Initialize(bar0, bar1, bar2, bar3, bar4);
// 	}
	
// 	void Device::Initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4)
// 	{
// 		if (_initted)
// 		{
// 			return;
// 		}
// 		_initted = true;
		
// 		Device::BAR0 = BAR0;
// 		Device::BAR1 = BAR1;
// 		Device::BAR2 = BAR2;
// 		Device::BAR3 = BAR3;
// 		Device::BAR4 = BAR4;

		

// 		if(BAR4 & 0x1)
// 		{
//         	Device::BAR4 = BAR4 & 0xfffffffc;
//     	}

// 		for (int i = 0; i < 512; ++i)
// 		{
// 			buf[i] = 0;
// 		}

// 		ASSERT(BAR0 == 0x1F0);
// 	}
	
	
// 	Device::Device(unsigned char channel, unsigned char role) : _device_initted(false), channel(channel), drive(role)
// 	{
// 		if (!Initialized)
// 		{
// 			Initialize();
// 		}
		
// 		assert(channel == ATA_PRIMARY || channel == ATA_SECONDARY);

// 		uint16_t base = (channel == ATA_PRIMARY) ? BAR0 : BAR2;
// 		uint16_t alt = (channel == ATA_PRIMARY) ? BAR1 : BAR3;
		
		
// 		data = base;
// 		error = base + 0x1;
// 		sectorCount = base + 0x2;
// 		LBALow = base + 0x3;
// 		LBAMid = base + 0x4;
// 		LBAHigh = base + 0x5;
// 		select = base + 0x6;
// 		command = base + 0x7;
// 		altStatus = alt;
// 		BMR = BAR4;
		
// 		init();
// 	}
	
// 	bool Device::init()
// 	{
// 		if (_device_initted)
// 		{
// 			return false;
// 		}
// 		_device_initted = true;
		
		
// 		unsigned char err = 0;
// 		type = Interface::ATA;
		
// 		softReset();
// 		delay();
				
// 		reserved = 0;
// 		port_byte_out(select, drive);
// 		port_byte_out(sectorCount, 0);
// 		port_byte_out(LBALow, 0);
// 		port_byte_out(LBAMid, 0);
// 		port_byte_out(LBAHigh, 0);
// 		port_byte_out(command, ATA_CMD_IDENTIFY);
// 		if (!port_byte_in(status))
// 		{
// 			err = 1;
// 			//return true;
// 		}
		
// 		uchar lLow = port_byte_in(LBALow);
// 		uchar lHigh = port_byte_in(LBAHigh);
// 		if (lLow != 0 || lHigh != 0)
// 		{
// 			// Not ATA
			
// 			err = 1;
// 			assert(false);
// 		}
		
// 		while (!err)
// 		{
// 			uchar stat = port_byte_in(status);
			
// 			if (stat & ATA_STATE_ERROR)
// 			{
// 				err = 1;
// 				break;
// 			}
// 			else if (stat & ATA_STATE_REQUEST_READY)
// 			{
// 				break;
// 			}
// 		}
		
// 		if (err != 0)
// 		{
// 			TRACE("ATA Error Encountered...\n");
// 			unsigned char cl = port_byte_in(LBAMid);
// 			unsigned char ch = port_byte_in(LBAHigh);
			
// 			if (cl == 0x14 && ch == 0xEB)
// 			{
// 				//TODO
// 				assert(NOT_IMPLEMENTED);
// 				type = Interface::ATAPI;
// 			}
// 			else if (cl == 0x69 && ch == 0x96)
// 			{
// 				//TODO
// 				assert(NOT_IMPLEMENTED);
// 				type = Interface::ATAPI;
// 			}
// 			else
// 			{
// 				assert(NOT_IMPLEMENTED);
// 				return true;
// 			}
			
// 			TRACE("Nvm, was just an ATAPI device.\n");
			
// 			port_byte_out(command, ATA_CMD_IDENTIFY_PACKET);
// 			delay();
			
// 			return true;
// 		}

// 		insl(data, (uint32_t*)buf, 127);
// 		reserved = 1;

// 		for (int k = 0; k < 40; k += 2)
// 		{
// 			model[k] = buf[ATA_IDENTIFY_MODEL + k + 1];
// 			model[k+1] = buf[ATA_IDENTIFY_MODEL + k];
// 		}

// 		auto pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::ATA, PCIType::Null);

// 		if (pciDev == PCI::NULL_DEVICE)
// 		{
// 			pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::PIIX4, PCIType::Null);
// 		}

// 		ASSERT(pciDev != PCI::NULL_DEVICE);

// 		auto pciCmd = pciDev.read(PCIRegister::Command);
// 		if (!(pciCmd & (1 << 2)))
// 		{
// 			pciCmd |= (1 << 2);
// 			pciDev.write(PCIRegister::Command, pciCmd);
// 		}
		
// 		// insl(data, (uint32_t*)buf, 256);
// 		// reserved = 1;
				
// 		/*
// 				Devices[count].type = type;
// 				Devices[count].channel = (Channel)i;
// 				Devices[count].drive = j;
// 				Devices[count].signature = *((uint16_t*)(buf + (uint16_t)ATAIdentify::DeviceType));
// 				Devices[count].capabilities = *((uint16_t*)(buf + (uint16_t)ATAIdentify::Capabilities));
// 				Devices[count].commandSets = *((uint32_t*)(buf + (uint16_t)ATAIdentify::CommandSets));

				
				
// 				if (Devices[count].commandSets & (1 << 26))
// 				{
// 					Devices[count].size = *((unsigned int*)buf + (uint16_t)ATAIdentify::MaxLBAExt);
// 				}
// 				else
// 				{
// 					Devices[count].size = *((unsigned int*)buf + (uint16_t)ATAIdentify::MaxLBA);
// 				}

// 				for (k = 0; k < 40; k += 2)
// 				{
// 					Devices[count].model[k] = buf[ATAIdentify::Model + k + 1];
// 					Devices[count].model[k+1] = buf[ATAIdentify::Model + k];
// 				}

// 				Devices[count].model[40] = '\0';

// 				++count;
		
// 		*/
		
// 		return true;
// 	}
	
// 	void Device::delay() const
// 	{
// 		for (int i = 0; i < 4; ++i)
// 		{
// 			port_byte_in(altStatus);
// 		}
// 	}
	
// 	void Device::softReset()
// 	{
// 		port_byte_out(control, 4);
// 		delay();
// 		port_byte_out(control, 0);
// 	}
// }
// }