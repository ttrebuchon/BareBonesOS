#ifndef INCLUDED_FILEHANDLE_HH
#define INCLUDED_FILEHANDLE_HH

#include "ResourceHandle.hh"
#include <Utils/mutex>

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
		Filesystem::FileNode* _node;
		Filesystem::File* _file;
		Utils::unique_lock<Utils::mutex> lock;
		
		virtual void __cleanup() noexcept;
		virtual void __makeActive() noexcept {};
		virtual void __makeInactive() noexcept;
		
		public:
		// References to guarantee no null-pointers here
		FileHandle(Filesystem::FileNode&, Filesystem::File&, Utils::unique_lock<Utils::mutex>&&);
		FileHandle(const FileHandle& o) = delete;
		FileHandle(FileHandle&&) = default;
		
		virtual ~FileHandle() = default;
		
		
		
		__attribute__((__always_inline__))
		Filesystem::File* file() const noexcept
		{ return _file; }
	};
}

#endif