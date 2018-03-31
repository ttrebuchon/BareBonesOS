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
	
	// Close fd
	int close(int fd);
	
	// File control
	int fcntl(int fd, int cmd, ...);
	
	// Truncate a file to len
	int ftruncate(int fd, off_t len);
	int truncate(const char* path, off_t len);
	
	// Get File Status
	int fstat(int fd, struct stat*);
	int stat(const char* __restrict path, struct stat* __restrict buf);
	
	// Get SymLink status
	int lstat(const char* __restrict path, struct stat* __restrict buf);
	
	// Get current working directory
	char* getcwd(char* buf, size_t size);
	
	// Determine accessibility of file
	int access(const char* path, int amode);
	
	
	
	
#ifdef __cplusplus
}
#endif

#endif