#ifndef INCLUDED_DRIVERS_ACPI_SDTS_MADT_H
#define INCLUDED_DRIVERS_ACPI_SDTS_MADT_H

#include <Common.h>
#include <drivers/ACPI/GenericAddress.h>
#include <drivers/ACPI/RSDT.h>

#ifdef __cplusplus
	#include <Utils/type_traits>
#endif

START_NS(Drivers)
START_NS(ACPI)


C_CODE

typedef struct _MADTEntryBase
{
	uint8_t type;
	uint8_t record_length;
} MADTEntryBase;

const MADTEntryBase* MADT_next_entry(const MADTEntryBase*);


C_END

#ifdef __cplusplus

struct MADT : public ACPISDTHeader
{
	
	uint32_t APIC_address;
	uint32_t flags;
	MADTEntryBase entries[];
	
	
	
	
	constexpr static const char* Signature = SDT_SIG_MADT;
	
	
	
	
	
	
	
	template <class TEntry>
	const TEntry* find_entry(int startIndex = 0) const
	{
		static_assert(Utils::is_base_of<MADTEntryBase, TEntry>::value);
		// TODO
		constexpr static uint8_t type = TEntry::type_id;
		
		const MADTEntryBase* it = &this->entries;
		const void* end_addr = (const void*)(((addr_t)(const ACPISDTHeader*)this) + this->length);
		
		while (it != end_addr && startIndex > 0)
		{
			it = MADT_next_entry(it);
			--startIndex;
		}
		
		while (it != end_addr)
		{
			if (it->type == type)
			{
				return (const TEntry*)it;
			}
		}
		
		return nullptr;
	}
	
	template <class TEntry>
	const TEntry** find_entries(const TEntry** results, int max_count, int startIndex = 0) const
	{
		static_assert(Utils::is_base_of<MADTEntryBase, TEntry>::value);
		constexpr static uint8_t type = TEntry::type_id;
		
		assert(results);
		
		for (int i = 0; i < max_count; ++i)
		{
			results[i] = nullptr;
		}
		
		
		const MADTEntryBase* it = &this->entries;
		const void* end_addr = (const void*)(((addr_t)(const ACPISDTHeader*)this) + this->length);
		
		while (it != end_addr && startIndex > 0)
		{
			it = MADT_next_entry(it);
			--startIndex;
		}
		
		for (int i = 0; i < max_count && it != end_addr;)
		{
			if (it->type == type)
			{
				results[i] = (const TEntry*)it;
				++i;
			}
			it = MADT_next_entry(it);
		}
		
		return results;
	}
	
	template <class TEntry>
	const TEntry** find_entries(int& count, const int startIndex = 0) const
	{
		static_assert(Utils::is_base_of<MADTEntryBase, TEntry>::value);
		constexpr static uint8_t type = TEntry::type_id;
		count = 0;
		const MADTEntryBase* it = this->entries;
		const void* end_addr = (const void*)(((addr_t)(const ACPISDTHeader*)this) + this->length);
		
		const MADTEntryBase* fwd_it = it;
		
		int start_rem = startIndex;
		while (it != end_addr && start_rem > 0)
		{
			it = MADT_next_entry(it);
			--start_rem;
		}
		
		assert(it != end_addr);
		
		fwd_it = it;
		
		while (fwd_it != end_addr)
		{
			if (fwd_it->type == type)
			{
				++count;
			}
			fwd_it = MADT_next_entry(fwd_it);
		}
		
		const TEntry** results = new const TEntry*[count];
		
		
		for (int i = 0; i < count;)
		{
			if (it->type == type)
			{
				results[i] = (const TEntry*)it;
				++i;
			}
			it = MADT_next_entry(it);
		}
		
		return results;
	}
	
	
}
__attribute__((__packed__));


struct Processor_Local_APIC_t : public MADTEntryBase
{
	constexpr static uint8_t type_id = 0;
	
	
	
	uint8_t processor_ID;
	uint8_t APIC_ID;
	uint32_t flags; // 1 = Processor Enabled
	
	
} __attribute__((__packed__));

static_assert(sizeof(Processor_Local_APIC_t) == 8);


struct IO_APIC_t : public MADTEntryBase
{
	constexpr static uint8_t type_id = 1;
	
	uint8_t processor_ID;
	uint8_t reserved;
	uint32_t IO_APIC_address;
	uint32_t global_system_interrupt_base;
};


struct Interrupt_Source_Override_t : public MADTEntryBase
{
	constexpr static uint8_t type_id = 2;
	
	uint8_t bus_source;
	uint8_t IRQ_source;
	uint32_t global_system_interrupt;
	uint16_t flags;
	
	
	
} __attribute__((__packed__));

struct NonMaskableInterrupts_t : public MADTEntryBase
{
	constexpr static uint8_t type_id = 4;
	
	// 0xFF indicates all processors
	uint8_t ACPI_processor_ID;
	
	uint16_t flags;
	uint8_t LINT_number;
	
} __attribute__((__packed__));

struct LocalAPICAddressOverride : public MADTEntryBase
{
	constexpr static uint8_t type_id = 5;
	
	uint16_t reserved;
	uint64_t local_APIC_physical_address;
	
} __attribute__((__packed__));


#endif







END_NS // (ACPI)
END_NS // (Drivers)


#endif