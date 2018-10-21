#ifndef INCLUDED_KERNEL_SCHEDULING_PROCESS_RESOURCES_HH
#define INCLUDED_KERNEL_SCHEDULING_PROCESS_RESOURCES_HH

#include <Common.h>
#include <kernel/ResourceHandles/ResourceHandle.hh>
#include <Utils/vector>
#include <kernel/Memory/Allocators/cross_processor_allocator.hh>


namespace Kernel
{
	struct process_resources
	{
		Utils::vector<ResourcePtr<>, Memory::cross_proc_allocator<ResourcePtr<>>> handles;
	};
	
	
}

#endif