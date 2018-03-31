#include "API.hh"
#include "Filesystem.hh"

extern "C"
{
	int fchown(int fd, uid_t owner, gid_t group)
	{
		// TODO
		ASSERT(false);
	}
	
	int fchmod(int fd, mode_t mode)
	{
		// TODO
		ASSERT(false);
	}
	
	int rmdir(const char* path)
	{
		// TODO
		ASSERT(false);
	}
	
	ssize_t write(int fd, const void* buffer, size_t bytes)
	{
		// TODO
		ASSERT(false);
	}
	
	ssize_t pread(int fd, void* buffer, size_t bytes, off_t offset)
	{
		// TODO
		ASSERT(false);
	}
	
	ssize_t read(int fd, void* buffer, size_t bytes)
	{
		// TODO
		ASSERT(false);
	}
	
	int close(int fd)
	{
		// TODO
		ASSERT(false);
	}
	
	int fcntl(int fd, int cmd, ...)
	{
		// TODO
		ASSERT(false);
	}
	
	int ftruncate(int fd, off_t len)
	{
		// TODO
		ASSERT(false);
	}
	
	int truncate(const char* path, off_t len)
	{
		// TODO
		ASSERT(false);
	}
	
	int fstat(int fd, struct stat* buf)
	{
		// TODO
		ASSERT(false);
	}
	
	int stat(const char* __restrict path, struct stat* __restrict buf)
	{
		if (Kernel::Filesystem::Filesystem::Current)
		{
			auto n = Kernel::Filesystem::Filesystem::Current->getNode(path);
			if (n)
			{
				// TODO
				ASSERT(false);
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
				ASSERT(false);
			}
		}
		return -1;
	}
	
	char* getcwd(char* buf, size_t size)
	{
		// TODO
		ASSERT(false);
	}
	
	int access(const char* path, int amode)
	{
		// TODO
		ASSERT(false);
	}
	
	int link(const char* from, const char* to)
	{
		// TODO
		ASSERT(false);
	}
	
	int unlink(const char* name)
	{
		// TODO
		ASSERT(false);
	}
	
	int mkdir(const char* path, mode_t mode)
	{
		// TODO
		ASSERT(false);
	}
	
	ssize_t readlink(const char* path, char* buf, size_t bufsize)
	{
		if (Kernel::Filesystem::Filesystem::Current)
		{
			auto n = Kernel::Filesystem::Filesystem::Current->getNode(path);
			if (n)
			{
				// TODO
				ASSERT(false);
			}
		}
		return -1;
	}
	
}