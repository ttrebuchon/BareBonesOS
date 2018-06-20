#include "FileNode.hh"
#include "File.hh"

namespace Kernel { namespace FS
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
		return new File(this);
		// TODO
		ASSERT(false);
	}
	
	
	
	
	File* FileNode::getFile() const
	{
		if (!file)
		{
			file = const_cast<FileNode*>(this)->initFile();
		}
		return file;
	}
	
	ResourcePtr<FileHandle> FileNode::handle()
	{
		if (!file)
		{
			file = initFile();
			ASSERT(file != nullptr);
		}
		Utils::unique_lock<Utils::mutex> lock(lock_m, Utils::try_to_lock);
		if (!lock.owns_lock())
		{
			return ResourcePtr<FileHandle>(nullptr);
		}
		assert(lock.owns_lock());
		return ResourcePtr<FileHandle>(new FileHandle(*this, *file, Utils::move(lock)));
	}
	
	bool FileNode::inUse() const
	{
		Utils::unique_lock<Utils::mutex> lock(lock_m, Utils::try_to_lock);
		if (lock.owns_lock())
		{
			return false;
		}
		return true;
	}
}
}