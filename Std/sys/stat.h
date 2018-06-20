#ifndef INCLUDED_STD_SYS_STAT_H
#define INCLUDED_STD_SYS_STAT_H

#include <Types.h>

C_CODE

struct stat
{
	unsigned long st_dev;
	unsigned long st_ino;
	unsigned int st_mode;
	unsigned int st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	unsigned long st_rdev;
	unsigned long __pad1;
	long st_size;
	int st_blksize;
	int __pad2;
	long st_blocks;
	long st_atime;
	unsigned long st_atime_nsec;
	long st_mtime;
	unsigned long st_mtime_nsec;
	long st_ctime;
	unsigned long st_ctime_nsec;
	unsigned int __unused4;
	unsigned int __unused5;
};

struct stat64
{
	unsigned long st_dev;
	unsigned long st_ino;
	unsigned int st_mode;
	unsigned int st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	unsigned long st_rdev;
	unsigned long __pad1;
	long st_size;
	int st_blksize;
	int __pad2;
	long st_blocks;
	long st_atime;
	unsigned long st_atime_nsec;
	long st_mtime;
	unsigned long st_mtime_nsec;
	long st_ctime;
	unsigned long st_ctime_nsec;
	unsigned int __unused4;
	unsigned int __unused5;
};

C_END


#define S_IRWXU		00700
#define S_IRUSR		00400
#define S_IWUSR		00200
#define S_IXUSR		00100
#define S_IRWXG		00070
#define S_IRGRP		00040
#define S_IWGRP		00020
#define S_IXGRP		00010
#define S_IRWXO		00007
#define S_IROTH		00004
#define S_IWOTH		00002
#define S_IXOTH		00001



/* vvvvvvv Valid on Linux, not part of POSIX standards vvvvvvv */
#define S_ISUID		0004000
#define S_ISGID		0002000
#define S_ISVTX		0001000



#endif