#ifndef INCLUDED_SUPPORT_SQLITE_TYPE_HH
#define INCLUDED_SUPPORT_SQLITE_TYPE_HH

#include <Common.h>


namespace Support { namespace SQLite
{
	enum sqlite_type
	{
		Integer = 1,
		Unsigned = 2,
		Float = 4,
		Text = 8,
		Blob = 16,
	};
	
	
	inline const char* sqlite_type_to_string(const sqlite_type t) noexcept
	{
		switch (t)
		{
			case Integer:
			return "INTEGER";
			
			case Unsigned:
			return "INTEGER";
			
			case Float:
			return "FLOAT";
			
			case Text:
			return "TEXT";
			
			case Blob:
			return "BLOB";
			
			default:
			assert(NOT_IMPLEMENTED);
		}
	}
	
	sqlite_type string_to_sqlite_type(const char* str) noexcept;
}
}

#endif