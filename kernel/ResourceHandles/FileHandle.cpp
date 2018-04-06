#include "FileHandle.hh"
#include <kernel/Filesystem/FileNode.hh>

namespace Kernel
{
	FileHandle::FileHandle(Filesystem::FileNode& node, Filesystem::File& file) : ResourceHandle(), node(&node), file(&file)
	{
		
	}
	
	void FileHandle::cleanup()
	{
		//node->releaseHandle(this);
	}
}