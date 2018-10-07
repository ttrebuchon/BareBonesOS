#include "Tests.hh"
#include <sqlite3/sqlite3.h>
#include <kernel/Filesystem/EXT2/EXT2.hh>
#include <kernel/Filesystem.hh>
#include <Support/sqlite/context.hh>
#include <drivers/Disk/Disk.hh>




const auto dbDirPath = std::string("DBs/");

const auto dbFileName = "DB.sqlite";
const auto dbPath = dbDirPath + dbFileName;
const auto dbContextPath = dbDirPath + "SQLContextDB.sqlite";

namespace Kernel::FS
{
	void test_node_ptr();
}
	
TEST(SQLite)
{
	Kernel::FS::test_node_ptr();
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
	auto db_fs = QA::MountTempFilesystem("/", "DBs", "images/SQLiteDisk.img", 8192*1024);
	assert(db_fs);
	#endif
	
	auto test_file = Kernel::FS::Filesystem::Current->getNode("/DBs")->as_directory()->add_file("Test.txt");
	assert(test_file);
	const char* test_string = "Hello, world!";
	auto test_len = strlen(test_string);
	assert(test_file->write(0, test_len + 1, test_string) == test_len+1);
	char test_read[test_len+1];
	assert(test_file->read(0, test_len + 1, test_read) == test_len+1);
	assert(strcmp(test_read, test_string) == 0);
	
	
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
	
	auto xyzabc_callback = [](void*, auto ncols, auto vals, auto cols) -> int
	{
		assert(ncols == 2);
		QA::out << QA::br << "(" << cols[0] << ", " << cols[1] << ") = (" << vals[0] << ", " << vals[1] << ")" << std::endl;
		
		return 0;
	};
	
	QA::out << QA::br << "Selecting from table..." << std::endl;
	res = sqlite3_exec(db, "SELECT * FROM [XYZABC];", xyzabc_callback, nullptr, nullptr);
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
	res = sqlite3_exec(db, "SELECT * FROM [XYZABC];", xyzabc_callback, nullptr, nullptr);
	std::cout << sqlite3_errmsg(db) << std::endl;
	assert(res == SQLITE_OK || res == SQLITE_DONE || res == SQLITE_ROW);
	
	
	QA::out << QA::br << "Closing DB..." << std::endl;
	sqlite3_close(db);
	assert(false);
	
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