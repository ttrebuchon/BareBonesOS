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
	
	uint8_t reserved;
	
	uint8_t pref_power_mngmt_profile;
	uint16_t SCI_inter;
	uint32_t SMI_cmd_port;
	uint8_t ACPI_enable;
	uint8_t ACPI_disable;
	uint8_t S4BIOS_req;
	uint8_t PSTATE_ctrl;
	uint32_t PM1a_event_block;
	uint32_t PM1b_event_block;
	uint32_t PM1a_ctrl_block;
	uint32_t PM1b_ctrl_block;
	uint32_t PM_timer_block;
	uint32_t GPE0_block;
	uint32_t GPE1_block;
	uint8_t PM1_event_len;
	uint8_t PM1_ctrl_len;
	uint8_t PM2_ctrl_len;
	uint8_t PM_timer_len;
	uint8_t GPE0_len;
	uint8_t GPE1_len;
	uint8_t GPE1_base;
	uint8_t c_state_ctrl;
	uint16_t worst_c2_laten;
	uint16_t worst_c3_laten;
	uint16_t flush_size;
	uint16_t flush_stride;
	uint8_t duty_offset;
	uint8_t duty_width;
	uint8_t day_alarm;
	uint8_t month_alarm;
	uint8_t century;
	
	
	uint16_t boot_arch_flags;
	
	uint8_t reserved2;
	
	uint32_t flags;
	
	
	GenericAddress_t reset_reg;
	
	uint8_t reset_value;
	uint8_t reserved3[3];
	
	uint64_t x_firmware_ctrl;
	uint64_t x_dsdt;
	
	GenericAddress_t x_PM1a_event_block;
	GenericAddress_t x_PM1b_event_block;
	GenericAddress_t x_PM1a_ctrl_block;
	GenericAddress_t x_PM1b_ctrl_block;
	GenericAddress_t x_PM2_ctrl_block;
	GenericAddress_t x_PM_timer_block;
	GenericAddress_t x_GPE0_block;
	GenericAddress_t x_GPE1_block;
	
	
	
	
	constexpr static const char* Signature = SDT_SIG_FADT;
	
}
__attribute__((__packed__));


C_END
END_NS // (ACPI)
END_NS // (Drivers)


#endif