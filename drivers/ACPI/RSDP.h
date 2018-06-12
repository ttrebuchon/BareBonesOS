#ifndef INCLUDED_DRIVERS_ACPI_RSDP_H
#define INCLUDED_DRIVERS_ACPI_RSDP_H

#include <Common.h>

C_CODE
START_NS(Drivers)
START_NS(ACPI)

typedef struct
{
	char signature[8];
	uint8_t checksum;
	char OEMID[6];
	uint8_t revision;
	uint32_t RSDT_address; // RSDT Table PHYSICAL Address
	
}
__attribute__((__packed__))
RSDPDescriptor_t;



typedef struct
{
	RSDPDescriptor_t first;
	
	uint32_t length;
	uint64_t XSDT_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
}
__attribute__((__packed__))
RSDPDescriptorExt_t;

#define RSDP_SIGNATURE "RSD PTR "

#define RSDP_REV_EXT(X) ((X) != 0)

#define RSDP_RNG_START 0x000E0000
#define RSDP_RNG_END 0x000FFFFF


RSDPDescriptor_t* try_find_RSDP();


END_NS
END_NS
C_END


#endif