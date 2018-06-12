#ifndef INCLUDED_DRIVERS_ACPI_SDTS_FADT_H
#define INCLUDED_DRIVERS_ACPI_SDTS_FADT_H

#include <Common.h>
#include <drivers/ACPI/GenericAddress.h>
#include <drivers/ACPI/RSDT.h>

START_NS(Drivers)
START_NS(ACPI)
C_CODE



struct FADT : public ACPISDTHeader
{
	uint32_t firmware_ctrl;
	uint32_t dsdt;
	
	//uint8_t 
	
	
	
	
	
	constexpr static const char* Signature = SDT_SIG_FADT;
	
}
__attribute__((__packed__));


C_END
END_NS // (ACPI)
END_NS // (Drivers)


#endif