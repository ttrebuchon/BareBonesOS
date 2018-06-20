#include "VFS.hh"
#include <sqlite3/sqlite3.h>

#ifndef TESTING

extern "C" {
	
	int sqlite3_os_init()
	{
		return sqlite3_vfs_register(Support::SQLite::VFS(), 1);
	}
	
	int sqlite3_os_end()
	{
		return sqlite3_vfs_unregister(Support::SQLite::VFS());
	}
}

#endif