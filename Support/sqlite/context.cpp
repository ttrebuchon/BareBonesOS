#if __cplusplus >= 201703L
#include "context.hh"
#include <sqlite3/sqlite3.h>

#include <Common.h>
#include "sqlite_type.hh"
#include <Utils/detail/functexcept.hh>
#include <Utils/map>
#include <Utils/unordered_map>
#include <Std/stdlib.h>
#include <Std/string.h>
#include "except.hh"

namespace Support { namespace SQLite
{
	
	#ifdef DEBUG_TRACK_QUERY
	
	static Utils::vector<Utils::string> __stored_queries;
	
	#endif
	
	
	void SQLiteContext::sqlite_delete(sqlite3* obj) noexcept
	{
		if (obj)
		{
			std::clog << "Closing sqlite3 db...\n";
			int res = sqlite3_close_v2(obj);
			if (res != SQLITE_OK)
			{
				assert(NOT_IMPLEMENTED);
			}
		}
	}
	
	Utils::shared_ptr<sqlite3_stmt> SQLiteContext::Prepare(Utils::shared_ptr<sqlite3> sqlite, const Utils::string& text)
	{
		int res;
		sqlite3_stmt* _stmt;
		res = sqlite3_prepare_v2(sqlite.get(), text.c_str(), text.length()+1, &_stmt, nullptr);
		if (res != SQLITE_OK)
		{
			__throw_sqlite_error(sqlite3_errmsg(sqlite.get()));
		}
		assert(res == SQLITE_OK);
		
		
		#ifdef DEBUG_TRACK_QUERY
		__stored_queries.push_back(text);
		
		#endif
		
		return Utils::shared_ptr<sqlite3_stmt>(_stmt, sqlite3_finalize);
	}
	
	template <>
	bool SQLiteContext::Bind<Integer>(Utils::shared_ptr<sqlite3_stmt> stmt, int index, typename type_from_enum<Integer>::pass_type value)
	{
		return (sqlite3_bind_int64(stmt.get(), index, value) == SQLITE_OK);
	}
	
	template <>
	bool SQLiteContext::Bind<Float>(Utils::shared_ptr<sqlite3_stmt> stmt, int index, typename type_from_enum<Float>::pass_type value)
	{
		return (sqlite3_bind_double(stmt.get(), index, value) == SQLITE_OK);
	}
	
	template <>
	bool SQLiteContext::Bind<Text>(Utils::shared_ptr<sqlite3_stmt> stmt, int index, typename type_from_enum<Text>::pass_type value)
	{
		return (sqlite3_bind_text(stmt.get(), index, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK);
	}
	
	bool SQLiteContext::BindNull(Utils::shared_ptr<sqlite3_stmt> stmt, int index)
	{
		return (sqlite3_bind_null(stmt.get(), index) == SQLITE_OK);
	}
	
	bool SQLiteContext::Unbind(stmt_ptr stmt)
	{
		if (!stmt)
		{
			return false;
		}
		return sqlite3_clear_bindings(stmt.get()) == SQLITE_OK;
	}
	
	bool SQLiteContext::ExecuteNonQuery(Utils::shared_ptr<sqlite3_stmt> stmt)
	{
		assert(stmt);
		
		#ifdef DEBUG_TRACK_QUERY
		{
			const char* text = sqlite3_sql(stmt.get());
			Utils::string str(text);
			__stored_queries.push_back(str);
		}
		#endif
		
		int result = sqlite3_step(stmt.get());
		if (result == SQLITE_OK)
		{
			return true;
		}
		
		if (result == SQLITE_ROW)
		{
			while ((result = sqlite3_step(stmt.get())) == SQLITE_ROW) ;
			
		}
		
		return (result == SQLITE_DONE || result == SQLITE_OK);
	}
	
	bool SQLiteContext::ExecuteNonQuery(Utils::shared_ptr<sqlite3> db, const Utils::string& query)
	{
		auto callback = [](void*, auto, auto, auto) -> int
		{
			return 0;
		};
		
		#ifdef DEBUG_TRACK_QUERY
		__stored_queries.push_back(query);
		
		#endif
		
		int res = sqlite3_exec(db.get(), query.c_str(), callback, nullptr, nullptr);
		return (res == SQLITE_OK || res == SQLITE_DONE);
	}
	
	int SQLiteContext::ExecuteScalarInt(Utils::shared_ptr<sqlite3_stmt> stmt)
	{
		if (sqlite3_step(stmt.get()) != SQLITE_ROW)
		{
			sqlite3_reset(stmt.get());
			return 0;
		}
		else
		{
			int res = sqlite3_column_int(stmt.get(), 0);
			sqlite3_reset(stmt.get());
			return res;
		}
	}
	
	bool SQLiteContext::StepQuery(Utils::shared_ptr<sqlite3_stmt> stmt)
	{
		return sqlite3_step(stmt.get()) == SQLITE_ROW;
	}
	
	bool SQLiteContext::Reset(stmt_ptr stmt)
	{
		if (!stmt)
		{
			return false;
		}
		
		int res = sqlite3_reset(stmt.get());
		
		return res == SQLITE_OK;
	}
	
	int SQLiteContext::get_parameter_index(Utils::shared_ptr<sqlite3_stmt> stmt, const char* name) noexcept
	{
		#ifdef ENABLED_THREAD_LOCAL
		thread_local
		#endif
		static
		char buffer[258];
		buffer[0] = '@';
		if (strlen(name) > 256)
		{
			// TODO
			assert(NOT_IMPLEMENTED);
		}
		
		strcpy(buffer+1, name);
		
		return sqlite3_bind_parameter_index(stmt.get(), buffer);
	}
	
	int SQLiteContext::get_column_index(Utils::shared_ptr<sqlite3_stmt> stmt, const char* name)
	{
		int count = sqlite3_column_count(stmt.get());
		if (count <= 0)
		{
			return -1;
		}
		
		for (int i = 0; i < count; ++i)
		{
			if (strcmp(name, sqlite3_column_name(stmt.get(), i)) == 0)
			{
				return i;
			}
		}
		
		return -1;
	}
	
	template <>
	long long SQLiteContext::get_stmt_value<long long>(Utils::shared_ptr<sqlite3_stmt> stmt, int index)
	{
		return sqlite3_column_int64(stmt.get(), index);
	}
	
	template <>
	long double SQLiteContext::get_stmt_value<long double>(Utils::shared_ptr<sqlite3_stmt> stmt, int index)
	{
		return sqlite3_column_double(stmt.get(), index);
	}
	
	
	
	
	
	
	
	
	
	SQLiteContext::SQLiteContext(const Utils::string& database) : db(nullptr)
	{
		sqlite3* _sqlite;
		int res = sqlite3_open(database.c_str(), &_sqlite);
		if (res != SQLITE_OK)
		{
			__throw_sqlite_error(sqlite3_errmsg(_sqlite));
			assert(NOT_IMPLEMENTED);
		}
		db = Utils::shared_ptr<sqlite3>(_sqlite, &sqlite_delete);
		
		sqlite3_extended_result_codes(db.get(), 1);
		ExecuteNonQuery(db, "PRAGMA FOREIGN_KEYS = ON;");
		
		
		for (auto& table : get_tables())
		{
			schema& schm = schemas[table];
			schm.name = table;
			for (auto& row : table_info(table))
			{
				schm.columns.push_back(Utils::get<1>(row));
				if (Utils::get<5>(row))
				{
					schm.key_name = Utils::get<1>(row);
				}
			}
		}
		
		initialize_meta();
	}
	
	SQLiteContext::~SQLiteContext()
	{
		#ifdef DEBUG_TRACK_QUERY
		__stored_queries.clear();
		#endif
	}
	
	
	
	bool SQLiteContext::table_exists(const Utils::string& name) const
	{
		int res;
		Utils::shared_ptr<sqlite3_stmt> stmt;
		if (!stmt)
		{
			sqlite3_stmt* _stmt;
			constexpr const char (&query) [75] = "SELECT COUNT(*) FROM [sqlite_master] WHERE [type] = 'table' AND [name] = ?";
			res = sqlite3_prepare_v2(db.get(), query, sizeof(query), &_stmt, nullptr);
			assert(res == SQLITE_OK);
			stmt = Utils::shared_ptr<sqlite3_stmt>(_stmt, sqlite3_finalize);
			#ifdef DEBUG_TRACK_QUERY
			__stored_queries.push_back(query);
			#endif
		}
		
		res = sqlite3_bind_text(stmt.get(), 1, name.c_str(), -1, nullptr);
		assert(res == SQLITE_OK);
		
		res = sqlite3_step(stmt.get());
		assert(res == SQLITE_ROW);
		
		int count = sqlite3_column_count(stmt.get());
		assert(count == 1);
		
		int found = sqlite3_column_int(stmt.get(), 0);
		
		res = sqlite3_step(stmt.get());
		assert(res == SQLITE_DONE);
		
		sqlite3_reset(stmt.get());
		sqlite3_clear_bindings(stmt.get());
		
		
		return found > 0;
	}
	
	Utils::string SQLiteContext::get_table_key(const Utils::string& table) const
	{
		int res;
		Utils::string column = "";
		assert(column == "");
		Utils::string query = "PRAGMA table_info(";
		query += table;
		query += ")";
		auto callback = [](void* str, int count, char** results, char** colNames) -> int
		{
			assert(count == 6);
			if (results[5] != nullptr)
			{
				if (results[5][0] == '1')
				{
					Utils::string* column = (Utils::string*)str;
					*column = results[1];
					return 1;
				}
			}
			return 0;
		};
		#ifdef DEBUG_TRACK_QUERY
		__stored_queries.push_back(query);
		
		#endif
		res = sqlite3_exec(db.get(), query.c_str(), callback, (void*)&column, nullptr);
		if (column == "")
		{
			Utils::string msg = "No table named [";
			msg += table + "]";
			__throw_sqlite_error(msg.c_str());
			assert(false);
		}
		
		return column;
	}
	
	void SQLiteContext::build_table(const Utils::string& name, Utils::list<column_def>& columns, const Utils::string& key_base, bool is_abstract)
	{
		// TODO
		//assert(NOT_IMPLEMENTED);
		
		Utils::map<Utils::string, column_def*> column_map;
		
		for (auto& c : columns)
		{
			assert(column_map.count(c.name) == 0);
			column_map[c.name] = &c;
		}
		
		Utils::string key_col;
		
		Utils::string query = "CREATE TABLE [" + name + "] (";
		
		bool needComma = false;
		for (auto& p : column_map)
		{
			if (needComma)
			{
				query += ", ";
			}
			query += "[" + p.first + "] ";
			query += sqlite_type_to_string(p.second->value_type);
			if (p.second->PK)
			{
				query += " PRIMARY KEY";
				key_col = p.first;
			}
			if (!p.second->nullable)
			{
				query += " NOT NULL";
			}
			else
			{
				query += " NULL";
			}
			needComma = true;
		}
		
		for (auto& p : column_map)
		{
			if (p.second->FK)
			{
				//assert(table_exists(p.second->FK_table));
				query += ", FOREIGN KEY(";
				query += p.second->name;
				query += ") REFERENCES [";
				query += p.second->FK_table;
				query += "](";
				if (p.second->FK_table != name)
				{
					query += get_table_key(p.second->FK_table);
				}
				else
				{
					query += key_col;
				}
				query += ") ON DELETE SET NULL ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED";
			}
		}
		
		if (key_base != "")
		{
			assert(table_exists(key_base));
			query += ", FOREIGN KEY(" + key_col + ") REFERENCES [" + key_base + "](";
			assert(get_table_key(key_base) == key_col);
			query += key_col;
			query += ") ON DELETE CASCADE  ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED";
		}
		
		
		query += ")";
		
		
		
		auto stmt = Prepare(db, query);
		int res = sqlite3_step(stmt.get());
		assert(res == SQLITE_DONE);
		
		
		
		schema& schm = schemas[name];
		schm.name = name;
		for (auto& row : table_info(name))
		{
			schm.columns.push_back(Utils::get<1>(row));
			if (Utils::get<5>(row))
			{
				schm.key_name = Utils::get<1>(row);
			}
		}
		
		
		if (!prepared.update_inheritance_2)
		{
			prepared.update_inheritance_2 = Prepare(db, Utils::string("INSERT INTO [") + MetaTables::Inheritance + "] ([base], [derived], [distance], [abstract]) VALUES (@D, @D, 0, @A);");
		}
		
		
		int derived_index = get_parameter_index(prepared.update_inheritance_2, "D");
		assert(derived_index > 0);
		
		int abstract_index = get_parameter_index(prepared.update_inheritance_2, "A");
		assert(abstract_index > 0);
		
		Bind<Text>(prepared.update_inheritance_2, derived_index, name);
		Bind<Integer>(prepared.update_inheritance_2, abstract_index, (int)is_abstract);
		
		if (!ExecuteNonQuery(prepared.update_inheritance_2))
		{
			assert(NOT_IMPLEMENTED);
		}
		
		Reset(prepared.update_inheritance_2);
		
		Unbind(prepared.update_inheritance_2);
		
		
	}
	
	auto SQLiteContext::table_info(const Utils::string& table) const -> Utils::list<table_info_row>
	{
		int res;
		Utils::list<table_info_row> rows;
		Utils::string query = "PRAGMA table_info(";
		query += table;
		query += ")";
		auto callback = [](void* rows, int count, char** results, char** colNames) -> int
		{
			assert(count == 6);
			table_info_row row;
			if (results[0])
			{
				Utils::get<0>(row) = atoi(results[0]);
			}
			
			if (results[1])
			{
				Utils::get<1>(row) = results[1];
			}
			
			if (results[2])
			{
				Utils::get<2>(row) = string_to_sqlite_type(results[2]);
			}
			
			if (results[3])
			{
				if (results[3][0] == '1')
				{
					Utils::get<3>(row) = true;
				}
				else
				{
					Utils::get<3>(row) = false;
				}
			}
			else
			{
				Utils::get<3>(row) = false;
			}
			
			if (results[4])
			{
				Utils::get<4>(row) = results[4];
			}
			
			if (results[5])
			{
				if (results[5][0] == '1')
				{
					Utils::get<5>(row) = true;
				}
				else
				{
					Utils::get<5>(row) = false;
				}
			}
			else
			{
				Utils::get<5>(row) = false;
			}
			
			auto vec = (Utils::list<table_info_row>*)rows;
			vec->push_back(row);
			
			return 0;
		};
		res = sqlite3_exec(db.get(), query.c_str(), callback, (void*)&rows, nullptr);
		assert(res == SQLITE_OK);
		
		return rows;
	}
	
	Utils::list<Utils::string> SQLiteContext::get_tables() const
	{
		int res;
		Utils::list<Utils::string> tables;
		auto stmt = Prepare(db, "SELECT [name] FROM [sqlite_master] WHERE [type] = 'table';");
		
		while (sqlite3_step(stmt.get()) == SQLITE_ROW)
		{
			tables.emplace_back((const char*)sqlite3_column_text(stmt.get(), 0));
		}
		
		return tables;
	}
	
	void SQLiteContext::initialize_meta()
	{
		assert(db);
		
		if (!table_exists(MetaTables::Inheritance))
		{
			bool res = ExecuteNonQuery(db, Utils::string("CREATE TABLE [") + MetaTables::Inheritance + "] ([base] TEXT, [derived] TEXT, [distance] INTEGER, [abstract] INTEGER, PRIMARY KEY([base], [derived]));");
			assert(res);
			
		}
	}
	
	void SQLiteContext::update_inheritance_hierarchy(const char* derived, const char* base, bool abstract)
	{
		if (!prepared.update_inheritance_1)
		{
			prepared.update_inheritance_1 = Prepare(db, Utils::string("INSERT INTO [") + MetaTables::Inheritance + "] ([base], [derived], [distance], [abstract]) SELECT [base], @DERIV, [distance]+1, @ABSTRACT FROM [" + MetaTables::Inheritance + "] WHERE [derived] = @BASE;");
			
		}
		
		if (!prepared.update_inheritance_2)
		{
			prepared.update_inheritance_2 = Prepare(db, Utils::string("INSERT INTO [") + MetaTables::Inheritance + "] ([base], [derived], [distance], [abstract]) VALUES (@B, @D, 0, @A);");
		}
		
		int base_index = get_parameter_index(prepared.update_inheritance_1, "BASE");
		int derived_index = get_parameter_index(prepared.update_inheritance_1, "DERIV");
		int abstract_index = get_parameter_index(prepared.update_inheritance_1, "ABSTRACT");
		assert(base_index > 0);
		assert(derived_index > 0);
		assert(abstract_index > 0);
		Bind<Text>(prepared.update_inheritance_1, base_index, base);
		Bind<Text>(prepared.update_inheritance_1, derived_index, derived);
		
		Bind<Integer>(prepared.update_inheritance_1, abstract_index, (int)abstract);
		
		if (!ExecuteNonQuery(prepared.update_inheritance_1))
		{
			assert(NOT_IMPLEMENTED);
		}
		
		Reset(prepared.update_inheritance_1);
		
		Unbind(prepared.update_inheritance_1);
		
		/*base_index = get_parameter_index(prepared.update_inheritance_2, "B");
		assert(base_index > 0);
		derived_index = get_parameter_index(prepared.update_inheritance_2, "D");
		assert(derived_index > 0);
		
		Bind<Text>(prepared.update_inheritance_2, derived_index, derived);
		Bind<Text>(prepared.update_inheritance_2, base_index, derived);
		
		if (!ExecuteNonQuery(prepared.update_inheritance_2))
		{
			std::clog << error_msg() << std::endl;
			assert(NOT_IMPLEMENTED);
		}
		
		Reset(prepared.update_inheritance_2);
		
		Unbind(prepared.update_inheritance_2);*/
		
	}
	
	
	
	Utils::string SQLiteContext::dump_table(const Utils::string& table) const noexcept
	{
		__try
		{
			Utils::string str;
			int res;
			
			auto stmt = Prepare(db, "SELECT * FROM [" + table + "];");
			
			res = sqlite3_step(stmt.get());
			if (res == SQLITE_DONE)
			{
				return "";
			}
			
			assert(res == SQLITE_ROW);
			
			int column_count = sqlite3_column_count(stmt.get());
			
			assert(column_count > 0);
			for (int i = 0; i < column_count; ++i)
			{
				str += "[";
				str += sqlite3_column_name(stmt.get(), i);
				str += "]\t";
			}
			str += "\n----------------------------------\n";
			
			do
			{
				for (int i = 0; i < column_count; ++i)
				{
					str += "[";
					str += (const char*)sqlite3_column_text(stmt.get(), i);
					str += "]\t";
				}
				str += "\n";
			}
			while (sqlite3_step(stmt.get()) == SQLITE_ROW);
			
			return str;
			
		}
		__catch(...)
		{
			
		}
		
		return "";
	}
	
	const char* SQLiteContext::error_msg() const noexcept
	{
		return sqlite3_errmsg(db.get());
	}
	
	#ifdef DEBUG_TRACK_QUERY
	
	Utils::string SQLiteContext::dump_queries() const
	{
		bool newLine = false;
		Utils::string str = "";
		
		for (const auto& q : __stored_queries)
		{
			if (newLine)
			{
				str += "\n";
			}
			newLine = true;
			str += "'" + q + "'";
		}
		return str;
	}
	
	void SQLiteContext::clear_queries()
	{
		__stored_queries.clear();
		__stored_queries.resize(0);
	}
	
	#endif
	
}
}
#endif