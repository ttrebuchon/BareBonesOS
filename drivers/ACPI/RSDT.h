#ifndef INCLUDED_DRIVERS_ACPI_RSDT_H
#define INCLUDED_DRIVERS_ACPI_RSDT_H

#include <Common.h>
#include "RSDP.h"

#ifdef __cplusplus
	#include <Std/string.h>
#endif


START_NS(Drivers)
START_NS(ACPI)
C_CODE

#define SDT_SIG_FADT "FACP"
#define SDT_SIG_MADT "APIC"
#define SDT_SIG_BERT "BERT"
#define SDT_SIG_CPEP "CPEP"
#define SDT_SIG_DSDT "DSDT"
#define SDT_SIG_ECDT "ECDT"
#define SDT_SIG_EINJ "EINJ"
#define SDT_SIG_ERST "ERST"
#define SDT_SIG_FACS "FACS"
#define SDT_SIG_HEST "HEST"
#define SDT_SIG_MSCT "MSCT"
#define SDT_SIG_MPST "MPST"
#define SDT_SIG_PMTT "PMTT"
#define SDT_SIG_PSDT "PSDT"
#define SDT_SIG_RASF "RASF"
#define SDT_SIG_RSDT "RSDT"
#define SDT_SIG_SBST "SBST"
#define SDT_SIG_SLIT "SLIT"
#define SDT_SIG_SRAT "SRAT"
#define SDT_SIG_SSDT "SSDT"
#define SDT_SIG_XSDT "XSDT"

// Any signature starting with OEM:
// #define SDT_SIG_OEMX



// SUM(Header and Table) % 0xFF == 0
typedef struct
{
	char signature[4];
	uint32_t length; // Includes header length
	uint8_t revision;
	uint8_t checksum;
	char OEMID[6];
	char OEM_table_ID[8];
	uint32_t OEM_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
}
__attribute__((__packed__))
ACPISDTHeader;

typedef struct
{
	ACPISDTHeader head;
	uint32_t sdt_pointers[];
	
	#ifdef __cplusplus
	uint32_t pointer_count() const
	{
		return (head.length - sizeof(head)) / sizeof(uint32_t);
	}
	#endif
	
}
__attribute__((__packed__))
RSDT;


typedef struct
{
	ACPISDTHeader head;
	uint64_t sdt_pointers[];
	
	#ifdef __cplusplus
	uint32_t pointer_count() const
	{
		return (head.length - sizeof(head)) / sizeof(uint64_t);
	}
	#endif
	
}
__attribute__((__packed__))
XSDT;


int acpi_sdt_header_validate(const ACPISDTHeader*);





C_END

#ifdef __cplusplus

class RSDT_Wrapper
{
	public:
	typedef size_t size_type;
	
	const size_type entry_count;
	
	protected:
	RSDT_Wrapper(const size_type sz) : entry_count(sz)
	{
		
	}
	
	virtual const void* findSDT(const char* sig, int start = 0, int* index = nullptr) const = 0;
	
	
	
	
	public:
	
	
	const void* findFADT(int start = 0) const
	{ return findSDT(SDT_SIG_FADT, start); }
	
	
	template <class SDT>
	const SDT* find(int start = 0, int* index = nullptr) const
	{
		return (const SDT*)findSDT(SDT::Signature, start, index);
	}
	
	template <class SDT>
	const SDT** find_all(int* count_ptr, int start = 0) const
	{
		int count = 0;
		int last_index = -1;
		while (find<SDT>(last_index+1, &last_index))
		{
			++count;
		}
		
		const SDT** arr = nullptr;
		if (count > 0)
		{
			arr = new const SDT*[count];
		}
		
		last_index = -1;
		for (int i = 0; i < count; ++i)
		{
			arr[i] = find<SDT>(last_index+1, &last_index);
		}
		
		if (count_ptr)
		{
			*count_ptr = count;
		}
		return arr;
	}
};




template <class T>
class RSDT_SDT_Wrapper : public RSDT_Wrapper
{
	public:
	typedef T table_type;
	
	const table_type* const ptr;
	
	
	protected:
	const void* findSDT(const char* sig, int start = 0, int* index = nullptr) const override
	{
		for (int i = start; i < entry_count; ++i)
		{
			const ACPISDTHeader* head = (const ACPISDTHeader*)(addr_t)ptr->sdt_pointers[i];
			if (strncmp(head->signature, sig, 4) == 0)
			{
				if (index)
				{
					*index = i;
				}
				return head;
			}
		}
		if (index)
		{
			*index = -1;
		}
		return nullptr;
	}
	
	
	public:
	
	
	RSDT_SDT_Wrapper(const table_type* t) : RSDT_Wrapper((!t) ? 0 : t->pointer_count()), ptr(t)
	{
		
	}
	
};

RSDT_Wrapper* get_SDT_root();


#endif


END_NS
END_NS


#endif