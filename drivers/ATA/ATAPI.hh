#ifndef INCLUDED_ATAPI_HH
#define INCLUDED_ATAPI_HH

#include <Common.h>
#include "ATA_Symbols.hh"
#include <drivers/IDE/IDE.hh>

namespace Drivers { namespace ATA {
	
	
	
	// class __Device
	// {
	// 	private:
	// 	static bool _initted;
	// 	static uint32_t BAR0, BAR1, BAR2, BAR3, BAR4;
		
	// 	bool _device_initted;

	// 	public:
	// 	static IDE::ChannelRegister_t Channels[2];
	// 	//static Device Devices[4];
		
	// 	// 0 or 1 (Empty or Exists)
	// 	unsigned char reserved;
		
	// 	// 0 or 1 (Primary channel or secondary)
	// 	unsigned char channel;
		
	// 	// (Master or Slave)
	// 	unsigned char drive;
		
	// 	// 0 or 1 (ATA or ATAPI)
	// 	Interface type;
		
	// 	// Drive Signature
	// 	unsigned short signature;
		
	// 	// Features
	// 	unsigned short capabilities;
		
	// 	// Cmd Sets Supported
	// 	unsigned int commandSets;
		
	// 	// Size (In # of sectors)
	// 	unsigned int size;
		
	// 	// Model (As string)
	// 	unsigned char model[41];

	// 	// Size (in bytes) of a sector
	// 	unsigned short sector_size;
		
	// 	// Register Addresses
	// 	uint16_t data;
	// 	uint16_t error;
	// 	uint16_t sectorCount;
	// 	union {
	// 		uint16_t sectorNum;
	// 		uint16_t lbaLow;
	// 	};
		
	// 	union {
	// 		uint16_t cylinderLow;
	// 		uint16_t lbaMid;
	// 	};
		
	// 	union {
	// 		uint16_t cylinderHigh;
	// 		uint16_t lbaHigh;
	// 	};
		
	// 	union {
	// 		uint16_t select;
	// 		uint16_t head;
	// 	};
		
	// 	union {
	// 		uint16_t command;
	// 		uint16_t status;
	// 	};
		
	// 	union {
	// 		uint16_t control;
	// 		uint16_t altStatus;
	// 	};
	// 	uint16_t BMR;

	// 	static unsigned char Read(const unsigned char channel, const uint16_t reg);
		
	// 	static void Write(const unsigned char channel, const uint16_t reg, unsigned char data);
		
	// 	static void ReadBuffer(const unsigned char channel, const uint16_t reg, uint32_t* buf, uint32_t dwordCount);
		
	// 	static unsigned char Poll(const unsigned char channel, const bool advCheck = false);

	// 	static void Select(const unsigned char channel, const unsigned char role);
		
	// 	static void Initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);
	// 	static void Initialize();
		
	// 	static inline bool Initialized()
	// 	{
	// 		return _initted;
	// 	}
		
	// 	__Device(const unsigned char channel, const unsigned char role);
		
	// 	bool init();
	// 	void delay() const;
	// 	void softReset();

	// } __attribute__((packed));
	
	

	class ATAPIDevice : public IDE::IDEDevice
	{
		public:

		private:



		// uint16_t data;
		// uint16_t error;
		// uint16_t sectorCount;
		// union {
		// 	uint16_t sectorNum;
		// 	uint16_t lbaLow;
		// };
		
		// union {
		// 	uint16_t cylinderLow;
		// 	uint16_t lbaMid;
		// };
		
		// union {
		// 	uint16_t cylinderHigh;
		// 	uint16_t lbaHigh;
		// };
		
		// union {
		// 	uint16_t select;
		// 	uint16_t head;
		// };
		
		// union {
		// 	uint16_t command;
		// 	uint16_t status;
		// };
		
		// union {
		// 	uint16_t control;
		// 	uint16_t altStatus;
		// };
		// uint16_t BMR;


		

		public:
		ATAPIDevice(IDE::ChannelRegister_t*, unsigned char role);



	};


}
}

#endif