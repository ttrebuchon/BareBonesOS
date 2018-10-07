#include "VFS.hh"
#include <sqlite3/sqlite3.h>

#if !defined(TESTING) || !defined(SQLITE_VFS_NATIVE_FS)

extern "C" {
	
	int sqlite3_os_init()
	{
		sqlite3_vfs_register(Support::SQLite::VFS(), 1);
		return 0;
	}
	
	int sqlite3_os_end()
	{
		//sqlite3_vfs_unregister(Support::SQLite::VFS());
		return 0;
	}
}

#endif