#ifndef INCLUDED_ATA_HH
#define INCLUDED_ATA_HH

#include <Common.h>
#include <Utils/EnumOperators.hh>

namespace Drivers { namespace IDE {
	
	#define ATA_STATE_BUSY 0x80
	#define ATA_STATE_READY 0x40
	#define ATA_STATE_FAULT 0x20
	#define ATA_STATE_SEEK_DONE 0x10
	#define ATA_STATE_REQUEST_READY 0x08
	#define ATA_STATE_CORRECTED_DATA 0x04
	#define ATA_STATE_INLEX 0x02
	#define ATA_STATE_ERROR 0x01
	
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
	
	#define ATA_ERROR_BAD_SECTOR 0x80
	#define ATA_ERROR_UNCORRECTABLE_DATA 0x40
	#define ATA_ERROR_NO_MEDIA 0x20
	#define ATA_ERROR_ID_NOT_FOUND 0x10
	#define ATA_ERROR_NO_MEDIA_2 0x08
	#define ATA_ERROR_CMD_ABORTED 0x04
	#define ATA_ERROR_TRACK_0_NOT_FOUND 0x02
	#define ATA_ERROR_NO_ADDR 0x01
	
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
	
	#define ATA_CMD_READ_PIO 0x20
	#define ATA_CMD_READ_PIO_EXT 0x24
	#define ATA_CMD_READ_DMA 0xC8
	#define ATA_CMD_READ_DMA_EXT 0x25
	#define ATA_CMD_WRITE_PIO 0x30
	#define ATA_CMD_WRITE_PIO_EXT 0x34
	#define ATA_CMD_WRITE_DMA 0xCA
	#define ATA_CMD_WRITE_DMA_EXT 0x35
	#define ATA_CMD_CACHE_FLUSH 0xE7
	#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
	#define ATA_CMD_PACKET 0xA0
	#define ATA_CMD_IDENTIFY_PACKET 0xA1
	#define ATA_CMD_IDENTIFY 0xEC
	
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
	
	#define ATA_IDENTIFY_DEVICE_TYPE 0x00
	#define ATA_IDENTIFY_CYLINDERS 0x02
	#define ATA_IDENTIFY_HEADS 0x06
	#define ATA_IDENTIFY_SECTORS 0x0C
	#define ATA_IDENTIFY_SERIAL 0x14
	#define ATA_IDENTIFY_MODEL 0x36
	#define ATA_IDENTIFY_CAPABILITIES 0x62
	#define ATA_IDENTIFY_FIELD_VALID 0x6A
	#define ATA_IDENTIFY_MAX_LBA 0x78
	#define ATA_IDENTIFY_COMMAND_SETS 0xA4
	#define ATA_IDENTIFY_MAX_LBA_EXT 0xC8
	
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
	
	#define ATA_MASTER 0xA0
	#define ATA_SLAVE 0xB0

	enum class Role : unsigned char
	{
		Master = 0xA0,
		Slave = 0xB0,
	};
	
	#define ATA_REG_DATA 0x00
	#define ATA_REG_ERROR 0x01
	#define ATA_REG_FEATURES 0x01
	#define ATA_REG_SECTOR_COUNT_0 0x02
	#define ATA_REG_LBA_LOW 0x03
	#define ATA_REG_LBA_MID 0x04
	#define ATA_REG_LBA_HIGH 0x05
	#define ATA_REG_DEVICE_SELECT 0x06
	#define ATA_REG_COMMAND 0x07
	#define ATA_REG_STATUS 0x07
	#define ATA_REG_SECTOR_COUNT_1 0x08
	#define ATA_REG_LBA_3 0x09
	#define ATA_REG_LBA_4 0x0A
	#define ATA_REG_LBA_5 0x0B
	#define ATA_REG_CONTROL 0x0C
	#define ATA_REG_ALT_STATUS 0x0C
	#define ATA_REG_DEVICE_ADDR 0x0D
	
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
	
	ENUM_OPS(ATAState, uint32_t)
	ENUM_OPS(ATAError, uint32_t)
	ENUM_OPS(ATACmd, uint32_t)
	ENUM_OPS(Channel, uint32_t)
	ENUM_OPS(Register, uint32_t)
	ENUM_OPS(ATAIdentify, uint32_t)
	
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
		static uint32_t BAR0, BAR1, BAR2, BAR3, BAR4;
		
		bool _device_initted;

		public:
		static ChannelRegister_t Channels[2];
		//static Device Devices[4];
		
		// 0 or 1 (Empty or Exists)
		unsigned char reserved;
		
		// 0 or 1 (Primary channel or secondary)
		Channel channel;
		
		// (Master or Slave)
		Role drive;
		
		// 0 or 1 (ATA or ATAPI)
		Interface type;
		
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

		// Size (in bytes) of a sector
		unsigned short sector_size;
		
		// Register Addresses
		uint16_t data;
		uint16_t error;
		uint16_t sectorCount;
		union {
			uint16_t sectorNum;
			uint16_t lbaLow;
		};
		
		union {
			uint16_t cylinderLow;
			uint16_t lbaMid;
		};
		
		union {
			uint16_t cylinderHigh;
			uint16_t lbaHigh;
		};
		
		union {
			uint16_t select;
			uint16_t head;
		};
		
		union {
			uint16_t command;
			uint16_t status;
		};
		
		union {
			uint16_t control;
			uint16_t altStatus;
		};
		uint16_t BMR;
		
		static unsigned char Read(const Channel channel, const Register reg);
		
		static void Write(const Channel, const Register, unsigned char data);
		
		static void ReadBuffer(const Channel, const Register, uint32_t* buf, uint32_t dwordCount);
		
		static unsigned char Poll(const Channel, const bool advCheck = false);
		
		static void Initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);
		static void Initialize();
		
		static inline bool Initialized()
		{
			return _initted;
		}
		
		Device(Channel, Role);
		
		bool init();
		void delay() const;
		void softReset();
		
	} __attribute__((packed));
	
	
}
}

#endif