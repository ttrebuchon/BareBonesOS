#ifndef INCLUDED_FILESYSTEM_FILENODE_HH
#define INCLUDED_FILESYSTEM_FILENODE_HH

#include <Common.h>
#include "Node.hh"
#include <Utils/mutex>
#include <kernel/ResourceHandles/FileHandle.hh>

namespace Kernel
{
	class ReadFileHandle;
}

namespace Kernel { namespace FS {
	
	class File;
	
	
	class FileNode_v : public virtual Node
	{
		protected:
		typedef File* FilePtr;
		mutable FilePtr file;
		mutable Utils::mutex lock_m;
		
		virtual File* initFile();
		//virtual void releaseHandle(FileHandle*);
		//virtual void releaseHandle(ReadFileHandle*);
		
		
		FileNode_v(const NodeType);
		
		public:
		FileNode_v();
		virtual ~FileNode_v();
		
		virtual File* getFile() const;
		virtual ResourcePtr<FileHandle> handle();
		//virtual ReadFileHandle* readOnlyHandle();
		virtual bool inUse() const;
		//virtual bool readInUse() const;
		
		virtual size_t size() const noexcept = 0;
		
		friend FileHandle;
		friend ReadFileHandle;
	};
	
	class FileNode : public FileNode_v
	{
		protected:
		using FileNode_v::FilePtr;
		//typedef File* FilePtr;
		//mutable FilePtr file;
		//mutable Utils::mutex lock_m;
		
		//virtual File* initFile();
		//virtual void releaseHandle(FileHandle*);
		//virtual void releaseHandle(ReadFileHandle*);
		
		
		FileNode(const NodeType);
		
		public:
		FileNode();
		virtual ~FileNode() = default;
		
		//virtual File* getFile() const;
		//virtual ResourcePtr<FileHandle> handle();
		//virtual ReadFileHandle* readOnlyHandle();
		//virtual bool inUse() const;
		//virtual bool readInUse() const;
		
		//virtual size_t size() const noexcept = 0;
		
		friend FileHandle;
		friend ReadFileHandle;
		
		virtual const FileNode* as_file() const noexcept override
		{ return this; }
		virtual FileNode* as_file() noexcept override
		{ return this; }
		
		virtual const DirectoryNode_v* as_directory() const noexcept override
		{ return nullptr; }
		virtual DirectoryNode_v* as_directory() noexcept override
		{ return nullptr; }
	};
}
}
#endif