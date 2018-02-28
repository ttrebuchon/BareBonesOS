#include "IDE.hh"
#include <drivers/PortIO.h>
#include <drivers/PCI.hh>
#include <kernel/Debug.h>


// https://wiki.osdev.org/IDE

namespace Drivers { namespace IDE {
	
	ChannelRegister_t Device::Channels[2];
	Device Device::Devices[4];
	bool Device::_initted = false;
	static uchar buf[2048];
	
	unsigned char Device::read(const Channel channel, const Register reg)
	{
		unsigned char result = 0;
		ChannelRegister_t& chan = (channel == Channel::Primary ? Channels[0] : Channels[1]);
		
		ASSERT(Channels[(unsigned char)channel].base + (unsigned char)reg == Channels[(unsigned char)channel].base + (uint16_t)reg);
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, 0x80 | chan.nIEN);
		}
		
		if (reg < 0x08)
		{
			result = port_byte_in(chan.base + reg - 0x00);
		}
		else if (reg < 0x0C)
		{
			result = port_byte_in(chan.base + reg - 0x06);
		}
		else if (reg < 0x0E)
		{
			result = port_byte_in(chan.ctrl + reg - 0x0A);
		}
		else if (reg < 0x16)
		{
			result = port_byte_in(chan.bmide + reg - 0x0E);
		}
		else
		{
			ASSERT(false);
		}
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, chan.nIEN);
		}
		
		return result;
	}
	
	
	void Device::write(const Channel channel, const Register reg, unsigned char data)
	{
		ChannelRegister_t& chan = (channel == Channel::Primary ? Channels[0] : Channels[1]);
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, 0x80 | chan.nIEN);
		}
		
		ASSERT(Channels[(unsigned char)channel].base + (uint16_t)reg - 0x00 == Channels[(unsigned char)channel].base + (unsigned short)reg - 0x00);

		if (reg < 0x08)
		{
			port_byte_out(chan.base + reg - 0x00, data);
		}
		else if (reg < 0x0C)
		{
			port_byte_out(chan.base + reg - 0x06, data);
		}
		else if (reg < 0x0E)
		{
			port_byte_out(chan.ctrl + reg - 0x0A, data);
		}
		else if (reg < 0x16)
		{
			port_byte_out(chan.bmide + reg - 0x0E, data);
		}
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, chan.nIEN);
		}
	}
	
	
	unsigned char Device::poll(const Channel chan, const bool advCheck)
	{
		for (int i = 0; i < 4; ++i)
		{
			read(chan, Register::AltStatus);
		}
		
		while (read(chan, Register::Status) & ATAState::Busy) ;
		
		if (advCheck)
		{
			unsigned char state = read(chan, Register::Status);
			
			if (state & ATAState::Error)
			{
				return 2;
			}
			
			if (state & ATAState::Fault)
			{
				return 1;
			}
			
			if ((state & ATAState::RequestReady) == 0)
			{
				return 3;
			}
		}
		
		return 0;
	}
	
	void Device::readBuffer(const Channel channel, const Register reg, uint32_t* buf, uint32_t dwordCount)
	{
		ChannelRegister_t& chan = (channel == Channel::Primary ? Channels[0] : Channels[1]);
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, 0x80 | chan.nIEN);
		}
		uint16_t esOld = 0;
		uint16_t ds = 0;
		#ifndef TESTING
		asm volatile ("movw %%es, %%bx" : "=b"(esOld));
		asm volatile ("movw %%ds, %%ax" : "=a"(ds));
		asm volatile ("movw %%bx, %%es" : : "b"(ds));
		#endif
		if (reg < 0x08)
		{
			// for (int i = 0; i < dwordCount*2; ++i)
			// {
			// 	((uint16_t*)buf)[i] = port_word_in(Channels[(uchar)channel].base + (uint16_t)reg - 0x00);
			// }
			insl(chan.base + reg - 0x00, buf, dwordCount);
		}
		else if (reg < 0x0C)
		{
			insl(chan.base + reg - 0x06, buf, dwordCount);
		}
		else if (reg < 0x0E)
		{
			insl(chan.base + reg - 0x0A, buf, dwordCount);
		}
		else if (reg < 0x16)
		{
			insl(chan.base + reg - 0xE, buf, dwordCount);
		}
		#ifndef TESTING
		asm volatile ("movw %%bx, %%es" : : "b"(esOld));
		#endif
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, chan.nIEN);
		}
	}

	void Device::Initialize()
	{
		if (_initted)
		{
			return;
		}

		auto pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::ATA, PCIType::Null);

		if (pciDev == PCI::NULL_DEVICE)
		{
			pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::PIIX4, PCIType::Null);
		}

		ASSERT(pciDev != PCI::NULL_DEVICE);

		auto bar0 = pciDev.read(PCIRegister::BAR0) + 0x1F0;
		auto bar1 = pciDev.read(PCIRegister::BAR1) + 0x1F0;
		auto bar2 = pciDev.read(PCIRegister::BAR2) + 0x1F0;
		auto bar3 = pciDev.read(PCIRegister::BAR3) + 0x1F0;
		auto bar4 = pciDev.read(PCIRegister::BAR4);

		// bar0 = 0x1F0;
		// bar1 = 0x3F6;
		// bar2 = 0x170;
		// bar3 = 0x376;

		ASSERT(bar0 == 0x1F0);

		Initialize(bar0, bar1, bar2, bar3, bar4);
	}
	
	void Device::Initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4)
	{
		if (_initted)
		{
			return;
		}
		_initted = true;

		for (int i = 0; i < 512; ++i)
		{
			buf[i] = 0;
		}

		ASSERT(BAR0 == 0x1F0);
		int j, k, count = 0;
		Channels[(uchar)Channel::Primary].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
		ASSERT(Channels[(uchar)Channel::Primary].base == 0x1F0);
		Channels[(uchar)Channel::Primary].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
		Channels[(uchar)Channel::Secondary].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
		
		Channels[(uchar)Channel::Secondary].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
		
		Channels[(uchar)Channel::Primary].bmide = (BAR4 & 0xFFFFFFFC);
		
		Channels[(uchar)Channel::Secondary].bmide = (BAR4 & 0xFFFFFFFC) + 0x08;
		
		
		auto initialStatus1 = read(Channel::Primary, Register::Status);
		auto initialStatus2 = read(Channel::Secondary, Register::Status);
		
		write(Channel::Primary, Register::Control, 2);
		write(Channel::Secondary, Register::Control, 2);
		Channel i = (Channel)0;
		Channel iEnd = (Channel)2;
		if (initialStatus1 == 0xFF)
		{
			Devices[0].reserved = 0;
			Devices[1].reserved = 0;
			i = (Channel)1;
		}
		if (initialStatus2 == 0xFF)
		{
			Devices[2].reserved = 0;
			Devices[3].reserved = 0;
			iEnd = (Channel)1;
		}
		if (initialStatus1 == 0xFF && initialStatus2 == 0xFF)
		{
			return;
		}
		Role currentRole = Role::Master;
		for (j = 0; j < 2; ++j)
		{
			for (; i < iEnd; i = (Channel)(i+1))
			{
				count = ((int)i)*2 + j;
				currentRole = (j == 0 ? Role::Master : Role::Slave);
				unsigned char err = 0;
				Interface type = Interface::ATA;
				
				Devices[count].reserved = 0;
				


				write(i, Register::HDDevSel, (uchar)currentRole);
				write(i, Register::SecCount0, 0);
				write(i, Register::LBA0, 0);
				write(i, Register::LBA1, 0);
				write(i, Register::LBA2, 0);
				sleep(1);

				//poll(i, false);

				write(i, Register::Command, (uchar)ATACmd::Identify);
				
				//TODO: Sleep for ~1ms
				sleep(1);
				
				if (read(i, Register::Status) == 0)
				{
					TRACE("ATA Bus is not present!\n");

					// Drivers::VGA::Write("ATA Bus ");
					// Drivers::VGA::Write((int)i);
					// Drivers::VGA::Write(" is not present!\n");
					continue;
				}

				while (true)
				{
					if (read(i, Register::LBA1) != 0)
					{
						err = 1;
						break;
					}
					if (read(i, Register::LBA2) != 0)
					{
						err = 1;
						break;
					}

					unsigned char status = read(i, Register::Status);
					if ((status & ATAState::Error))
					{
						err = 1;
						break;
					}
					
					if (!((uchar)status & ATAState::Busy) && ((uchar)status & ATAState::RequestReady))
					{
						break;
					}
				}
				
				if (err != 0)
				{
					TRACE("ATA Error Encountered...\n");
					unsigned char cl = read(i, Register::LBA1);
					unsigned char ch = read(i, Register::LBA2);
					
					if (cl == 0x14 && ch == 0xEB)
					{
						//TODO
						type = Interface::ATAPI;
					}
					else if (cl == 0x69 && ch == 0x96)
					{
						//TODO
						type = Interface::ATAPI;
					}
					else
					{
						continue;
					}

					TRACE("Nvm, was just an ATAPI device.\n");
					
					write(i, Register::Command, (uchar)ATACmd::IdentifyPacket);
					sleep(1);
				}

				readBuffer(i, Register::Data, (uint32_t*)buf, 128);
				
				Devices[count].reserved = 1;
				Devices[count].type = type;
				Devices[count].channel = (Channel)i;
				Devices[count].drive = j;
				Devices[count].signature = *((uint16_t*)(buf + (uint16_t)ATAIdentify::DeviceType));
				Devices[count].capabilities = *((uint16_t*)(buf + (uint16_t)ATAIdentify::Capabilities));
				Devices[count].commandSets = *((uint32_t*)(buf + (uint16_t)ATAIdentify::CommandSets));

				
				/*Drivers::VGA::Write("Size could be either ");
				Drivers::VGA::Write(*((uint32_t*)buf + (uint16_t)ATAIdentify::MaxLBAExt));
				Drivers::VGA::Write(" or ");
				Drivers::VGA::Write(*((uint32_t*)buf + (uint16_t)ATAIdentify::MaxLBA));
				Drivers::VGA::Write("\n");
				*/
				
				if (Devices[count].commandSets & (1 << 26))
				{
					Devices[count].size = *((unsigned int*)buf + (uint16_t)ATAIdentify::MaxLBAExt);
				}
				else
				{
					Devices[count].size = *((unsigned int*)buf + (uint16_t)ATAIdentify::MaxLBA);
				}

				for (k = 0; k < 40; k += 2)
				{
					Devices[count].model[k] = buf[ATAIdentify::Model + k + 1];
					Devices[count].model[k+1] = buf[ATAIdentify::Model + k];
				}

				Devices[count].model[40] = '\0';

				++count;
			}
		}
	}
}
}