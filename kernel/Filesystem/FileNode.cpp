#include "FileNode.hh"
#include "File.hh"

namespace Kernel { namespace FS
{
	
	FileNode_v::FileNode_v(const NodeType t) : Node(t | NodeType::File), file(nullptr), lock_m()
	{
		this->_type |= (t | NodeType::File);
	}
	
	FileNode_v::FileNode_v() : FileNode_v(NodeType::File)
	{
		
	}
	
	FileNode_v::~FileNode_v()
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
	
	
	
	
	File* FileNode_v::initFile()
	{
		return new File(this);
		// TODO
		ASSERT(false);
	}
	
	void FileNode_v::releaseHandle(FileHandle* hndl)
	{
		assert(hndl->file() == file);
		delete file;
		file = nullptr;
	}
	
	void FileNode_v::releaseHandle(ReadFileHandle* rhndl)
	{
		/*assert(hndl->file() == file);
		delete file;
		file = nullptr;*/
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	File* FileNode_v::getFile() const
	{
		if (!file)
		{
			file = const_cast<FileNode_v*>(this)->initFile();
		}
		return file;
	}
	
	ResourcePtr<FileHandle> FileNode_v::handle()
	{
		if (!file)
		{
			file = initFile();
			ASSERT(file != nullptr);
		}
		Utils::unique_lock<Utils::mutex> lock(lock_m, Utils::try_to_lock);
		if (!lock.owns_lock())
		{
			return nullptr;
			return ResourcePtr<FileHandle>(nullptr);
		}
		assert(lock.owns_lock());
		return ResourcePtr<FileHandle>(new FileHandle(*this, *file, Utils::move(lock)));
	}
	
	bool FileNode_v::inUse() const
	{
		Utils::unique_lock<Utils::mutex> lock(lock_m, Utils::try_to_lock);
		if (lock.owns_lock())
		{
			return false;
		}
		return true;
	}
	
	
	
	FileNode::FileNode(const NodeType t) : FileNode_v(t | NodeType::File)
	{
		assert((this->_type & (t | NodeType::File)) != NodeType::Unknown);
		this->_type |= (t | NodeType::File);
	}
	
	FileNode::FileNode() : FileNode(NodeType::File)
	{
		
	}
}
}