#ifndef INCLUDED_DRIVERS_CPUID_H
#define INCLUDED_DRIVERS_CPUID_H

#include <Common.h>

#ifdef __cplusplus

#include <Utils/string>

#endif


#if !defined(__cplusplus)
	unsigned char* cpuid_string(uint32_t code);
#endif

START_NS(Drivers)
START_NS(CPUID)


C_CODE
	
	
	int cpuid_supported();
	uint32_t cpuid_cmd(uint32_t code, uint32_t* ax);
	
	
C_END


IF_CPP(Utils::string cpuid_string(uint32_t code));
IF_CPP(static_assert(sizeof(register_t) >= 4));


END_NS // (CPUID)
END_NS // (Drivers)







#endif