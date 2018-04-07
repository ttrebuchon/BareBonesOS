#include "Resource_Deleter.hh"
#include "ResourceHandle.hh"


namespace Kernel
{
	void Resource_Deleter::operator()(ResourceHandle* hndl) const
	{
		hndl->cleanup();
		delete hndl;
	}
}