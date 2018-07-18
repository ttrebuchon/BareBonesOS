#ifndef INCLUDED_FILEHANDLE_HH
#define INCLUDED_FILEHANDLE_HH

#include "ResourceHandle.hh"
#include <Utils/mutex>

namespace Kernel
{
	namespace FS
	{
		class Node;
		class FileNode_v;
		class File;
	}
	
	
	class FileHandle : public ResourceHandle
	{
		protected:
		FS::FileNode_v* _node;
		FS::File* _file;
		Utils::unique_lock<Utils::mutex> lock;
		
		virtual void __cleanup() noexcept;
		
		public:
		// References to guarantee no null-pointers here
		FileHandle(FS::FileNode_v&, FS::File&, Utils::unique_lock<Utils::mutex>&&);
		FileHandle(const FileHandle& o) = delete;
		FileHandle(FileHandle&&) = default;
		
		virtual ~FileHandle() = default;
		
		
		
		__attribute__((__always_inline__))
		FS::File* file() const noexcept
		{ return _file; }
	};
}

#endif