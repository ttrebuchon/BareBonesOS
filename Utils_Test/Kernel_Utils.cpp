#include "Tests.hh"

#include <kernel/Utility.hh>

TEST(kernel_utility)
{
	addr_t x = 0;
	addr_t eip1, eip2;
	std::cout << &x << std::endl;
	eip1 = Kernel::read_eip();
	std::cout << "EIP1: " << eip1 << std::endl;
	if (eip1 != 0)
	{
		Kernel::pop_eip(eip1);
	}
	std::cout << &x << std::endl;
}