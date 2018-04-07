#include "FileHandle.hh"
#include <kernel/Filesystem/FileNode.hh>

namespace Kernel
{
	
	typedef Utils::unique_lock<Utils::mutex> unique_lock_t;
	FileHandle::FileHandle(Filesystem::FileNode& node, Filesystem::File& file, unique_lock_t&& lock) : ResourceHandle(), _node(&node), _file(&file), lock(Utils::forward<unique_lock_t&&>(lock))
	{
		ASSERT(lock.owns_lock());
	}
	
	void FileHandle::__cleanup()
	{
		//node->releaseHandle(this);
	}
}