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
	
	class FileNode : public Node
	{
		protected:
		typedef File* FilePtr;
		mutable FilePtr file;
		mutable Utils::mutex lock_m;
		
		virtual File* initFile();
		//virtual void releaseHandle(FileHandle*);
		//virtual void releaseHandle(ReadFileHandle*);
		
		
		FileNode(const NodeType);
		
		public:
		FileNode();
		virtual ~FileNode();
		
		virtual File* getFile() const;
		virtual ResourcePtr<FileHandle> handle();
		//virtual ReadFileHandle* readOnlyHandle();
		virtual bool inUse() const;
		//virtual bool readInUse() const;
		
		virtual size_t size() const noexcept = 0;
		
		friend FileHandle;
		friend ReadFileHandle;
	};
}
}
#endif