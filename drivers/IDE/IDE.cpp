#include "IDE.hh"
#include <drivers/PortIO.h>


// https://wiki.osdev.org/IDE

namespace Drivers { namespace IDE {
	
	ChannelRegister_t Device::Channels[2];
	Device Device::Devices[4];
	static uint32_t buf[512];
	
	unsigned char Device::read(const Channel channel, const Register reg)
	{
		unsigned char result;
		
		
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
	
	void Device::readBuffer(const Channel, const Register, uint32_t* buf, uint32_t quads)
	{
		//TODO
	}
	
	void Device::Initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4)
	{
		int j, k, count = 0;
		Channels[(uchar)Channel::Primary].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
		Channels[(uchar)Channel::Primary].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
		Channels[(uchar)Channel::Secondary].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
		
		Channels[(uchar)Channel::Secondary].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
		
		Channels[(uchar)Channel::Primary].bmide = (BAR4 & 0xFFFFFFFC);
		
		Channels[(uchar)Channel::Secondary].bmide = (BAR4 & 0xFFFFFFFC) + 0x08;
		
		
		
		
		write(Channel::Primary, Register::Control, 2);
		write(Channel::Secondary, Register::Control, 2);
		
		for (Channel i = (Channel)0; i < 2; i = (Channel)(i+1))
		{
			for (j = 0; j < 2; ++j)
			{
				unsigned char err = 0,
				type = 0x0,
				status;
				
				Devices[count].reserved = 0;
				write(i, Register::Command, (uchar)ATACmd::Identify);
				
				//TODO: Sleep for ~1ms
				
				if (read(i, Register::Status) == 0)
				{
					continue;
				}
				
				while (true)
				{
					status = read(i, Register::Status);
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
						//type = ATAPI;
					}
					else if (cl == 0x69 && ch == 0x96)
					{
						//TODO
						//type = ATAPI;
					}
					else
					{
						continue;
					}
					
					write(i, Register::Command, (uchar)ATACmd::IdentifyPacket);
					
					
					//TODO
					//sleep(1)
				}
				
				readBuffer(i, Register::Data, buf, 128);
				
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