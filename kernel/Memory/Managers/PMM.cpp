#include "PMM.hh"
#include <kernel/Memory/PhysicalMemory.hh>

namespace Kernel { namespace Memory
{
	
	PMM::PMM(frames_type& f) noexcept : frames(f)
	{
		PhysicalMemory::Initialize();
	}
	
}
}