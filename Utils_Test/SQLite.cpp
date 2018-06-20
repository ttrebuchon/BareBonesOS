#ifdef TESTING
#include "Tests.hh"
#include <sqlite3/sqlite3.h>

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
	assert(rc == SQLITE_OK);
	assert(db != NULL);
	
	sqlite3_close(db);
}
#endif