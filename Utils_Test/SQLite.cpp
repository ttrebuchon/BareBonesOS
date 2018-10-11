#include "Tests.hh"
#include <sqlite3/sqlite3.h>
#include <kernel/Filesystem/EXT2/EXT2.hh>
#include <kernel/Filesystem.hh>
#include <Support/sqlite/context.hh>
#include <drivers/Disk/Disk.hh>


typedef int(*sqlite3_exec_callback_fn)(void*, int, char**, char**);


const auto dbDirPath = std::string("DBs/");
//const auto dbDirPath = std::string("bin/");

const auto dbFileName = "DB.sqlite";
const std::string dbPath = /*dbDirPath + */dbFileName;
const auto dbContextPath = dbDirPath + "SQLContextDB.sqlite";


const size_t more_values = 10000;


	
TEST(SQLite)
{
	std::cout << "Opening a SQLite memory DB..." << std::endl;
	sqlite3* db;
	//const auto dbName = "Test.sqlite";
	const auto dbName = ":memory:";
	//const auto dbName = "";
	int rc = sqlite3_open(dbName, &db);
	std::cout << "Result: " << rc << std::endl;
	std::cout << sqlite3_errstr(rc) << std::endl;
	ASSERT(rc == SQLITE_OK);
	ASSERT(db != NULL);
	
	sqlite3_close(db);
	
	#ifndef SQLITE_VFS_NATIVE_FS
	auto db_fs = QA::MountTempFilesystem("/", "DBs", "images/SQLiteDisk.img",  90*1024*1024);
	assert(db_fs);
	QA::out << "Temp Filesystem Mounted." << std::endl;
	
	assert(QA::NativeFileMap("DB.sqlite", "DBs/NativeMapped2.sqlite", true));
	#endif
	
	auto test_file = Kernel::FS::Filesystem::Current->getNode("/DBs")->as_directory()->add_file("Test.txt");
	assert(test_file);
	const char* test_string = "Hello, world!";
	auto test_len = strlen(test_string);
	assert(test_file->write(0, test_len + 1, test_string) == test_len+1);
	char test_read[test_len+1];
	assert(test_file->read(0, test_len + 1, test_read) == test_len+1);
	assert(strcmp(test_read, test_string) == 0);
	
	/*{
		auto dir = Kernel::FS::Filesystem::Current->getNode("/DBs").as_directory();
		for (size_t i = 0; i < 600 && dir; ++i)
		{
			QA::out << "Directory depth: " << i << std::endl;
			dir = dir->add_directory("some_dir");
			assert(dir);
		}
	}*/
	
	
	{
		/*Support::SQLite::SQLiteContext context(dbFileName.c_str());
		QA::out << "Context created." << std::endl;*/
	}
	rc = sqlite3_open(dbPath.c_str(), &db);
	std::cout << "Result: " << rc << std::endl;
	std::cout << sqlite3_errstr(rc) << std::endl;
	
	
	ASSERT(rc == SQLITE_OK);
	ASSERT(db != NULL);
	
	auto null_callback = [](void*, auto, auto, auto) -> int
	{
		return 0;
	};
	
	QA::out << QA::br << "Creating table..." << QA::br << std::endl;
	int res = sqlite3_exec(db, "CREATE TABLE [XYZABC] (X INT, Y INT);", null_callback, nullptr, nullptr);
	if (!(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW))
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
	}
	//assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	QA::out << QA::br << "Inserting into table..." << std::endl;
	res = sqlite3_exec(db, "INSERT INTO [XYZABC] (X, Y) VALUES (1, 2);", null_callback, nullptr, nullptr);
	if (!(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW))
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
	}
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	QA::out << QA::br << "Inserting into table..." << std::endl;
	res = sqlite3_exec(db, "INSERT INTO [XYZABC] (X, Y) VALUES (3, 4), (5, 6);", null_callback, nullptr, nullptr);
	if (!(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW))
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
	}
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	QA::out << QA::br << "Selecting from table..." << std::endl;
	res = sqlite3_exec(db, "SELECT * FROM [XYZABC];", null_callback, nullptr, nullptr);
	if (!(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW))
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
	}
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	sqlite3_exec_callback_fn xyzabc_callbacks[] = {
		
		[](void*, int ncols, char** vals, char** cols) -> int
		{
			assert(ncols == 2);
			QA::out << QA::br;
			QA::out << "(" << cols[0] << ", " << cols[1] << ") = (" << vals[0] << ", " << vals[1] << ")" << std::endl;
			return 0;
		},
		
		
		[](void*, int ncols, char** vals, char** cols) -> int
		{
			assert(ncols == 2);
			QA::out << "(" << cols[0] << ", " << cols[1] << ") = (" << vals[0] << ", " << vals[1] << ")" << std::endl;
			return 0;
		},
		
		
		[](void*, int ncols, char** vals, char** cols) -> int
		{
			QA::out << "(";
			
			for (int i = 0; i < ncols; ++i)
			{
				if (i > 0)
				{
					QA::out << ", ";
				}
				QA::out << cols[i];
			}
			
			QA::out << ") = (";
			for (int i = 0; i < ncols; ++i)
			{
				if (i > 0)
				{
					QA::out << ", ";
				}
				QA::out << vals[i];
			}
			
			QA::out << ")" << std::endl;
			return 0;
		},
	};
	
	QA::out << QA::br << "Selecting from table..." << std::endl;
	res = sqlite3_exec(db, "SELECT * FROM [XYZABC];", xyzabc_callbacks[0], nullptr, nullptr);
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	
	
	QA::out << QA::br << "Closing DB..." << std::endl;
	sqlite3_close(db);
	
	QA::out << QA::br << "Opening DB..." << std::endl;
	rc = sqlite3_open(dbPath.c_str(), &db);
	std::cout << "Result: " << rc << std::endl;
	std::cout << sqlite3_errstr(rc) << std::endl;
	
	assert(rc == SQLITE_OK);
	assert(db != NULL);
	
	
	
	QA::out << QA::br << "Selecting from table..." << std::endl;
	res = sqlite3_exec(db, "SELECT * FROM [XYZABC];", xyzabc_callbacks[0], nullptr, nullptr);
	std::cout << sqlite3_errmsg(db) << std::endl;
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	
	
	QA::out << "Inserting more values..." << std::endl;
	res = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	for (size_t i = 0; i < more_values; ++i)
	{
		std::string q = "INSERT INTO [XYZABC] (X, Y) VALUES (";
		q += std::to_string(2*i + 4) + ", " + std::to_string(2*i + 1 + 4) + ")";
		
		res = sqlite3_exec(db, q.c_str(), null_callback, nullptr, nullptr);
		assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
		
	}
	res = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	auto exec_select = [db, xyzabc_callbacks](auto msg, auto query, int callback_index = 2)
	{
		QA::out << QA::br << msg << std::endl;
		auto res = sqlite3_exec(db, query, xyzabc_callbacks[callback_index], nullptr, nullptr);
		QA::out << sqlite3_errmsg(db) << std::endl;
		assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	};
	
	
	
	
	
	
	QA::out << QA::br << "Selecting from table..." << std::endl;
	res = sqlite3_exec(db, "SELECT * FROM [XYZABC] LIMIT 100;", xyzabc_callbacks[1], nullptr, nullptr);
	std::cout << sqlite3_errmsg(db) << std::endl;
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	
	QA::out << QA::br << "Closing DB..." << std::endl;
	sqlite3_close(db);
	
	using Support::SQLite::SQLiteContext;
	
	QA::out << "Opening sqlite context..." << std::endl;
	SQLiteContext* sqlcontext = new SQLiteContext(dbContextPath.c_str());
	QA::out << "Checking context for table..." << std::endl;
	assert(sqlcontext->table_exists("__Meta_Inheritance"));
	QA::out << "Deleting sqlite context..." << std::endl;
	delete sqlcontext;
	
	
	#ifndef SQLITE_VFS_NATIVE_FS
	
	bool umount_res = QA::UnmountTempFilesystem("/", "DBs", db_fs);
	assert(umount_res);
	
	#endif
}