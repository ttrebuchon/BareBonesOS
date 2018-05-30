#include "Reader.hh"
#include <sqlite3/sqlite3.h>

namespace Support { namespace SQLite
{
	
	auto SQLiteFuncs::Prepare(db_type db, const Utils::string& text) -> stmt_type
	{
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(db.get(), text.c_str(), text.length()+1, &stmt, nullptr);
		if (!stmt)
		{
			return nullptr;
		}
		
		return stmt_type(stmt, (void(*)(sqlite3_stmt*))[](sqlite3_stmt* stmt) -> void
		{
			sqlite3_finalize(stmt);
		});
	}
	
	bool SQLiteFuncs::Step(stmt_type stmt)
	{
		if (stmt)
		{
			return sqlite3_step(stmt.get()) == SQLITE_ROW;
		}
		
		return false;
	}
	
	
	
	
	template <>
	int SQLiteFuncs::GetValue<int>(stmt_type stmt, size_t index)
	{
		return sqlite3_column_int(stmt.get(), index);
	}
	
	template <>
	long SQLiteFuncs::GetValue<long>(stmt_type stmt, size_t index)
	{
		return sqlite3_column_int(stmt.get(), index);
	}
	
	template <>
	const char* SQLiteFuncs::GetValue<const char*>(stmt_type stmt, size_t index)
	{
		return (const char*)sqlite3_column_text(stmt.get(), index);
	}
}
}