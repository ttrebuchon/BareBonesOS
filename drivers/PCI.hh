#ifndef INCLUDED_PCI_HH
#define INCLUDED_PCI_HH

#include <Common.h>
#include <Utils/map>

namespace Drivers {

	#define PCI_MULTI_FUNCTION_HEADER 0x80
	
	enum class PCIPort
	{
		ConfigAddress = 0xCF8,
		ConfigData = 0xCFC,
	};
	
	enum class PCIRegister
	{
		//All Headers
		VendorID = 0x00,
		DeviceID = 0x02,
		Command = 0x04,
		Status = 0x06,
		RevisionID = 0x08,
		ProgIF = 0x09,
		Subclass = 0x0A,
		ClassCode = 0x0B,
		CacheLineSize = 0x0C,
		LatencyTimer = 0x0D,
		HeaderType = 0x0E,
		BIST = 0x0F,
		
		
		BAR0 = 0x10,
		BAR1 = 0x14,
		BAR2 = 0x18,
		BAR3 = 0x1C,
		BAR4 = 0x20,
		BAR5 = 0x24,
		CardbusCIS = 0x28,
		SubsystemVendorID = 0x2C,
		SubsystemID = 0x2E,
		ExpansionROM = 0x30,
		Capabilities = 0x34,
		Reserved = 0x35,
		InterruptLine = 0x3C,
		InterruptPIN = 0x3D,
		MinGrant = 0x3E,
		MaxLatency = 0x3F,
		
		
		PrimaryBusNo = 0x18,
		SecondaryBusNo = 0x19,
		SubordinateBusNo = 0x1A,
		SecondaryLatencyTimer = 0x1B,
		IOBase = 0x1C,
		IOLimit = 0x1D,
		SecondaryStatus = 0x1E,
		MemoryBase = 0x20,
		MemoryLimit = 0x22,
		PrefetchableMemoryBase = 0x24,
		PrefetchableMemoryLimit = 0x26,
		PrefetchableBaseUpper32 = 0x28,
		PrefetchableLimitUpper32 = 0x2C,
		IOBaseUpper16 = 0x30,
		IOLimitUpper16 = 0x32,
		CapabilityPtr = 0x34,
		BridgeControl = 0x3E,
	};




	enum class PCIClass
	{
		Unclassified						= 0x00,
		MassStorage							= 0x01,
		NetworkController					= 0x02,
		DisplayController					= 0x03,
		MultimediaController				= 0x04,
		MemoryController					= 0x05,
		Bridge								= 0x06,
		SimpleCommunicationController		= 0x07,
		BaseSystemPeripheral				= 0x08,
		InputDeviceController				= 0x09,
		DockingStation						= 0x0A,
		Processor							= 0x0B,
		SerialBusController					= 0x0C,
		WirelessController					= 0x0D,
		IntelligentController				= 0x0E,
		SatelliteCommunicationsController	= 0x0F,
		EncryptionController				= 0x10,
		SignalProcessingController			= 0x11,
		ProcessingAccelerator				= 0x12,
		NonEssentialInstrumentation			= 0x13,
		Reserved1							= 0x14,
		CoProcessor							= 0x40,
		Reserved2							= 0x41,
		VendorSpecific						= 0xFF,
	};
	
	
	enum class PCIHeaderType
	{
		Device = 0x0,
		Bridge = 0x1,
		Cardbus = 0x2
	};
	
	enum class PCIType
	{
		SCSI = 0x0100,
		IDE = 0x0101,
		Floppy = 0x0102,
		RAID = 0x0104,
		ATA = 0x0105,
		SATA = 0x0106,
		Bridge = 0x0604,


		None = 0xFFFF,
		Null = -1,
	};
	
	

	
	
	
	
	
	enum class PCIVendorID : uint16_t
	{
		ATA = 0x8086,
		None = 0xFFFF
	};
	
	enum class PCIDeviceID : uint16_t
	{
		ATA = 0x7010,
		PIIX4 = 0x7111
	};
	
	
	
	
	
	
	
	class PCIDevice_t
	{
		struct alignas (uint32_t) Data_t
		{
			uint32_t zero : 2;
			uint32_t fieldNo : 6;
			uint32_t functionNo : 3;
			uint32_t deviceNo : 5;
			uint32_t busNo : 8;
			uint32_t reserved : 7;
			uint32_t enabled : 1;
			
			operator uint32_t() const
			{
				return *reinterpret_cast<const uint32_t*>(this);
			}

			Data_t() :	zero(0), fieldNo(0), functionNo(0), deviceNo(0), busNo(0),
						reserved(0), enabled(0)
			{
				
			}
			
		} __attribute__((packed));

		static_assert(sizeof(Data_t) == 4);

		public:
		Data_t data;
		
		uint32_t read(PCIRegister);
		PCIType type();
		bool reachedEnd();
		uint32_t secondaryBus();
		void write(PCIRegister, uint32_t);
	};

	inline bool operator==(const PCIDevice_t d1, const PCIDevice_t d2)
	{
		return d1.data == d2.data;
	}
	inline bool operator!=(const PCIDevice_t d1, const PCIDevice_t d2)
	{
		return d1.data != d2.data;
	}
	
	class PCI
	{
		private:
		static bool _initted;

		static Utils::map<uint8_t, Utils::map<uint8_t, Utils::map<uint8_t, PCIDevice_t>>> devices;
		
		public:
		static const PCIDevice_t NULL_DEVICE;

		static void Initialize();
		
		static uint16_t ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
		static uint16_t CheckVendor(uint8_t bus, uint8_t slot);
		static bool CheckDevice(uint8_t bus, uint8_t device);

		static PCIDevice_t ScanFunction(uint16_t vendorID, uint16_t deviceID, uint32_t bus, uint32_t device, uint32_t function, PCIType deviceType);
		static PCIDevice_t ScanDevice(uint16_t vendorID, uint16_t deviceID, uint32_t bus, uint32_t device, PCIType deviceType);
		static PCIDevice_t ScanBus(uint16_t vendorID, uint16_t deviceID, uint32_t bus, PCIType deviceType);
		static PCIDevice_t GetDevice(uint16_t vendorID, uint16_t deviceID, PCIType deviceType);
		static PCIDevice_t GetDevice(PCIVendorID vendorID, PCIDeviceID deviceID, PCIType deviceType);

		static PCIDevice_t ScanFunctionByClass(PCIType deviceType, uint32_t bus, uint32_t device, uint32_t func);
		static PCIDevice_t ScanDeviceByClass(PCIType deviceType, uint32_t bus, uint32_t device);
		static PCIDevice_t ScanBusByClass(PCIType deviceType, uint32_t bus);
		static PCIDevice_t ScanByClass(PCIType deviceType);
	};
	
}
#endif