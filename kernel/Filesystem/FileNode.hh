#ifndef INCLUDED_FILESYSTEM_FILENODE_HH
#define INCLUDED_FILESYSTEM_FILENODE_HH

#include <Common.h>
#include "Node.hh"
#include <Utils/mutex>
#include <Utils/unique_ptr>

namespace Kernel
{
	class FileHandle;
	class ReadFileHandle;
}

namespace Kernel { namespace Filesystem {
	
	class File;
	
	class FileNode : public Node
	{
		protected:
		File* file;
		Utils::mutex lock_m;
		
		//virtual void releaseHandle(FileHandle*);
		//virtual void releaseHandle(ReadFileHandle*);
		
		
		FileNode(const NodeType);
		
		public:
		FileNode();
		virtual ~FileNode();
		
		virtual File* getFile() const;
		virtual Utils::unique_ptr<FileHandle> handle();
		//virtual ReadFileHandle* readOnlyHandle();
		virtual bool inUse() const;
		//virtual bool readInUse() const;
		
		friend FileHandle;
		friend ReadFileHandle;
	};
}
}
#endif