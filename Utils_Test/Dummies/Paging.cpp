#include <kernel/Memory/PageDir.hh>
#include <kernel/Memory/Paging.hh>


namespace Kernel { namespace Memory
{
	PageDirectory* PageDirectory::Current = nullptr;
	
	PageDirectory* kernel_dir = nullptr;
	
	
}
}