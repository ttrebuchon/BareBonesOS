#include <sqlite3/sqlite3.h>
#include "VFS.hh"
#include <kernel/Filesystem/API.hh>
#include <Std/cstring>
//#include <sys/file.h>
#include <Std/stat.h>
#include <kernel/Memory.h>
#include <Std/errno.h>
//#include <unistd.h>
#include <Std.h>
#include <Utils/chrono>
#include <kernel/Filesystem.hh>
#ifdef TESTING
//#include <fcntl.h>
#endif



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


#ifdef SQLITE_VFS_NATIVE_FS
	
	
	struct File : public sqlite3_file
	{
		//sqlite3_file base;
		int fd;
		
		char* aBuffer;
		int nBuffer;
		sqlite3_int64 iBufferOffst; 
	};
	
}
	
	
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
		p->aBuffer = nullptr;
		::close(p->fd);
		p->fd = -1;
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
		
		TRACE_VAL(zName);
		
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
			TRACE("Couldn't open file.");
			TRACE_VAL(strerror(errno));
			if ((flags & SQLITE_OPEN_CREATE) != SQLITE_OPEN_CREATE)
			{
				TRACE("File create flag was not provided");
			}
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
		TRACE_VAL(zPath);
		
		rc = unlink(zPath);
		if (rc != 0
		#ifndef TESTING
		&& errno == ENOENT
		#endif
		)
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
		/*else if (rc != 0 && errno == ENOENT)
		{
			return SQLITE_OK;
		}*/
		
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
	
	

























#else
































	
	
	struct File
	{
		sqlite3_file base;
		Kernel::FS::FileNode_v* node;
		Kernel::ResourcePtr<Kernel::FileHandle> hndl;
		
		/*char* aBuffer;
		int nBuffer;
		sqlite3_int64 iBufferOffst; */
	};
	
}
	constexpr bool verify_file(sqlite3_file* pFile)
	{
		assert(pFile);
		File* p = (File*)pFile;
		assert(p);
		assert(p->node);
		
		return true;
	}
	
	
	static Kernel::FS::FileNode_v* find_file(const char* zName, bool create)
	{
		auto fs = Kernel::FS::Filesystem::Current;
		assert(fs);
		auto node = fs->getNode(zName);
		Kernel::FS::FileNode_v* fnode = nullptr;
		if (node)
		{
			fnode = node->as_file();
		}
		
		if (!node && create)
		{
			TRACE_VAL(zName);
			Kernel::FS::Path_t path(zName);
			if (path.parts_length() == 1)
			{
				if (!path.part(0).empty())
				{
					fnode = fs->root()->as_directory()->add_file(path.part(0));
				}
			}
			else if (path.parts_length() > 0)
			{
				auto parent_path = path.subpath(0, path.parts_length() - 1);
				auto n_parent = fs->getNode(parent_path);
				auto parent = (n_parent ? n_parent->as_directory() : nullptr);
				if (parent && !path.back().empty())
				{
					fnode = parent->add_file(path.back());
				}
			}
		}
		
		return fnode;
	}
	
	// Write directly to the file, ignoring
	// buffer even if present
	static int directWrite(File* p, const void* zBuf, int size, sqlite3_int64 offset)
	{
		off_t realOff;
		int nWrite;
		
		assert(p->node);
		
		/*TRACE("Writing!");
		TRACE_VAL(p->node->name.c_str());
		TRACE_VAL(offset);
		TRACE_VAL(size);
		TRACE_VAL(p->node->size());*/
		uint64_t cast_size = size;
		assert(cast_size == size);
		
		auto written = p->node->write(offset, cast_size, zBuf);
		//TRACE_VAL(p->node->size());
		
		assert(written == size);
		uint8_t read_buf[size];
		assert(p->node->read(offset, cast_size, read_buf) == cast_size);
		if (memcmp(read_buf, zBuf, size) != 0)
		{
			TRACE_VAL(offset);
			TRACE_VAL(size);
		}
		assert(memcmp(read_buf, zBuf, size) == 0);
		
		//TRACE_VAL(written);
		return written;
		
		/*realOff = lseek(p->fd, offset, SEEK_SET);
		if (realOff != offset)
		{
			return SQLITE_IOERR_WRITE;
		}
		
		nWrite = write(p->fd, zBuf, size);
		if (nWrite != size)
		{
			return SQLITE_IOERR_WRITE;
		}
		
		return SQLITE_OK;*/
	}
	
	static int flushBuffer(File* p)
	{
		return SQLITE_OK;
		if (p->hndl)
		{
			p->hndl->file()->out.flush();
		}
		int rc = SQLITE_OK;
		return rc;
	}
	
	static int close(sqlite3_file* pFile)
	{
		verify_file(pFile);
		int rc;
		File* p = (File*)pFile;
		TRACE_VAL(p->node->name.c_str());
		rc = flushBuffer(p);
		p->hndl = nullptr;
		p->node = nullptr;
		//sqlite3_free(p->aBuffer);
		//::close(p->fd);
		return rc;
	}
	
	static int read(sqlite3_file* pFile, void* zBuf, int size, sqlite3_int64 off)
	{
		verify_file(pFile);
		File* p = (File*)pFile;
		int rc;
		
		/*rc = flushBuffer(p);
		if (rc != SQLITE_OK)
		{
			return rc;
		}*/
		if (!p->node)
		{
			assert(false);
			return SQLITE_IOERR_READ;
		}
		
		/*TRACE(p->node->name.c_str());
		TRACE_VAL(off);
		TRACE_VAL(size);*/
		
		if (p->node->size() < (off + size))
		{
			uint64_t read = 0;
			if (p->node->size() > off)
			{
				read = p->node->read(off, (p->node->size() - off), zBuf);
			}
			
			memset((uint8_t*)zBuf + read, 0, size - read);
			TRACE("SHORT READ.");
			return SQLITE_IOERR_SHORT_READ;
		}
		
		if (off + size > p->node->size())
		{
			TRACE_VAL(off);
			TRACE_VAL(size);
		}
		
		
		auto nRead = p->node->read(off, size, zBuf);
		assert(nRead <= size);
		if (nRead == size)
		{
			return SQLITE_OK;
		}
		else if (nRead >= 0)
		{
			memset(((uint8_t*)zBuf) + nRead, 0, size - nRead);
			TRACE("SHORT READ.");
			return SQLITE_IOERR_SHORT_READ;
		}
		
		assert(false);
		return SQLITE_IOERR_READ;
	}
	
	
	
	static int write(sqlite3_file* pFile, const void* zBuf, int size, sqlite3_int64 offset)
	{
		verify_file(pFile);
		/*int rc;
		off_t realOff;
		int nWrite;
		File* p = (File*)pFile;
		if (!p->hndl)
		{
			assert(size > 0);
			auto written = directWrite(p, zBuf, size, offset);
			if (written < size)
			{
				return SQLITE_IOERR_WRITE;
			}
			else
			{
				return SQLITE_OK;
			}
		}
		else
		{
			TRACE("Using buffered write!");
			auto f = p->hndl->file();
			auto& os = f->out;
			os.seekp(offset);
			os.write((const char*)zBuf, size);
			return SQLITE_OK;
		}*/
		int rc;
		off_t realOff;
		int nWrite;
		File* p = (File*)pFile;
		assert(size > 0);
		auto written = directWrite(p, zBuf, size, offset);
		if (written < size)
		{
			assert(false);
			return SQLITE_IOERR_WRITE;
		}
		else
		{
			return SQLITE_OK;
		}
	}
	
	static int truncate(sqlite3_file* pFile, sqlite3_int64 size)
	{
		verify_file(pFile);
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
		verify_file(pFile);
		
		
		File* p = (File*)pFile;
		int rc;
		
		rc = flushBuffer(p);
		if (rc != SQLITE_OK)
		{
			return rc;
		}
		
		// TODO: If files/disks ever get a
		// sync() or flush() member
		/*if (!p->node->flush())
		{
			return SQLITE_IOERR_FSYNC;
		}*/
		
		
		return SQLITE_OK;
	}
	
	static int fileSize(sqlite3_file* pFile, sqlite3_int64* size)
	{
		verify_file(pFile);
		File* p = (File*)pFile;
		int rc;
		struct stat s;
		
		rc = flushBuffer(p);
		if (rc != SQLITE_OK)
		{
			return rc;
		}
		
		if (!p->node)
		{
			return SQLITE_IOERR_FSTAT;
		}
		
		*size = p->node->size();
		return SQLITE_OK;
	}
	
	static int lock(sqlite3_file* pFile, int eLock)
	{
		verify_file(pFile);
		return SQLITE_OK;
	}
	
	static int unlock(sqlite3_file* pFile, int eLock)
	{
		verify_file(pFile);
		return SQLITE_OK;
	}
	
	static int checkReservedLock(sqlite3_file* pFile, int *pResOut)
	{
		verify_file(pFile);
		*pResOut = 0;
		return SQLITE_OK;
	}
	
	static int fileControl(sqlite3_file* pFile, int op, void* pArg)
	{
		verify_file(pFile);
		return SQLITE_OK;
	}
	
	static int sectorSize(sqlite3_file* pFile)
	{
		verify_file(pFile);
		return 0;
	}
	
	static int deviceCharacteristics(sqlite3_file* pFile)
	{
		verify_file(pFile);
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
		
		if (zName == 0)
		{
			assert(false);
			return SQLITE_IOERR;
		}
		
		
		/*if (flags & SQLITE_OPEN_MAIN_JOURNAL)
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
		}*/
		
		::memset(p, 0, sizeof(File));
		TRACE_VAL(zName);
		bool allow_create = ((flags & SQLITE_OPEN_CREATE) == SQLITE_OPEN_CREATE);
		auto fnode = find_file(zName, allow_create);
		/*auto fs = Kernel::FS::Filesystem::Current;
		assert(fs);
		auto node = fs->getNode(zName);
		Kernel::FS::FileNode_v* fnode = nullptr;
		if (node)
		{
			fnode = node->as_file();
		}
		
		if (!node)
		{
			Kernel::FS::Path_t path(zName);
			if (path.parts_length() == 1)
			{
				if (!path.part(0).empty())
				{
					fnode = fs->root()->as_directory()->add_file(path.part(0));
				}
			}
			else if (path.parts_length() > 0)
			{
				auto parent_path = path.subpath(0, path.parts_length() - 1);
				auto n_parent = fs->getNode(parent_path);
				auto parent = (n_parent ? n_parent->as_directory() : nullptr);
				if (parent && !path.back().empty())
				{
					fnode = parent->add_file(path.back());
				}
			}
		}*/
		
		//p->fd = ::open(zName, oflags, 0600);
		if (!fnode)
		{
			TRACE("Couldn't open.");
			if (!allow_create)
			{
				TRACE("CREATE flag was not provided.");
			}
			return SQLITE_CANTOPEN;
		}
		p->node = fnode;
		
		if (pOutFlags)
		{
			*pOutFlags = flags;
		}
		
		p->base.pMethods = &io;
		return SQLITE_OK;
	}
	
	
	static int fileDelete(sqlite3_vfs* pVfs, const char* zPath, int dirSync)
	{
		TRACE_VAL(zPath);
		
		auto n = find_file(zPath, false);
		//return ((n != nullptr) ? SQLITE_OK : SQLITE_IOERR_DELETE);
		if (n)
		{
			auto p = n->get_parent();
			if (p)
			{
				p->remove(n);
			}
		}
		
		// TODO
		return SQLITE_OK;
		
		// TODO
		assert(NOT_IMPLEMENTED);
		
		/*int rc;
		
		rc = unlink(zPath);
		if (rc != 0
		#ifndef TESTING
		&& errno == ENOENT
		#endif
		)
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
		
		return (rc == 0 ? SQLITE_OK : SQLITE_IOERR_DELETE);*/
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
		TRACE_VAL(path);
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
		
		rc = 0;
		
		auto node = find_file(path, false);
		if (node)
		{
			auto fnode = node->as_file();
			if (fnode)
			{
				TRACE("File exists.");
				rc = 0;
			}
			else
			{
				rc = -1;
			}
		}
		else
		{
			rc = -1;
		}
		
		if (rc == -1)
		{
			TRACE("File does not exist.");
			errno = EACCES;
		}
		
		
		//rc = ::access(path, eAccess);
		*pResOut = (rc == 0);
		return SQLITE_OK;
	}
	
	static int fullPath(sqlite3_vfs* pVfs, const char* name, int nPathOut, char* pathOut)
	{
		TRACE_VAL(name);
		/*auto n = find_file(name, false);
		if (!n)
		{
			strncpy(pathOut, name, nPathOut);
			return SQLITE_OK;
		}
		else
		{
			auto path = n->get_path().str();
			strncpy(pathOut, path.c_str(), nPathOut);
			return SQLITE_OK;
		}*/
		
		strncpy(pathOut, name, nPathOut-1);
		return SQLITE_OK;
		
		// TODO
		assert(NOT_IMPLEMENTED);
		
		/*char dir[MAXPATH+1];
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
		return SQLITE_OK; */
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
		std::random_shuffle(zByte, zByte+nByte);
		return SQLITE_OK;
	}
	
	static int sleep(sqlite3_vfs*, int nMicro)
	{
		assert(NOT_IMPLEMENTED);
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
	



#endif



	
	
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
	
	
}
}