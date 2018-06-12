#include "Tests.hh"
#include <drivers/CPUID.h>
#include <drivers/APIC.h>
#include <drivers/ACPI/RSDT.h>
#include <drivers/ACPI/SDTs/FADT.h>
#include <drivers/ACPI/SDTs/MADT.h>


TEST(SMP)
{
	auto vendor = Drivers::CPUID::cpuid_string(0);
	
	std::cout << vendor.c_str() << std::endl;
	
	assert(Drivers::APIC::apic_supported());
	
	Drivers::ACPI::RSDT_Wrapper* rsdt = Drivers::ACPI::get_SDT_root();
	if (rsdt)
	{
		assert(!rsdt->find<Drivers::ACPI::FADT>());
	}
	
	
}