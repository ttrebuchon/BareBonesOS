#include <sqlite3/sqlite3.h>
#include "VFS.hh"
#include <kernel/Filesystem/API.hh>
#include <Std/cstring>
//#include <sys/file.h>
//#include <sys/stat.h>
#include <kernel/Memory.h>
//#include <errno.h>
//#include <unistd.h>
#include <Std.h>
#include <Utils/chrono>



#ifndef SQLITE_DEFVFS_BUFFERSZ
	#define SQLITE_DEFVFS_BUFFERSZ 8192
#endif

#define MAXPATH 512



#ifndef SQLITE_TEMP_STORE
	#define TEMP_STORE_ERROR
#else
	#if SQLITE_TEMP_STORE != 3
		#define TEMP_STORE_ERROR
	#endif
#endif

#ifdef TEMP_STORE_ERROR
	#error "Must be compiled with -DSQLITE_TEMP_STORE=3"
#endif

namespace Support { namespace SQLite
{
extern "C" {
	
	
	struct File : public sqlite3_file
	{
		//sqlite3_file base;
		int fd;
		
		char* aBuffer;
		int nBuffer;
		sqlite3_int64 iBufferOffst; 
	};
	
}
	
	#ifdef FREE_TESTING
	// Write directly to the file, ignoring
	// buffer even if present
	static int directWrite(File* p, const void* zBuf, int size, sqlite3_int64 offset)
	{
		off_t realOff;
		int nWrite;
		
		realOff = lseek(p->fd, offset, SEEK_SET);
		if (realOff != offset)
		{
			return SQLITE_IOERR_WRITE;
		}
		
		nWrite = write(p->fd, zBuf, size);
		if (nWrite != size)
		{
			return SQLITE_IOERR_WRITE;
		}
		
		return SQLITE_OK;
	}
	
	static int flushBuffer(File* p)
	{
		int rc = SQLITE_OK;
		if (p->nBuffer)
		{
			rc = directWrite(p, p->aBuffer, p->nBuffer, p->iBufferOffst);
			p->nBuffer = 0;
		}
		return rc;
	}
	
	static int close(sqlite3_file* pFile)
	{
		int rc;
		File* p = (File*)pFile;
		rc = flushBuffer(p);
		sqlite3_free(p->aBuffer);
		::close(p->fd);
		return rc;
	}
	
	static int read(sqlite3_file* pFile, void* zBuf, int size, sqlite3_int64 off)
	{
		File* p = (File*)pFile;
		off_t realOff;
		int rc;
		int nRead;
		
		rc = flushBuffer(p);
		if (rc != SQLITE_OK)
		{
			return rc;
		}
		
		realOff = lseek(p->fd, off, SEEK_SET);
		if (realOff != off)
		{
			return SQLITE_IOERR_READ;
		}
		
		nRead = ::read(p->fd, zBuf, size);
		if (nRead == size)
		{
			return SQLITE_OK;
		}
		else if (nRead >= 0)
		{
			return SQLITE_IOERR_SHORT_READ;
		}
		
		return SQLITE_IOERR_READ;
	}
	
	
	
	static int write(sqlite3_file* pFile, const void* zBuf, int size, sqlite3_int64 offset)
	{
		int rc;
		off_t realOff;
		int nWrite;
		File* p = (File*)pFile;
		
		if (p->aBuffer)
		{
			char* z = (char*)zBuf;
			
			int n = size;
			sqlite3_int64 i = offset;
			
			while (n > 0)
			{
				
				int nCopy;
				
				if (p->nBuffer == SQLITE_DEFVFS_BUFFERSZ || p->iBufferOffst + p->nBuffer != i)
				{
					int rc = flushBuffer(p);
					if (rc != SQLITE_OK)
					{
						return rc;
					}
				}
				
				p->iBufferOffst = i - p->nBuffer;
				nCopy = SQLITE_DEFVFS_BUFFERSZ - p->nBuffer;
				if (nCopy > n)
				{
					nCopy = n;
				}
				
				memcpy(&p->aBuffer[p->nBuffer], z, nCopy);
				p->nBuffer += nCopy;
				
				n -= nCopy;
				i += nCopy;
				z += nCopy;
				
			}
		}
		else
		{
			return directWrite(p, zBuf, size, offset);
		}
		
		return SQLITE_OK;
	}
	
	static int truncate(sqlite3_file* pFile, sqlite3_int64 size)
	{
		#if 0
		if (ftruncate(((File*)pFile)->fd, size))
		{
			return SQLITE_IOERR_TRUNCATE;
		}
		#endif
		return SQLITE_OK;
	}
	
	static int sync(sqlite3_file* pFile, int flags)
	{
		File* p = (File*)pFile;
		int rc;
		
		rc = flushBuffer(p);
		if (rc != SQLITE_OK)
		{
			return rc;
		}
		
		rc = fsync(p->fd);
		return (rc == 0 ? SQLITE_OK : SQLITE_IOERR_FSYNC);
	}
	
	static int fileSize(sqlite3_file* pFile, sqlite3_int64* size)
	{
		File* p = (File*)pFile;
		int rc;
		struct stat s;
		
		rc = flushBuffer(p);
		if (rc != SQLITE_OK)
		{
			return rc;
		}
		
		rc = fstat(p->fd, &s);
		if (rc != 0)
		{
			return SQLITE_IOERR_FSTAT;
		}
		*size = s.st_size;
		return SQLITE_OK;
	}
	
	static int lock(sqlite3_file* pFile, int eLock)
	{
		return SQLITE_OK;
	}
	
	static int unlock(sqlite3_file* pFile, int eLock)
	{
		return SQLITE_OK;
	}
	
	static int checkReservedLock(sqlite3_file* pFile, int *pResOut)
	{
		*pResOut = 0;
		return SQLITE_OK;
	}
	
	static int fileControl(sqlite3_file* pFile, int op, void* pArg)
	{
		return SQLITE_OK;
	}
	
	static int sectorSize(sqlite3_file*)
	{
		return 0;
	}
	
	static int deviceCharacteristics(sqlite3_file*)
	{
		return 0;
	}
	
	static int open(sqlite3_vfs* pVfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags)
	{
		static const sqlite3_io_methods io =
		{
			1,
			close,
			read,
			write,
			truncate,
			sync,
			fileSize,
			lock,
			unlock,
			checkReservedLock,
			fileControl,
			sectorSize,
			deviceCharacteristics
		};
		
		File* p = (File*)pFile;
		int oflags = 0;
		char* aBuf = 0;
		
		if (zName == 0)
		{
			return SQLITE_IOERR;
		}
		
		if (flags & SQLITE_OPEN_MAIN_JOURNAL)
		{
			aBuf = (char*)sqlite3_malloc(SQLITE_DEFVFS_BUFFERSZ);
			if (!aBuf)
			{
				return SQLITE_NOMEM;
			}
		}
		
		if (flags & SQLITE_OPEN_EXCLUSIVE)
		{
			oflags |= O_EXCL;
		}
		
		if (flags & SQLITE_OPEN_CREATE)
		{
			oflags |= O_CREAT;
		}
		
		if (flags & SQLITE_OPEN_READONLY)
		{
			oflags |= O_RDONLY;
		}
		
		if (flags & SQLITE_OPEN_READWRITE)
		{
			oflags |= O_RDWR;
		}
		
		::memset(p, 0, sizeof(File));
		p->fd = ::open(zName, oflags, 0600);
		if (p->fd < 0)
		{
			sqlite3_free(aBuf);
			return SQLITE_CANTOPEN;
		}
		
		p->aBuffer = aBuf;
		
		if (pOutFlags)
		{
			*pOutFlags = flags;
		}
		
		p->pMethods = &io;
		return SQLITE_OK;
	}
	
	
	static int fileDelete(sqlite3_vfs* pVfs, const char* zPath, int dirSync)
	{
		int rc;
		
		rc = unlink(zPath);
		if (rc != 0 && errno == ENOENT)
		{
			return SQLITE_OK;
		}
		
		if (rc == 0 && dirSync)
		{
			int dfd;
			int i;
			char zDir[MAXPATH+1];
			sqlite3_snprintf(MAXPATH, zDir, "%s", zPath);
			zDir[MAXPATH] = '\0';
			for (i = strlen(zDir); i > 1 && zDir[i] != '/'; ++i) ;
			
			
			
			zDir[i] = '\0';
			dfd = ::open(zDir, O_RDONLY, 0);
			if (dfd < 0)
			{
				rc = -1;
			}
			else
			{
				rc = fsync(dfd);
				::close(dfd);
			}
		}
		
		return (rc == 0 ? SQLITE_OK : SQLITE_IOERR_DELETE);
	}
	
	
	#ifndef F_OK
		#define F_OK 0
	#endif
	
	#ifndef R_OK
		#define R_OK 4
	#endif
	
	#ifndef W_OK
		#define W_OK 2
	#endif
	
	
	static int access(sqlite3_vfs *pVfs, const char* path, int flags, int* pResOut)
	{
		int rc;
		int eAccess = F_OK;
		
		if (flags == SQLITE_ACCESS_READWRITE)
		{
			eAccess = R_OK|W_OK;
		}
		
		if (flags == SQLITE_ACCESS_READ)
		{
			eAccess = R_OK;
		}
		
		rc = ::access(path, eAccess);
		*pResOut = rc;
		return SQLITE_OK;
	}
	
	static int fullPath(sqlite3_vfs* pVfs, const char* name, int nPathOut, char* pathOut)
	{
		char dir[MAXPATH+1];
		if (name[0] == '/')
		{
			dir[0] = '\0';
		}
		else if (getcwd(dir, sizeof(dir)) == 0)
		{
			return SQLITE_IOERR;
		}
		
		dir[MAXPATH] = 0;
		sqlite3_snprintf(nPathOut, pathOut, "%s/%s", dir, name);
		pathOut[nPathOut-1] = 0;
		return SQLITE_OK; 
	}
	
	static void* dlOpen(sqlite3_vfs*, const char* path)
	{
		return nullptr;
	}
	
	static void dlError(sqlite3_vfs*, int nByte, char* msg)
	{
		sqlite3_snprintf(nByte, msg, "%s", "Loading extensions not supported");
		msg[nByte-1] = 0;
	}
	
	static void (*dlSym(sqlite3_vfs*, void* pH, const char* z))(void)
	{
		return nullptr;
	}
	
	static void dlClose(sqlite3_vfs*, void* pHandle)
	{
		
	}
	
	static int randomness(sqlite3_vfs*, int nByte, char* zByte)
	{
		//Utils::random_shuffle(zByte, zByte+nByte);
		return SQLITE_OK;
	}
	
	static int sleep(sqlite3_vfs*, int nMicro)
	{
		::sleep(nMicro/1000);
		::usleep(nMicro % 1000);
		return nMicro;
	}
	
	static int currentTime(sqlite3_vfs*, double* pTime)
	{
		*pTime = Utils::chrono::seconds(Utils::chrono::system_clock::now().time_since_epoch()).count();
		//time_t t = time(nullptr);
		//*pTime = t/86400.0 + 2440587.5;
		return SQLITE_OK;
	}
	
	
	
	sqlite3_vfs* VFS()
	{
		static sqlite3_vfs vfs = {
			1,
			sizeof(File),
			MAXPATH,
			0,
			"DefaultVFS",
			0,
			open,
			fileDelete,
			access,
			fullPath,
			dlOpen,
			dlError,
			dlSym,
			dlClose,
			randomness,
			sleep,
			currentTime
		};
		return &vfs;
	}
	
	#endif
}
}