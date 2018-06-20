#include "ResourceHandle.hh"


namespace Kernel
{
	ResourceHandle::~ResourceHandle() noexcept
	{
		assert(__cleaned);
	}
	
	
	void ResourceHandle::cleanup()
	{
		if (!__cleaned)
		{
			__cleaned = true;
			this->__cleanup();
		}
	}
	
	void ResourceHandle::makeActive()
	{
		if (!__active)
		{
			__active = true;
			this->__makeActive();
		}
	}
	
	void ResourceHandle::makeInactive()
	{
		if (__active)
		{
			__active = false;
			this->__makeInactive();
		}
	}
}