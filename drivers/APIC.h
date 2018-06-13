#ifndef INCLUDED_DRIVERS_APIC_H
#define INCLUDED_DRIVERS_APIC_H

#include <Common.h>

#define APIC_FLAG_LOW 0x2
#define APIC_FLAG_LEVEL 0x8

START_NS(Drivers)
START_NS(APIC)


C_CODE
	
	
	int apic_supported();
	int apic_enable();
	
	typedef struct
	{
		uint8_t CPU_id;
		uint8_t id;
		uint32_t flags;
		addr_t base_address;
	}
	APICLocal_t;
	
	typedef struct
	{
		uint8_t id;
		uint32_t address;
		uint32_t global_sys_int_base;
	}
	APICIO_t;
	
	typedef struct
	{
		uint8_t bus_src;
		uint8_t irq_src;
		uint32_t global_sys_int;
		uint16_t flags;
	}
	APICSourceOverride_t;
	
	void apic_write(uint32_t base_addr, uint32_t reg_off, uint32_t data);
	
	
C_END


#ifdef __cplusplus

class APICDevice
{
	public:
	typedef APICLocal_t local_type;
	typedef APICIO_t IO_type;
	typedef APICSourceOverride_t override_type;
	
	protected:
	local_type apic;
	
	public:
	
	APICDevice(const local_type& ld) : apic(ld)
	{
		
	}
	
	void write(uint32_t reg, uint32_t data);
	
	
};

#endif




END_NS // (APIC)
END_NS // (Drivers)







#endif