#include "PCI.hh"
#include "PortIO.h"

namespace Drivers {
	
	uint16_t PCI::ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
	{
		uint32_t address;
		uint32_t lbus = (uint32_t)bus;
		uint32_t lslot = (uint32_t)slot;
		uint32_t lfunc = (uint32_t)func;
		uint16_t tmp = 0;
		
		address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset << 0xfc) | ((uint32_t)0x80000000));
		
		port_long_out(0xCF8, address);

		tmp = (uint16_t)((port_long_in(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
		return tmp;
	}

	uint16_t PCI::CheckVendor(uint8_t bus, uint8_t slot)
	{
		uint16_t vendor, device;
		if ((vendor = ConfigReadWord(bus, slot, 0, 0)) != 0xFFFF)
		{
			device = ConfigReadWord(bus, slot, 0, 2);
		}
		return vendor;
	}


	bool PCI::CheckDevice(uint8_t bus, uint8_t device)
	{
		uint8_t function = 0;
		uint16_t vendorID;

		//vendorID = CheckVendor(bus, 
	}
}