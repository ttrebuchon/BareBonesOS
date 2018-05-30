#include "sqlite_type.hh"
#include <Std/string.h>

namespace Support { namespace SQLite
{
	
	sqlite_type string_to_sqlite_type(const char* str) noexcept
	{
		if (strcmp(str, "INTEGER") == 0)
		{
			return Integer;
		}
		
		if (strcmp(str, "FLOAT") == 0)
		{
			return Float;
		}
		
		if (strcmp(str, "TEXT") == 0)
		{
			return Text;
		}
		
		assert(NOT_IMPLEMENTED);
	}
	
}
}