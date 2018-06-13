#include "MADT.h"


namespace Drivers { namespace ACPI
{
	
	extern "C"
	const MADTEntryBase* MADT_next_entry(const MADTEntryBase* entry)
	{
		return reinterpret_cast<const MADTEntryBase*>(
			reinterpret_cast<const unsigned char*>(entry) + entry->record_length
		);
	}
	
}
}