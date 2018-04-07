#include "shared_ptr_control.hh"


namespace Utils { namespace detail
{
	shared_ptr_control::shared_ptr_control() : refcount(0), usecount(0), obj(nullptr), deleter_obj(nullptr), deleter(nullptr), dealloc_object(nullptr), dealloc(nullptr)
	{
		
	}
	
	bool shared_ptr_control::strongRelease()
	{
		if (--usecount == 0)
		{
			deleter(deleter_obj, obj);
			obj = nullptr;
		}
		
		return (--refcount != 0);
	}
	
	bool shared_ptr_control::weakRelease()
	{
		return (--refcount != 0);
	}
}
}