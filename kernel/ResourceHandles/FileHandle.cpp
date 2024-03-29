#include "FileHandle.hh"
#include <kernel/Filesystem/FileNode.hh>

namespace Kernel
{
	
	typedef Utils::unique_lock<Utils::mutex> unique_lock_t;
	FileHandle::FileHandle(FS::FileNode_v& node, FS::File& file, unique_lock_t&& lock) : ResourceHandle(), _node(&node), _file(&file), lock(Utils::forward<unique_lock_t&&>(lock))
	{
		ASSERT(this->lock.owns_lock());
	}
	
	void FileHandle::__cleanup() noexcept
	{
		_node->releaseHandle(this);
	}
}