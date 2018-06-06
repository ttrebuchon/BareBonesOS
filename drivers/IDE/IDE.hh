#ifndef INCLUDED_DRIVERS_IDE_IDE_HH
#define INCLUDED_DRIVERS_IDE_IDE_HH

#include <Common.h>

namespace Drivers { namespace IDE
{
    enum class Interface : unsigned char
	{
		ATA = 0x0,
		ATAPI = 0x1,
	};



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
        


        unsigned char read(const uint16_t reg);
		void write(const uint16_t reg, unsigned char data);
		void readBuffer(const uint16_t reg, uint32_t* buf, uint32_t dwordCount);
        unsigned char poll(const bool advCheck = false);
		void select(const unsigned char role);
		
	} __attribute__((packed));


    class IDEDevice
    {
        public:

		private:
		static IDEDevice* Devices[4];
        static bool __initted;
        static void Initialize();



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

        
        protected:
        Interface type;
        unsigned short signature;
        unsigned short features;
        unsigned int command_sets;
        unsigned int sector_count;
        unsigned char model[41];



		IDEDevice(ChannelRegister_t*, unsigned char role);
		

		public:
		ChannelRegister_t* const channel;
		const unsigned char role;


        Interface get_type() const noexcept
        {
            return type;
        }

		const char* get_model() const noexcept
		{
			return reinterpret_cast<const char*>(model);
		}


		static IDEDevice* Get(const unsigned char channel, const unsigned char role);
    };
	
	static_assert(sizeof(ChannelRegister_t) == 7);

    extern ChannelRegister_t Channels[2];


    struct IDEDeviceFactory
    {

        static IDEDevice* Make(Interface type, ChannelRegister_t*, unsigned char role);
    };


}
}

#endif