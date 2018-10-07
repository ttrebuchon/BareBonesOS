#include "LowMutex.hh"

namespace Support::threading
{
	void low_mutex::lock()
	{
		low_lock(state, false);
	}
	
	bool low_mutex::try_lock() noexcept
	{
		return low_try_lock(state);
	}
	
	void low_mutex::unlock() noexcept
	{
		low_unlock(state);
	}
}