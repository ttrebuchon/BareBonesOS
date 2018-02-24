#include "Tests.hh"
#include <drivers/IDE/IDEDisk.hh>
#include <kernel/utils/EnumOperators.hh>

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
}