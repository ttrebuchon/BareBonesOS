#ifndef INCLUDED_FILESYSTEM_API_HH
#define INCLUDED_FILESYSTEM_API_HH

#include <Common.h>

#ifdef __cplusplus
extern "C"
{
#endif


	// Change ownership of file described by file descriptor fd
	int fchown(int fd, uid_t owner, gid_t group);
	
	// Removes a directory (Only if empty)
	int rmdir(const char* path);
	
	// Write to fd
	ssize_t write(int fd, const void* buf, size_t bytes);
	
	// Read from fd at off
	ssize_t pread(int fd, void* buf, size_t bytes, off_t offset);
	
	// Read from fd
	ssize_t read(int fd, void* buf, size_t bytes);
	
#ifdef __cplusplus
}
#endif

#endif