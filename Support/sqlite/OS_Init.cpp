#include "VFS.hh"
#include <sqlite3/sqlite3.h>


extern "C" {
	
	int sqlite3_os_init()
	{
		sqlite3_vfs_register(Support::SQLite::VFS(), 1);
	}
	
	int sqlite3_os_end()
	{
		//sqlite3_vfs_unregister(Support::SQLite::VFS());
	}
}