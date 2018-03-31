#include "API.hh"


extern "C"
{
	int fchown(int fd, uid_t owner, gid_t group)
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
		// TODO
		ASSERT(false);
	}
	
	int lstat(const char* __restrict path, struct stat* __restrict buf)
	{
		// TODO
		ASSERT(false);
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
	
}