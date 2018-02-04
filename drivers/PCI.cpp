#include "PCI.hh"


namespace Drivers {
	
	void outl(unsigned char port, uint32_t data)
	{
		asm volatile ("outl %0, %1" : : "r"(port), "r"(data));
		
	}
	
	uint16_t PCI::ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
	{
		uint32_t address;
		uint32_t lbus = (uint32_t)bus;
		uint32_t lslot = (uint32_t)slot;
		uint32_t lfunc = (uint32_t)func;
		uint16_t tmp = 0;
		
		address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset << 0xfc) | ((uint32_t)0x80000000));
		
		outl(0xCF8, address);
	}
}