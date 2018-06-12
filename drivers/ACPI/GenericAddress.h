#ifndef INCLUDED_DRIVERS_ACPI_GENERICADDRESS_H
#define INCLUDED_DRIVERS_ACPI_GENERICADDRESS_H

#include <Common.h>

START_NS(Drivers)
START_NS(ACPI)
C_CODE



typedef struct
{
	uint8_t address_space;
	uint8_t bit_width;
	uint8_t bit_off;
	uint8_t access_size;
	uint64_t address;
	
}
__attribute__((__packed__))
GenericAddress_t;


C_END
END_NS // (ACPI)
END_NS // (Drivers)


#endif