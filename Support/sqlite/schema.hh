#if __cplusplus >= 201703L
#ifndef INCLUDED_SUPPORT_SQLITE_SCHEMA_HH
#define INCLUDED_SUPPORT_SQLITE_SCHEMA_HH

#include <Common.h>
#include <Utils/string>
#include <Utils/map>
#include <Utils/vector>

namespace Support { namespace SQLite
{
	struct schema
	{
		Utils::string name;
		Utils::string key_name;
		Utils::vector<Utils::string> columns;
		
		schema() : name(), key_name(), columns()
		{
			
		}
		
		
		schema(const Utils::string& name, const Utils::string& key_name, int columns) : name(name), key_name(key_name), columns()
		{
			this->columns.reserve(columns);
		}
		
		
	};
}
}

#endif
#endif