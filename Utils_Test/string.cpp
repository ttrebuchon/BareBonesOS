#include <cassert>
#include "Tests.hh"
#include <Utils/string>
#include <string>

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
	
	{
		char cstr[] = "Hello";
		char scstr[] = "Hello";
		
		// Sanity checks and all that
		static_assert(sizeof(cstr) == 6);
		static_assert(sizeof(scstr) == 6);
		
		
		
		string st(cstr);
		std::string sst(scstr);
		
		ASSERT(st == "Hello");
		ASSERT(sst == "Hello");
		
		cstr[0] = scstr[0] = 'G';
		ASSERT(st == "Hello");
		ASSERT(sst == "Hello");
		
		
		
	}
	
	
	
	{
		
		
		string s1("Hello");
		
		ASSERTEQ(Utils::Char_Traits<char>::find(s1.c_str(), 5, 'H'), s1.c_str());
		
		
		
		ASSERTEQ(s1.find("H"), 0);
		ASSERTEQ(s1.find("He"), 0);
		ASSERTEQ(s1.find("Hel"), 0);
		ASSERTEQ(s1.find("Hell"), 0);
		ASSERTEQ(s1.find("Hello"), 0);
		
		
		ASSERTEQ(s1.find("a"), string::npos);
		
		ASSERTEQ(s1.find("e"), 1);
		ASSERTEQ(s1.find("el"), 1);
		ASSERTEQ(s1.find("ell"), 1);
		ASSERTEQ(s1.find("ello"), 1);
		
		ASSERTEQ(s1.find("l"), 2);
		ASSERTEQ(s1.find("ll"), 2);
		ASSERTEQ(s1.find("llo"), 2);
		
		ASSERTEQ(s1.find("l", 3), 3);
		ASSERTEQ(s1.find("lo"), 3);
		
		ASSERTEQ(s1.find("o"), 4);
	}
	
	
	
	static_assert(std::is_same<typename Utils::iterator_traits<int*>::iterator_category, Utils::random_access_iterator_tag>::value);
}