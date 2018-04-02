#ifndef INCLUDED_SUPPORT_SQLITE_VFS_HH
#define INCLUDED_SUPPORT_SQLITE_VFS_HH

#include <Common.h>

struct sqlite3_vfs;

namespace Support { namespace SQLite
{
	sqlite3_vfs* VFS();
}
}

#endif