#ifndef INCLUDED_FILESYSTEM_API_HH
#define INCLUDED_FILESYSTEM_API_HH

#include <Common.h>

#ifdef __cplusplus
extern "C"
{
#endif


	// Change ownership of file described by file descriptor FD
	int fchown(int fd, uid_t owner, gid_t group);
	
	// Change permissions of file described by FD to MODE
	int fchmod(int fd, mode_t mode);
	
	// Removes a directory (Only if empty)
	int rmdir(const char* path);
	
	// Write to FD
	ssize_t write(int fd, const void* buf, size_t bytes);
	
	// Read from FD at OFFSET
	ssize_t pread(int fd, void* buf, size_t bytes, off_t offset);
	
	// Read from FD
	ssize_t read(int fd, void* buf, size_t bytes);
	
	// Close FD
	int close(int fd);
	
	// File control
	int fcntl(int fd, int cmd, ...);
	
	// Truncate a file to LEN
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
	
	// Make a link to FROM called TO
	int link(const char* from, const char* to);
	
	// Remove the link named NAME
	int unlink(const char* name);
	
	// Create a directory named PATH with protections MODE
	int mkdir(const char* path, mode_t mode);
	
	// Stores the path pointed to by
	// the specified symlink (at PATH)
	// into BUF and returns the number of
	// chars put into BUF
	ssize_t readlink(const char* path, char* buf, size_t bufsize);
	
	// Obtains information about open file
	// at FD and stores it in BUF
	int fstat(int fd, struct stat* buf);
	
	
	
	
	
#ifdef __cplusplus
}
#endif

#endif