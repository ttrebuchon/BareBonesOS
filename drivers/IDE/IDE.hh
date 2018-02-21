#ifndef INCLUDED_ATA_HH
#define INCLUDED_ATA_HH

#include <Common.h>
#include <kernel/utils/EnumOperators.hh>

namespace Drivers { namespace IDE {
	
	
	enum class ATAState : unsigned char
	{
		Busy = 0x80,
		Ready = 0x40,
		Fault = 0x20,
		SeekDone = 0x10,
		RequestReady = 0x08,
		CorrectedData = 0x04,
		Inlex = 0x02,
		Error = 0x01
	};
	
	
	
	enum class ATAError : unsigned char
	{
		BadSector = 0x80,
		UncorectableData = 0x40,
		NoMedia = 0x20,
		IDNotFound = 0x10,
		NoMedia_2 = 0x08,
		CmdAborted = 0x04,
		Track0NotFound = 0x02,
		NoAddr = 0x01
	};
	
	enum class ATACmd : unsigned char
	{
		ReadPIO = 0x20,
		ReadPIOExt = 0x24,
		ReadDMA = 0xC8,
		ReadDMAExt = 0x25,
		WritePIO = 0x30,
		WritePIOExt = 0x34,
		WriteDMA = 0xCA,
		WriteDMAExt = 0x35,
		CacheFlush = 0xE7,
		CacheFlushExt = 0xEA,
		Packet = 0xA0,
		IdentifyPacket = 0xA1,
		Identify = 0xEC
	};
	
	enum class ATAIdentify : unsigned char
	{
		DeviceType = 0x00,
		Cylinders = 0x02,
		Heads = 0x06,
		Sectors = 0x0C,
		Serial = 0x14,
		Model = 0x36,
		Capabilities = 0x62,
		FieldValid = 0x6A,
		MaxLBA = 0x78,
		CommandSets = 0xA4,
		MaxLBAExt = 0xC8
	};

	enum class Interface : unsigned char
	{
		ATA = 0x0,
		ATAPI = 0x1,
	};
	
	enum class Channel : unsigned char
	{
		Primary = 0x00,
		Secondary = 0x01
	};

	enum class Role : unsigned char
	{
		Master = 0xA0,
		Slave = 0xB0,
	};
	
	enum class Register : unsigned char
	{
		Data = 0x00,
		Error = 0x01,
		Features = 0x01,
		SecCount0 = 0x02,
		LBA0 = 0x03,
		LBA1 = 0x04,
		LBA2 = 0x05,
		HDDevSel = 0x06,
		Command = 0x07,
		Status = 0x07,
		SecCount1 = 0x08,
		LBA3 = 0x09,
		LBA4 = 0x0A,
		LBA5 = 0x0B,
		Control = 0x0C,
		AltStatus = 0x0C,
		DevAddr = 0x0D
	};
	
	ENUM_OPS(ATAState, unsigned char)
	ENUM_OPS(ATAError, unsigned char)
	ENUM_OPS(ATACmd, unsigned char)
	ENUM_OPS(Channel, unsigned char)
	ENUM_OPS(Register, unsigned char)
	ENUM_OPS(ATAIdentify, unsigned char)
	
	struct ChannelRegister_t
	{
		// I/O Base
		unsigned short base;
		
		// Control Base
		unsigned short ctrl;
		
		// Bus Master IDE
		unsigned short bmide;
		
		// No Interrupt
		unsigned char nIEN;
		
	} __attribute__((packed));
	
	static_assert(sizeof(ChannelRegister_t) == 7);
	
	class Device
	{
		private:
		static bool _initted;

		public:
		static ChannelRegister_t Channels[2];
		static Device Devices[4];
		
		// 0 or 1 (Empty or Exists)
		unsigned char reserved;
		
		// 0 or 1 (Primary channel or secondary)
		Channel channel;
		
		// 0 or 1 (Master or Slave)
		unsigned char drive;
		
		// 0 or 1 (ATA or ATAPI)
		unsigned short type;
		
		// Drive Signature
		unsigned short signature;
		
		// Features
		unsigned short capabilities;
		
		// Cmd Sets Supported
		unsigned int commandSets;
		
		// Size (In # of sectors)
		unsigned int size;
		
		// Model (As string)
		unsigned char model[41];
		
		
		
		
		static unsigned char read(const Channel channel, const Register reg);
		
		static void write(const Channel, const Register, unsigned char data);
		
		static void readBuffer(const Channel, const Register, uint32_t* buf, uint32_t dwordCount);
		
		static unsigned char poll(const Channel, const bool advCheck = false);
		
		static void Initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);
		
		static inline bool Initialized()
		{
			return _initted;
		}
		
	} __attribute__((packed));
	
	static_assert(sizeof(Device) == 58);
}
}

#endif