#include <cassert>
#include "Tests.hh"
#include <kernel/utils/string>
//#include <string>

using namespace Utils;

TEST(String)
{
	string s("Testing!");
	string s2("Testing!");
	string s3("Test");
	
	
	assert(s == s2);
	assert(s3 != s);
	assert(s3 != s2);
	
	s3 = s2;
	assert(s2 == s);
	assert(s3 == s2);
	assert(s3 == s);
	
	std::cerr << s3.c_str() << std::endl;
	
	s += "  ";
	
	string s4 = s + s3;
	assert(s4 != "");
	assert(s4 == "Testing!  Testing!");
	
	s3 = "Test";
	assert(s3 == "Test");
	s4 += s3;
	
	assert(s4 == "Testing!  Testing!Test");
	
	s4 += '!';
	assert(s4 == "Testing!  Testing!Test!");
	std::cout << s4.c_str() << std::endl;
	assert(s4 == "Testing!  Testing!Test!");
	assert(s3 == "Test");
	assert(s2 == "Testing!");
	assert(s == "Testing!  ");
	
	static_assert(std::is_same<typename Utils::iterator_traits<int*>::iterator_category, Utils::random_access_iterator_tag>::value);
}