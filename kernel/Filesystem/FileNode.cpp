#include "FileNode.hh"
#include "File.hh"

namespace Kernel { namespace Filesystem
{
	
	FileNode::FileNode(const NodeType t) : Node(t | NodeType::File)
	{
		
	}
	
	FileNode::FileNode() : FileNode(NodeType::File)
	{
		
	}
	
	FileNode::~FileNode()
	{
		// TODO
		//Utils::unique_lock<mutex> lock(lock_m);
		if (file != nullptr)
		{
			// TODO
			//file->sync();
			delete file;
		}
	}
	
	
	
	
	File* FileNode::initFile()
	{
		// TODO
		assert(false);
	}
	
	
	
	
	File* FileNode::getFile() const
	{
		if (!file)
		{
			file = const_cast<FileNode*>(this)->initFile();
		}
		return file;
	}
	
	ResourcePtr<FileHandle>&& FileNode::handle()
	{
		if (!file)
		{
			file = initFile();
			assert(file != nullptr);
		}
		Utils::unique_lock<Utils::mutex> lock(lock_m, Utils::try_to_lock);
		if (!lock.owns_lock())
		{
			return Utils::move(ResourcePtr<FileHandle>(nullptr));
		}
		return Utils::move(ResourcePtr<FileHandle>(new FileHandle(*this, *file, Utils::move(lock))));
	}
	
	bool FileNode::inUse() const
	{
		if (lock_m.try_lock())
		{
			lock_m.unlock();
			return false;
		}
		return true;
	}
}
}