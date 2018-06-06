#ifdef TESTING
#include "Tests.hh"
#include <kernel/Scheduling/context.h>
#include <kernel/Utility.hh>

static void verify_read_eip();

namespace Kernel {
extern "C" void __save_context_store(context_t* c, void* ip, void* fp, void* sp);
}

extern "C"
void bar();

TEST(context)
{
	verify_read_eip();
	bar();
	void* k = nullptr;
	void* f = nullptr;
	void* i = &&Label;
	auto c = new Kernel::context_t;
	int j = 0;
	c->stack.sp = c->stack.fp = c->ip = nullptr;
	
	Label:
	if (j > 0 && j < 10)
	{
		++j;
		std::clog << j << std::endl;
		goto *i;
	}
	else if (j > 0)
	{
		return;
	}
	++j;
	
	ASM_READ_ESP(k);
	ASM_READ_EBP(f);
	if (Kernel::save_context(c))
	{
		return;
	}
	if (c->stack.sp != k || c->stack.fp != f)
	{
		std::clog << "C->SP: " << c->stack.sp << "\nK: " << k << std::endl;
		std::clog << "C->FP: " << c->stack.fp << "\nF: " << f << std::endl;
		std::clog << "C->IP: " << c->ip << std::endl;
	}
	assert(c->stack.sp == k);
	assert(c->stack.fp == f);
	std::clog << "Correctly retrieved values!\n";
	Kernel::load_destroy_context(c, [](auto c)
	{
		c->stack.sp = c->stack.fp = c->ip = nullptr;
		delete c;
	});
}


static void verify_read_eip()
{
	void* k = &&Label2;
	void* ip = nullptr;
	
	ip = (void*)Kernel::read_eip();
	Label2:
	asm volatile ("NOP");
	
	if (ip+4 != k)
	{
		std::clog << "IP: " << ip+4 << "\nLabel2: " << k << std::endl;
	}
	assert(ip+4 == k);
	
	
	
	k = &&Label3;
	ip = nullptr;
	
	ip = (void*)Kernel::read_eip();
	Label3:
	asm volatile ("NOP");
	
	if (ip+4 != k)
	{
		std::clog << "IP: " << ip+4 << "\nLabel2: " << k << std::endl;
	}
	assert(ip+4 == k);
}

volatile addr_t foo_i;

extern "C" void foobar()
{
	//std::clog << "foobar()\n";
}

extern "C" void foo()
{
	ASM_READ_EBP(foo_i);
	foobar();
}

volatile addr_t bar_i;

extern "C"
void bar()
{
	ASM_READ_EBP(bar_i);
	foo();
	
	//assert(bar_i != foo_i);
}
#endif