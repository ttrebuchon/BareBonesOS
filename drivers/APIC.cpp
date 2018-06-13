#include "APIC.h"
#include "CPUID.h"
#include "ACPI/RSDT.h"
#include "ACPI/SDTs/FADT.h"
#include "ACPI/SDTs/MADT.h"


namespace Drivers { namespace APIC
{
	
	extern "C"
	int apic_supported()
	{
		if (CPUID::cpuid_supported())
		{
			return (CPUID::cpuid_cmd(0x1, nullptr) & (1 << 9)) != 0;
		}
		else
		{
			return 0;
		}
	}
	
	
	
	extern "C"
	int apic_enable()
	{
		if (!apic_supported())
		{
			return -1;
		}
		
		auto rsdt = ACPI::get_SDT_root();
		if (!rsdt)
		{
			return -1;
		}
		
		auto fadt = rsdt->findFADT();
		
		
		if (!fadt)
		{
			delete rsdt;
			return -1;
		}
		
		int madt_count;
		auto madts = rsdt->find_all<ACPI::MADT>(&madt_count);
		
		if (!madts)
		{
			delete rsdt;
			return -1;
		}
		
		
		int cpu_count = 0;
		
		for (int i = 0; i < madt_count; ++i)
		{
			auto madt_info = madts[i]->find_entries<ACPI::Processor_Local_APIC_t>(cpu_count);
			assert(cpu_count > 0);
			
			
			delete[] madt_info;
		}
		
		
		
		delete[] madts;
		delete rsdt;
	}
	
	
	
	
	extern "C" void apic_write(uint32_t base, uint32_t reg, uint32_t data)
	{
		*(volatile uint32_t*)(addr_t)(base + reg) = data;
	}
	
	
	
	
	
	
	
	
	
	
	void APICDevice::write(uint32_t reg, uint32_t data)
	{
		apic_write(apic.base_address, reg, data);
	}
}
}