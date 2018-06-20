// #include "DMA.hh"
// #include <drivers/PortIO.h>
// #include <kernel/Interrupts.h>
// #include <drivers/VGA.hh>

// namespace Drivers { namespace ATA {

// 	bool DMADrive::_initted = false;
// 	static bool int_fired = false;
// 	static DMADrive* current_drive = nullptr;

// 	void HandleInterrupt(Registers_t regs)
// 	{
// 		Drivers::VGA::Write("_Interrupt Received!\n");
// 	}

// 	void DMADrive::Initialize()
// 	{
// 		if (_initted)
// 		{
// 			return;
// 		}
// 		_initted = true;

// 		Kernel::Interrupts::register_interrupt_handler(IRQ14, DMADrive::HandleInterrupt);
// 		Kernel::Interrupts::register_interrupt_handler(IRQ15, DMADrive::HandleInterrupt);
// 		Device::Channels[0].nIEN = 0;
// 		Device::Channels[1].nIEN = 0;
// 	}

	

// 	void DMADrive::HandleInterrupt(Registers_t regs)
// 	{
// 		Drivers::VGA::Write("Interrupt Received!\n");
// 		if (current_drive != nullptr)
// 		{
// 			port_byte_in(current_drive->dev->status);
// 			port_byte_in(current_drive->BMR + (uint16_t)Register::Status);
// 			port_byte_out(current_drive->BMR + (uint16_t)Register::Command, 0);
// 		}
// 		int_fired = true;
// 	}
	
// 	DMADrive::DMADrive(const unsigned char chan, const unsigned char role) : DMADrive(new Device(chan, role)/*&Device::Devices[2*(int)(chan == Channel::Secondary) + (int)(role == Role::Slave)]*/)
// 	{}
	
// 	DMADrive::DMADrive(Device* dev) : Disk(), BMR(0), _dir(), _state(false), dev(dev), prdt(nullptr), prdt_phys(0x0)
// 	{
// 		Initialize();
// 		prdt = PRDT<10>::Create(&prdt_phys);
// 		current_drive = this;
// 		port_byte_out(dev->command, ATA_CMD_IDENTIFY);
// 		dev->delay();
// 		for (int i = 0; i < 256; ++i) port_word_in(dev->data);
// 		for (auto i = 0; i < PRDT_Size; ++i)
// 		{
// 			bufsPhys[i] = 0;
// 			bufs[i] = nullptr;
// 			bufsSize[i] = 0;
// 		}
// 		BMR = dev->BMR;
// 		dev->softReset();
// 		dev->delay();
// 		port_byte_out(dev->control, 0);

// 	}
	
	
	
// 	void DMADrive::configureForSize(size_t len) const
// 	{
// 		auto fullEntries = len / (64*1024);
// 		for (auto i = 0; i < fullEntries; ++i)
// 		{
// 			if (bufsSize[i] < (64*1024))
// 			{
// 				bufsSize[i] = 64*1024;
// 				if (bufs[i] != nullptr)
// 				{
// 					free(bufs[i]);
// 				}
// 				bufs[i] = (unsigned char*)kmalloc(64*1024, 1, &bufsPhys[i]);
// 			}
			
// 			prdt->entries[i].end = 0;
// 			prdt->entries[i].phys_buf = bufsPhys[i];
// 			prdt->entries[i].count = 0;
// 		}
		
// 		auto last = (len % (64*1024) > 0) ? fullEntries : fullEntries - 1;
// 		prdt->entries[last].end = 1;
// 		if (len % (64*1024) > 0)
// 		{
// 			prdt->entries[last].count = (len % 64*1024);
// 			if (bufsSize[last] < (len % 64*1024))
// 			{
// 				if (bufs[last] != nullptr)
// 				{
// 					free(bufs[last]);
// 				}
// 				bufs[last] = (unsigned char*)kmalloc(len % 64*1024, 1, &bufsPhys[last]);
// 				bufsSize[last] = (len % 64*1024);
// 			}
// 			prdt->entries[last].phys_buf = bufsPhys[last];
// 		}
// 	}
	
	
// 	unsigned char* DMADrive::readSector(uint32_t lba, uint32_t len)
// 	{
// 		assert(len <= 512);
// 		auto data = (unsigned char*)kmalloc(len, 0, 0);
		
// 		// DEBUG
// 		//kmemset(data, 0, 512);
// 		kmemset(data, 1, 512);
// 		assert(data[511] == 1);
		
// 		if (read(lba, len, data) == 0)
// 		{
// 			return data;
// 		}
// 		else
// 		{
// 			free(data);
// 			return nullptr;
// 		}
// 	}
	
// 	int DMADrive::read(size_t lba, size_t len, unsigned char* data) const
// 	{
// 		/*auto fullEntries = len / (64*1024);
// 		for (auto i = 0; i < fullEntries; ++i)
// 		{
// 			if (bufsSize[i] < (64*1024))
// 			{
// 				bufsSize[i] = 64*1024;
// 				if (bufs[i] != nullptr)
// 				{
// 					free(bufs[i]);
// 				}
// 				bufs[i] = (unsigned char*)kmalloc(64*1024, 1, &bufsPhys[i]);
// 			}
			
// 			prdt->entries[i].end = 0;
// 			prdt->entries[i].phys_buf = bufsPhys[i];
// 			prdt->entries[i].count = 0;
// 		}
		
// 		auto last = (len % (64*1024) > 0) ? fullEntries : fullEntries - 1;
// 		prdt->entries[last].end = 1;
// 		if (len % (64*1024) > 0)
// 		{
// 			prdt->entries[last].count = (len % 64*1024);
// 			if (bufsSize[last] < (len % 64*1024))
// 			{
// 				if (bufs[last] != nullptr)
// 				{
// 					free(bufs[last]);
// 				}
// 				bufs[last] = (unsigned char*)kmalloc(len % 64*1024, 1, &bufsPhys[last]);
// 				bufsSize[last] = (len % 64*1024);
// 			}
// 			prdt->entries[last].phys_buf = bufsPhys[last];
// 		}
// 		*/
		
// 		if (data == nullptr)
// 		{
// 			return -1;
// 		}
		
// 		configureForSize(len);
		
// 		assert(BMR != 0);
// 		port_byte_out(dev->control, 0);
// 		port_byte_out(BMR + (uint16_t)Register::Command, 0);
// 		port_long_out(BMR + (uint16_t)Register::PRDT_Addr, prdt_phys);
// 		//port_byte_out(BMR + (uint16_t)Register::Command, 0x8);
// 		port_byte_out(BMR + (uint16_t)Register::Status, Status{0,0,0,0,0,0});
// 		port_byte_out(dev->select, (uchar)dev->drive);
// 		port_byte_out(dev->sectorCount, len / 512 + (int)((len % 512) > 0));
// 		assert((len / 512 + (int)((len % 512) > 0)) > 0);
// 		port_byte_out(dev->lbaLow, lba & 0xFF);
// 		// port_byte_out(dev->lbaMid, (lba >> 8) & 0xFF);
// 		port_byte_out(dev->lbaMid, (lba & 0x0000FF00) >> 8);
// 		// port_byte_out(dev->lbaHigh, (lba >> 16) & 0xFF);
// 		port_byte_out(dev->lbaHigh, (lba & 0x00FF0000) >> 16);

// 		current_drive = const_cast<DMADrive*>(this);
// 		int_fired = false;
// 		assert(int_fired == false);
// 		TRACE("int_fired = false\n");

// 		port_byte_out(dev->command, ATA_CMD_READ_DMA);
// 		port_byte_out(dev->BMR + (uint16_t)Register::Command, 0x8 | 0x1);
		
// 		// unsigned long count = 0;
// 		// while (++count < 10000000)
// 		// {
			
// 		// 	auto stat = port_byte_in(dev->BMR + (uint16_t)Register::Status);
// 		// 	auto dstat = port_byte_in(dev->altStatus);
			
// 		// 	if ((stat & 0x04) == 0)
// 		// 	{
// 		// 		continue;
// 		// 	}
			
// 		// 	if ((dstat & ATAState::Busy) == 0)
// 		// 	{
// 		// 		break;
// 		// 	}
// 		// 	//assert(Status::Get(stat).done);
// 		// }
// 		// assert(count < 10000000);
// 		while (!int_fired) ;

// 		assert(!(port_byte_in(BMR + 0x2) & 0x2));
		
// 		assert(!Status::Get(port_byte_in(BMR + 0x2)).done);
// 		VGA::Write("Command: ");
// 		VGA::Write((void*)port_byte_in(BMR));
// 		VGA::Write("\n");

// 		assert(!Status::Get(port_byte_in(BMR + (uint16_t)Register::Status)).failed);
		
// 		auto last = (len % (64*1024) > 0) ? (len / (64*1024)) : (len / (64*1024)) - 1;
		
// 		unsigned char* ptr = data;
// 		for (int i = 0; i <= last; ++i)
// 		{
// 			memcpy(ptr, bufs[i], bufsSize[i]);
// 			ptr += bufsSize[i];
// 		}
// 		TRACE("Returning...\n");
// 		return 0;
// 	}
	
// 	int DMADrive::write(size_t lba, size_t len, const unsigned char* data)
// 	{
// 		configureForSize(len);
// 		auto last = (len % (64*1024) > 0) ? (len / (64*1024)) : (len / (64*1024)) - 1;
		
// 		const unsigned char* ptr = data;
// 		for (int i = 0; i <= last; ++i)
// 		{
// 			memcpy(bufs[i], ptr, bufsSize[i]);
// 			ptr += bufsSize[i];
// 		}
		
		
// 		// TODO
// 		assert(false);
		
// 	}
	
// 	size_t DMADrive::capacity() const
// 	{
// 		return 512*dev->size;
// 	}
// }
// }