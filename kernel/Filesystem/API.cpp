#include "API.hh"
#include "FileNode.hh"
#include "Filesystem.hh"
#include "Descriptors.h"
#include "File.hh"
#include "Files/File_streambuf.hh"
#include "Files/StreamFile.hh"

using namespace Kernel;
using namespace Filesystem;

typedef char fchar_type;

extern "C"
{
	int fchown(int fd, uid_t owner, gid_t group)
	{
		// TODO
		assert(false);
	}
	
	int fchmod(int fd, mode_t mode)
	{
		// TODO
		assert(false);
	}
	
	int rmdir(const char* path)
	{
		// TODO
		assert(false);
	}
	
	ssize_t write(int fd, const void* buffer, size_t bytes)
	{
		auto file = FileDescriptors::Current->resolve(fd);
		file->out.write(reinterpret_cast<const fchar_type*>(buffer), bytes);
		return bytes;
		
		// TODO
		assert(false);
	}
	
	ssize_t pread(int fd, void* buffer, size_t bytes, off_t offset)
	{
		// TODO
		assert(false);
	}
	
	ssize_t read(int fd, void* buffer, size_t bytes)
	{
		auto file = FileDescriptors::Current->resolve(fd);
		
		file->in.read(reinterpret_cast<fchar_type*>(buffer), bytes);
		return bytes;
		// TODO
		assert(false);
	}
	
	int close(int fd)
	{
		if (FileDescriptors::Current->unmap(fd))
		{
			return 0;
		}
		else
		{
			return -1;
		}
		// TODO
		assert(false);
	}
	
	int open(const char* path, int oflags, mode_t mode)
	{
		
		if (!Filesystem::Filesystem::Current || path == nullptr)
		{
			return -1;
		}
		
		auto node = Filesystem::Filesystem::Current->getNode(path);
		if (!node)
		{
			return -1;
		}
		
		if (!node->isKind(NodeType::File))
		{
			return -1;
		}
		
		auto fnode = static_cast<FileNode*>(node);
		
		// TODO
		assert(false);
		
		return -1;
	}
	
	int fcntl(int fd, int cmd, ...)
	{
		// TODO
		assert(false);
	}
	
	int ftruncate(int fd, off_t len)
	{
		// TODO
		assert(false);
	}
	
	int truncate(const char* path, off_t len)
	{
		// TODO
		assert(false);
	}
	
	int stat(const char* __restrict path, struct stat* __restrict buf)
	{
		if (Kernel::Filesystem::Filesystem::Current)
		{
			auto n = Kernel::Filesystem::Filesystem::Current->getNode(path);
			if (n)
			{
				// TODO
				assert(false);
			}
		}
		return -1;
	}
	
	int lstat(const char* __restrict path, struct stat* __restrict buf)
	{
		if (Kernel::Filesystem::Filesystem::Current)
		{
			auto n = Kernel::Filesystem::Filesystem::Current->getNode(path);
			if (n)
			{
				// TODO
				assert(false);
			}
		}
		return -1;
	}
	
	char* getcwd(char* buf, size_t size)
	{
		// TODO
		assert(false);
	}
	
	int access(const char* path, int amode)
	{
		// TODO
		assert(false);
	}
	
	int link(const char* from, const char* to)
	{
		// TODO
		assert(false);
	}
	
	int unlink(const char* name)
	{
		// TODO
		assert(false);
	}
	
	int mkdir(const char* path, mode_t mode)
	{
		// TODO
		assert(false);
	}
	
	ssize_t readlink(const char* path, char* buf, size_t bufsize)
	{
		if (Kernel::Filesystem::Filesystem::Current)
		{
			auto n = Kernel::Filesystem::Filesystem::Current->getNode(path);
			if (n)
			{
				// TODO
				assert(false);
			}
		}
		return -1;
	}
	
	int fstat(int fd, struct stat* buf)
	{
		// TODO
		assert(false);
	}
	
	off_t lseek(int fd, off_t offset, int whence)
	{
		// TODO
		assert(false);
	}
	
	int fsync(int fd)
	{
		// TODO
		assert(false);
	}
	
	
}