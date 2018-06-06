#ifdef TESTING
#include "Tests.hh"
#include <drivers/IDE/ATADisk.hh>
#include <drivers/IDE/DMA.hh>
#include <drivers/PCI.hh>
#include <Utils/EnumOperators.hh>

struct IDEChannelRegisters {
   unsigned short base;  // I/O Base.
   unsigned short ctrl;  // Control Base
   unsigned short bmide; // Bus Master IDE
   unsigned char  nIEN;  // nIEN (No Interrupt);
} __attribute__((packed)) channels[2];

static_assert(sizeof(unsigned short) == 2);
static_assert(sizeof(unsigned char) == 1);

TEST(IDE)
{
	int y[2];
	std::cout << "Size: " << sizeof(IDEChannelRegisters) << std::endl;
	std::cout << "Size: " << sizeof(channels) << std::endl;
	std::cout << "Size: " << sizeof(Drivers::IDE::Device) << std::endl;
	assert(true);
	static_assert(sizeof(Drivers::IDE::PRDT<10>) == 80);
	
	Drivers::IDE::PRDT<10>::Create();
	
	Drivers::IDE::DMADrive dr(Drivers::IDE::Channel::Primary, Drivers::IDE::Role::Master);
	
	assert(dr.read(0, 1024, nullptr) == -1);
}
#endif