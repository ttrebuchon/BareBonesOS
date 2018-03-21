#include "Tests.hh"
#include <sqlite3/sqlite3.h>

TEST(SQLite)
{
	sqlite3* db;
	sqlite3_open(":memory:", &db);
	
	
}