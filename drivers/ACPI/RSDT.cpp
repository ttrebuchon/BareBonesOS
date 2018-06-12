#include "RSDT.h"
#include <kernel/Memory/PageDir.hh>
#include <kernel/Memory/Paging.hh>

namespace Drivers { namespace ACPI
{
	
	
	extern "C"
	RSDPDescriptor_t* try_find_RSDP()
	{
		#ifdef TESTING
			return nullptr;
		#endif
		uint64_t str_value = 0;
		memcpy(reinterpret_cast<char*>(&str_value), RSDP_SIGNATURE, 8);
		assert(str_value != 0);
		
		uint64_t* start = (uint64_t*)RSDP_RNG_START;
		uint64_t* end = (uint64_t*)(RSDP_RNG_END & (~0xF));
		
		static_assert(0x000FFFFF % 16 == 15);
		
		while (start <= end)
		{
			if (*start == str_value)
			{
				return (RSDPDescriptor_t*)start;
			}
			
			start += 2;
		}
		
		return nullptr;
	}
	
	
	
	
	
	
	extern "C"
	int acpi_sdt_header_validate(const ACPISDTHeader* head)
	{
		assert(head);
		unsigned char sum = 0;
		const unsigned char* table = reinterpret_cast<const unsigned char*>(head);
		for (int i = 0; i < head->length; ++i)
		{
			sum += table[i];
		}
		
		return (sum == 0);
	}
	
	
	
	
	
	
	RSDT_Wrapper* get_SDT_root()
	{
		RSDPDescriptor_t* ptr = try_find_RSDP();
		if (!ptr)
		{
			return nullptr;
		}
		
		void* phys = nullptr;
		bool ext = false;
		
		
		// This address range should be
		// identity mapped in the kernel
		// page directory
		assert(Kernel::Memory::PageDirectory::Current == Kernel::Memory::kernel_dir);
		
		if (RSDP_REV_EXT(ptr->revision))
		{
			ext = true;
			auto ptr_ext = (RSDPDescriptorExt_t*)ptr;
			phys = (void*)(addr_t)ptr_ext->XSDT_address;
			
			auto rsdt = (XSDT*)phys;
			
			return new RSDT_SDT_Wrapper<XSDT>(rsdt);
		}
		else
		{
			phys = (void*)(addr_t)ptr->RSDT_address;
			
			auto rsdt = (RSDT*)phys;
			
			return new RSDT_SDT_Wrapper<RSDT>(rsdt);
		}
	}
	
}
}