#ifndef INCLUDED_PCI_HH
#define INCLUDED_PCI_HH

#include <Common.h>

namespace Drivers {
	
	
	class PCI
	{
		
		
		public:
		
		
		static uint16_t ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
		static uint16_t CheckVendor(uint8_t bus, uint8_t slot);
		static bool CheckDevice(uint8_t bus, uint8_t device);
	};
	
}
#endif