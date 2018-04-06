#ifndef INCLUDED_FILEHANDLE_HH
#define INCLUDED_FILEHANDLE_HH

#include "ResourceHandle.hh"

namespace Kernel
{
	namespace Filesystem
	{
		class Node;
		class FileNode;
		class File;
	}
	
	
	class FileHandle : public ResourceHandle
	{
		protected:
		Filesystem::FileNode* node;
		Filesystem::File* file;
		// Utils::unique_lock<mutex> lock;
		
		public:
		// References to guarantee no null-pointers here
		FileHandle(Filesystem::FileNode&, Filesystem::File&);
		FileHandle(const FileHandle& o) = delete;
		FileHandle(FileHandle&&) = default;
		
		virtual ~FileHandle() = default;
		
		virtual void cleanup();
	};
}

#endif