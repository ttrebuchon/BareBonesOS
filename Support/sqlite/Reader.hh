#ifndef INCLUDED_SUPPORT_SQLITE_READER_HH
#define INCLUDED_SUPPORT_SQLITE_READER_HH

#include <Common.h>
#include <Utils/tuple>
#include <Utils/memory>
#include "except.hh"
#include <Utils/string>

extern "C" {
	struct sqlite3;
	struct sqlite3_stmt;
	
	int sqlite3_prepare_v2(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
	
	int sqlite3_finalize(sqlite3_stmt*);
	int sqlite3_step(sqlite3_stmt*);
	
	
}

namespace Support { namespace SQLite
{
	class SQLiteFuncs
	{
		public:
		typedef Utils::shared_ptr<sqlite3_stmt> stmt_type;
		typedef Utils::shared_ptr<sqlite3> db_type;
		
		static stmt_type Prepare(db_type, const Utils::string&);
		
		static bool Step(stmt_type);
		static bool Reset(stmt_type);
		static bool IsNull(stmt_type, int index);
		
		template <class T>
		static T GetValue(stmt_type, size_t index);
	};
	
	template <>
	const char* SQLiteFuncs::GetValue<const char*>(stmt_type, size_t index);
	
	
	namespace detail
	{
		template <class T, bool b = Utils::is_pointer<T>::value>
		struct sqlite_reader_nullable_helper;
		
		template <class T>
		struct sqlite_reader_nullable_helper<T, false>
		{
			typedef typename Utils::remove_pointer<T>::type type;
			static T call(Utils::shared_ptr<sqlite3_stmt> stmt, int index)
			{
				return SQLiteFuncs::GetValue<T>(stmt, index);
			}
		};
		
		template <class T>
		struct sqlite_reader_nullable_helper<T, true>
		{
			typedef typename Utils::remove_pointer<T>::type type;
			static T call(Utils::shared_ptr<sqlite3_stmt> stmt, int index)
			{
				if (SQLiteFuncs::IsNull(stmt, index))
				{
					return nullptr;
				}
				return new type(SQLiteFuncs::GetValue<type>(stmt, index));
			}
		};
		
		template <>
		struct sqlite_reader_nullable_helper<const char*, true>
		{
			typedef const char* type;
			static const char* call(Utils::shared_ptr<sqlite3_stmt> stmt, int index)
			{
				return SQLiteFuncs::GetValue<const char*>(stmt, index);
			}
		};
		
		template <unsigned N, unsigned M, class... T>
		struct sqlite_reader_get_helper
		{
			static void call(Utils::shared_ptr<sqlite3_stmt> stmt, Utils::tuple<T...>& tup)
			{
				Utils::get<N>(tup) = sqlite_reader_nullable_helper<typename Utils::decay<decltype(Utils::get<N>(tup))>::type>::call(stmt, N);
				sqlite_reader_get_helper<N+1, M, T...>::call(stmt, tup);
			}
		};
		
		template <unsigned N, class... T>
		struct sqlite_reader_get_helper<N, N, T...>
		{
			static void call(Utils::shared_ptr<sqlite3_stmt> stmt, Utils::tuple<T...>& tup)
			{
				
			}
		};
	}
	
	
	template <class... Columns>
	class SQLiteReader
	{
		public:
		typedef Utils::tuple<Columns...> row_type;
		typedef Utils::shared_ptr<sqlite3_stmt> stmt_type;
		
		
		private:
		stmt_type statement;
		bool canStep;
		
		public:
		SQLiteReader(stmt_type statement) : statement(statement), canStep(true)
		{
			if (!statement)
			{
				__throw_null_statement_error("SQLiteReader requires a non-NULL sqlite3_stmt");
			}
		}
		
		SQLiteReader(Utils::shared_ptr<sqlite3> db, const Utils::string& text) : SQLiteReader(SQLiteFuncs::Prepare(db, text))
		{
			
		}
		
		bool next()
		{
			return (canStep = SQLiteFuncs::Step(statement));
		}
		
		row_type row()
		{
			row_type values;
			detail::sqlite_reader_get_helper<0, sizeof...(Columns), Columns...>::call(statement, values);
			return values;
		}
		
		bool reset()
		{
			return SQLiteFuncs::Reset(statement);
		}
	};
	
	
}
}

#endif