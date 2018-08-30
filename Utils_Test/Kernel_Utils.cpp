#include "Tests.hh"

#include <kernel/Utility.hh>
#include <csetjmp>

int foo(auto buf);
std::jmp_buf jbuf;

TEST(kernel_utility)
{
	volatile addr_t x = 0;
	addr_t eip1, eip2;
	std::cout << &x << std::endl;
	eip1 = Kernel::read_eip();
	std::cout << "EIP1: " << eip1 << std::endl;
	if (eip1 != 0)
	{
		Kernel::pop_eip(eip1);
	}
	std::cout << &x << std::endl;
	
	x = setjmp(jbuf);
	std::cout << "x: " << x << std::endl;
	if (x < 10)
	{
		foo(x+1);
	}
	ASM_READ_ESP(x);
	addr_t y;
	ASM_MOV(%0, SP, : "=r"(x));
	ASM_READ_ESP(y);
	ASSERTEQ(x, y);
}

int foo(auto b)
{
	Foo_t f(b);
	std::longjmp(jbuf, b);
	/*volatile Foo_t f(3);
	if (setjmp(b) == 2)
	{
		f.n = 100;
		return foo(b);
	}
	return f.n;*/
	__builtin_unreachable();
}