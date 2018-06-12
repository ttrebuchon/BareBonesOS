#include "APIC.h"
#include "CPUID.h"


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
	
	
}
}