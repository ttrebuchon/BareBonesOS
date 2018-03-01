#include "DMA.hh"
#include <drivers/PortIO.h>

namespace Drivers { namespace IDE {
	
	DMADrive::DMADrive(const Channel chan, const Role role) : DMADrive(new Device(chan, role)/*&Device::Devices[2*(int)(chan == Channel::Secondary) + (int)(role == Role::Slave)]*/)
	{}
	
	DMADrive::DMADrive(Device* dev) : Disk(), BMR(), _dir(), _state(false), dev(dev), prdt(nullptr), prdt_phys(0x0)
	{
		prdt = PRDT<10>::Create(&prdt_phys);
	}
	
	
}
}