#include "IDE.hh"
#include <drivers/PortIO.h>
#include <kernel/Debug.h>


// https://wiki.osdev.org/IDE

namespace Drivers { namespace IDE {
	
	ChannelRegister_t Device::Channels[2];
	Device Device::Devices[4];
	bool Device::_initted = false;
	static uint32_t buf[512];
	
	unsigned char Device::read(const Channel channel, const Register reg)
	{
		unsigned char result = 0;
		
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, 0x80 | Channels[(unsigned char)channel].nIEN);
		}
		
		if (reg < 0x08)
		{
			result = port_byte_in(Channels[(unsigned char)channel].base + (unsigned char)reg - 0x00);
		}
		else if (reg < 0x0C)
		{
			result = port_byte_in(Channels[(unsigned char)channel].base + (unsigned char)reg - 0x06);
		}
		else if (reg < 0x0E)
		{
			result = port_byte_in(Channels[(unsigned char)channel].ctrl + (unsigned char)reg - 0x0A);
		}
		else if (reg < 0x16)
		{
			result = port_byte_in(Channels[(unsigned char)channel].bmide + (unsigned char)reg - 0x0E);
		}
		else
		{
			ASSERT(false);
		}
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, Channels[(unsigned char)channel].nIEN);
		}
		
		return result;
	}
	
	
	void Device::write(const Channel channel, const Register reg, unsigned char data)
	{
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, 0x80 | Channels[(unsigned char)channel].nIEN);
		}
		
		if (reg < 0x08)
		{
			port_byte_out(Channels[(unsigned char)channel].base + (unsigned char)reg - 0x00, data);
		}
		else if (reg < 0x0C)
		{
			port_byte_out(Channels[(unsigned char)channel].base + (unsigned char)reg - 0x06, data);
		}
		else if (reg < 0x0E)
		{
			port_byte_out(Channels[(unsigned char)channel].ctrl + (unsigned char)reg - 0x0A, data);
		}
		else if (reg < 0x16)
		{
			port_byte_out(Channels[(unsigned char)channel].bmide + (unsigned char)reg - 0x0E, data);
		}
		
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, Channels[(unsigned char)channel].nIEN);
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
	
	//BROKEN
	void Device::readBuffer(const Channel channel, const Register reg, uint32_t* buf, uint32_t dwordCount)
	{
		
		//TODO
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, 0x80 | Channels[(uchar)channel].nIEN);
		}
		asm volatile ("pushw %es; movw %ds, %ax; movw %ax, %es");
		if (reg < 0x08)
		{
			insl(Channels[(uchar)channel].base + reg - 0x00, buf, dwordCount);
			ASSERT(false);
		}
		else if (reg < 0x0C)
		{
			insl(Channels[(uchar)channel].base + reg - 0x06, buf, dwordCount);
		}
		else if (reg < 0x0E)
		{
			insl(Channels[(uchar)channel].base + reg - 0x0A, buf, dwordCount);
		}
		else if (reg < 0x16)
		{
			insl(Channels[(uchar)channel].base + reg - 0xE, buf, dwordCount);
		}
		asm volatile ("popw %es");
		if (reg > 0x07 && reg < 0x0C)
		{
			write(channel, Register::Control, Channels[(uchar)channel].nIEN);
		}
	}
	
	void Device::Initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4)
	{
		if (_initted)
		{
			return;
		}
		_initted = true;
		auto initialStatus1 = read(Channel::Primary, Register::Status);
		auto initialStatus2 = read(Channel::Secondary, Register::Status);
		int j, k, count = 0;
		Channels[(uchar)Channel::Primary].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
		Channels[(uchar)Channel::Primary].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
		Channels[(uchar)Channel::Secondary].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
		
		Channels[(uchar)Channel::Secondary].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
		
		Channels[(uchar)Channel::Primary].bmide = (BAR4 & 0xFFFFFFFC);
		
		Channels[(uchar)Channel::Secondary].bmide = (BAR4 & 0xFFFFFFFC) + 0x08;
		
		
		
		
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
		for (; i < iEnd; i = (Channel)(i+1))
		{
			for (j = 0; j < 2; ++j)
			{
				currentRole = (j == 0 ? Role::Master : Role::Slave);
				unsigned char err = 0,
				type = (uchar)Interface::ATA;
				
				Devices[count].reserved = 0;
				


				write(i, Register::HDDevSel, (uchar)currentRole);
				write(i, Register::SecCount0, 0);
				write(i, Register::LBA0, 0);
				write(i, Register::LBA1, 0);
				write(i, Register::LBA2, 0);
				sleep(1);

				write(i, Register::Command, (uchar)ATACmd::Identify);
				
				//TODO: Sleep for ~1ms
				sleep(1);
				
				if (read(i, Register::Status) == 0)
				{
					continue;
				}
				
				while (true)
				{
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
					unsigned char cl = read(i, Register::LBA1);
					unsigned char ch = read(i, Register::LBA2);
					
					if (cl == 0x14 && ch == 0xEB)
					{
						//TODO
						type = (uchar)Interface::ATAPI;
					}
					else if (cl == 0x69 && ch == 0x96)
					{
						//TODO
						type = (uchar)Interface::ATAPI;
					}
					else
					{
						continue;
					}
					
					write(i, Register::Command, (uchar)ATACmd::IdentifyPacket);
					sleep(1);
				}
				
				readBuffer(i, Register::Data, buf, 127);
				
				Devices[count].reserved = 1;
				Devices[count].type = type;
				Devices[count].channel = (Channel)i;
				Devices[count].drive = j;
				Devices[count].signature = *((unsigned short*)(buf + (uchar)ATAIdentify::DeviceType));
				Devices[count].capabilities = *((unsigned short*)(buf + (uchar)ATAIdentify::Capabilities));
				Devices[count].commandSets = *((unsigned int*)(buf + (uchar)ATAIdentify::CommandSets));


				if (Devices[count].commandSets & (1 << 26))
				{
					Devices[count].size = *((unsigned int*)buf + (uchar)ATAIdentify::MaxLBAExt);
				}
				else
				{
					Devices[count].size = *((unsigned int*)buf + (uchar)ATAIdentify::MaxLBA);
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