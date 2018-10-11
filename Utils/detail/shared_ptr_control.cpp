#include "shared_ptr_control.hh"


namespace Utils { namespace detail
{
	shared_ptr_control::shared_ptr_control() : refcount(0), usecount(0), data(nullptr), deleter(nullptr), deallocator(nullptr)
	{
		
	}
	
	bool shared_ptr_control::strongRelease()
	{
		if (--usecount == 0)
		{
			assert(deleter);
			deleter(data);
		}
		
		return (--refcount != 0);
	}
	
	bool shared_ptr_control::weakRelease()
	{
		return (--refcount != 0);
	}
}
}