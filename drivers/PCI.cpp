#include "PCI.hh"
#include "PortIO.h"
#include <Common.h>

namespace Drivers {

	const PCIDevice_t PCI::NULL_DEVICE;
	static uint32_t pci_sizes[60];
	bool PCI::_initted = false;
	Utils::map<uint8_t, Utils::map<uint8_t, Utils::map<uint8_t, PCIDevice_t>>> PCI::devices;

	void PCI::Initialize()
	{
		if (_initted)
		{
			return;
		}
		_initted = true;
		pci_sizes[(uint16_t)PCIRegister::VendorID] 			= 2;
		pci_sizes[(uint16_t)PCIRegister::DeviceID]			= 2;
		pci_sizes[(uint16_t)PCIRegister::Command]			= 2;
		pci_sizes[(uint16_t)PCIRegister::Status]			= 2;
		pci_sizes[(uint16_t)PCIRegister::Subclass]			= 1;
		pci_sizes[(uint16_t)PCIRegister::ClassCode]			= 1;
		pci_sizes[(uint16_t)PCIRegister::CacheLineSize]		= 1;
		pci_sizes[(uint16_t)PCIRegister::LatencyTimer]		= 1;
		pci_sizes[(uint16_t)PCIRegister::HeaderType]		= 1;
		pci_sizes[(uint16_t)PCIRegister::BIST]				= 1;
		pci_sizes[(uint16_t)PCIRegister::BAR0]				= 4;
		pci_sizes[(uint16_t)PCIRegister::BAR1]				= 4;
		pci_sizes[(uint16_t)PCIRegister::BAR2]				= 4;
		pci_sizes[(uint16_t)PCIRegister::BAR3]				= 4;
		pci_sizes[(uint16_t)PCIRegister::BAR4]				= 4;
		pci_sizes[(uint16_t)PCIRegister::BAR5]				= 4;
		pci_sizes[(uint16_t)PCIRegister::InterruptLine]		= 1;
		pci_sizes[(uint16_t)PCIRegister::SecondaryBusNo]	= 1;

		assert(PCI::NULL_DEVICE.data == 0);
	}




	
	uint16_t PCI::ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
	{
		uint32_t address;
		uint32_t lbus = (uint32_t)bus;
		uint32_t lslot = (uint32_t)slot;
		uint32_t lfunc = (uint32_t)func;
		uint16_t tmp = 0;
		
		address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
		
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

		PCIDevice_t tmp_dev;
		tmp_dev.data.busNo = bus;
		tmp_dev.data.deviceNo = device;

		return ((PCIVendorID)tmp_dev.read(PCIRegister::VendorID) != PCIVendorID::None);
	}

	PCIDevice_t PCI::ScanFunction(uint16_t vendorID, uint16_t deviceID, uint32_t bus, uint32_t device, uint32_t function, PCIType deviceType)
	{
		if (!_initted)
		{
			Initialize();
		}

		// if (devices[bus][device].count(function))
		// {
		// 	return devices[bus][device][function];
		// }

		PCIDevice_t dev;
		dev.data.busNo = bus;
		dev.data.deviceNo = device;
		dev.data.functionNo = function;

		if (dev.type() == PCIType::Bridge)
		{
			auto tmp = PCI::ScanBus(vendorID, deviceID, dev.secondaryBus(), deviceType);
			if (tmp != NULL_DEVICE)
			{
				assert(NOT_IMPLEMENTED);
				// I think I'm supposed to return this?
				// It's been too long since I worked on this code,
				// but I don't think I was supposed to call this
				// and discard the result
			}
		}

		if (deviceType == PCIType::Null || deviceType == dev.type())
		{
			uint32_t devID = dev.read(PCIRegister::DeviceID);
			uint32_t venID = dev.read(PCIRegister::VendorID);
			if (devID == deviceID && venID == vendorID)
			{
				return dev;
			}
		}
		return NULL_DEVICE;
	}

	PCIDevice_t PCI::ScanDevice(uint16_t vendorID, uint16_t deviceID, uint32_t bus, uint32_t device, PCIType deviceType)
	{
		if (!_initted)
		{
			Initialize();
		}
		PCIDevice_t dev;
		dev.data.busNo = bus;
		dev.data.deviceNo = device;

		if ((PCIVendorID)dev.read(PCIRegister::VendorID) == PCIVendorID::None)
		{
			return NULL_DEVICE;
		}
		

		PCIDevice_t t = PCI::ScanFunction(vendorID, deviceID, bus, device, 0, deviceType);
		if (t.data != 0)
		{
			return t;
		}

		if (dev.reachedEnd())
		{
			return NULL_DEVICE;
		}

		for (int func = 1; func < 32; ++func)
		{
			if ((PCIVendorID)dev.read(PCIRegister::VendorID) != PCIVendorID::None)
			{
				t = PCI::ScanFunction(vendorID, deviceID, bus, device, func, deviceType);
				if (t.data != 0)
				{
					return t;
				}
			}
		}
		return NULL_DEVICE;
	}

	PCIDevice_t PCI::ScanBus(uint16_t vendorID, uint16_t deviceID, uint32_t bus, PCIType deviceType)
	{
		if (!_initted)
		{
			Initialize();
		}
		for (int device = 0; device < 32; ++device)
		{
			PCIDevice_t t = PCI::ScanDevice(vendorID, deviceID, bus, device, deviceType);
			if (t.data != 0)
			{
				return t;
			}
		}
		return NULL_DEVICE;
	}

	PCIDevice_t PCI::GetDevice(uint16_t vendorID, uint16_t deviceID, PCIType deviceType)
	{
		if (!_initted)
		{
			Initialize();
		}
		auto dev = PCI::ScanBus(vendorID, deviceID, 0, deviceType);

		if (dev.data != 0)
		{
			return dev;
		}

		if (dev.reachedEnd())
		{
			TRACE("PCI::GetDevice() failed...\n");
			// return NULL_DEVICE;
		}

		for (int func = 1; func < 32; ++func)
		{
			PCIDevice_t fdev;

			fdev.data.functionNo = func;

			if ((PCIVendorID)fdev.read(PCIRegister::VendorID) == PCIVendorID::None)
			{
				break;
			}

			dev = PCI::ScanBus(vendorID, deviceID, func, deviceType);
			if (dev.data != 0)
			{
				return dev;
			}

		}

		return NULL_DEVICE;
	}

	PCIDevice_t PCI::GetDevice(PCIVendorID vendorID, PCIDeviceID deviceID, PCIType deviceType)
	{
		return GetDevice((uint16_t)vendorID, (uint16_t)deviceID, deviceType);
	}

	PCIDevice_t PCI::ScanFunctionByClass(PCIType deviceType, uint32_t bus, uint32_t device, uint32_t func)
	{
		if (!_initted)
		{
			Initialize();
		}
		
		PCIDevice_t dev;
		dev.data.busNo = bus;
		dev.data.deviceNo = device;
		dev.data.functionNo = func;


		if (dev.type() == PCIType::Bridge)
		{
			PCIDevice_t otherBus = PCI::ScanBusByClass(deviceType, dev.secondaryBus());
			if (otherBus != NULL_DEVICE)
			{
				return otherBus;
			}
		}

		if (deviceType == dev.type())
		{
			uint32_t devID = dev.read(PCIRegister::DeviceID);
			uint32_t vendID = dev.read(PCIRegister::VendorID);
			assert((PCIVendorID)vendID != PCIVendorID::None);
			return dev;
		}
		else
		{
			return NULL_DEVICE;
		}
	}

	PCIDevice_t PCI::ScanDeviceByClass(PCIType deviceType, uint32_t bus, uint32_t device)
	{
		if (!_initted)
		{
			Initialize();
		}

		PCIDevice_t dev;
		dev.data.busNo = bus;
		dev.data.deviceNo = device;
		dev.data.functionNo = 0;

		if ((PCIVendorID)dev.read(PCIRegister::VendorID) == PCIVendorID::None)
		{
			return NULL_DEVICE;
		}

		PCIDevice_t t = PCI::ScanFunctionByClass(deviceType, bus, device, 0);
		if (t.data != 0)
		{
			assert(t != NULL_DEVICE);
			return t;
		}
		assert(t == NULL_DEVICE);

		auto headerType = dev.read(PCIRegister::HeaderType);

		if (dev.reachedEnd())
		{
			return NULL_DEVICE;
		}

		for (int func = 1; func < 32; ++func)
		{
			dev.data.functionNo = func;

			t = PCI::ScanFunctionByClass(deviceType, bus, device, func);
			if (t.data != 0)
			{
				assert(t != NULL_DEVICE);
				return t;
			}
			assert(t == NULL_DEVICE);
		}

		return NULL_DEVICE;
	}

	PCIDevice_t PCI::ScanBusByClass(PCIType deviceType, uint32_t bus)
	{
		if (!_initted)
		{
			Initialize();
		}

		for (int device = 0; device < 32; ++device)
		{
			if (CheckDevice(bus, device))
			{
				auto tmp = ScanDeviceByClass(deviceType, bus, device);
				if (tmp != NULL_DEVICE)
				{
					return tmp;
				}
			}
		}

		return NULL_DEVICE;
	}

	PCIDevice_t PCI::ScanByClass(PCIType deviceType)
	{
		return ScanBusByClass(deviceType, 0);
	}












	uint32_t PCIDevice_t::read(PCIRegister reg)
	{
		data.fieldNo = (((uint32_t)reg) & 0xFC) >> 2;
		data.enabled = 1;
		port_long_out((uint16_t)PCIPort::ConfigAddress, data);

		uint32_t size = pci_sizes[(uint16_t)reg];

		if (size == 1)
		{
			return port_byte_in((uint16_t)PCIPort::ConfigData + ((uint16_t)reg & 3));
		}
		else if (size == 2)
		{
			return port_word_in((uint16_t)PCIPort::ConfigData + ((uint16_t)reg & 2));
		}
		else if (size == 4)
		{
			return port_long_in((uint16_t)PCIPort::ConfigData);
		}

		return 0xFFFF;
	}

	PCIType PCIDevice_t::type()
	{
		return (PCIType)((read(PCIRegister::ClassCode) << 8) | read(PCIRegister::Subclass));
	}

	bool PCIDevice_t::reachedEnd()
	{
		return (read(PCIRegister::HeaderType) & 0x80) == 0;
	}

	uint32_t PCIDevice_t::secondaryBus()
	{
		return read(PCIRegister::SecondaryBusNo);
	}

	void PCIDevice_t::write(PCIRegister reg, uint32_t value)
	{
		data.fieldNo = (((uint32_t)reg) & 0xFC) >> 2;
		data.enabled = 1;
		port_long_out((uint16_t)PCIPort::ConfigAddress, data);

		port_long_out((uint16_t)PCIPort::ConfigData, value);
	}
}