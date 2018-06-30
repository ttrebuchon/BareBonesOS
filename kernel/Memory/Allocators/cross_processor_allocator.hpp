#ifndef INCLUDED_MEMORY_ALLOCATORS_CROSS_PROCESSOR_HPP
#define INCLUDED_MEMORY_ALLOCATORS_CROSS_PROCESSOR_HPP

#include "cross_processor_allocator.hh"
#include <Utils/detail/forward.hh>

namespace Kernel { namespace Memory
{
	
	template <class T>
	template <class U, class... Args>
	void cross_proc_allocator<T>::construct(U* p, Args... args)
	{
		::new ((void*)p) U(Utils::forward<Args>(args)...);
	}
	
}
}


#endif