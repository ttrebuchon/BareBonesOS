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
	
}