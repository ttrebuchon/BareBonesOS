#ifndef INCLUDED_TESTS_HH
#define INCLUDED_TESTS_HH

#ifdef __cplusplus
#include <iostream>

using std::cerr;

#else

#include <stdio.h>

#endif

#define TEST(X) void Test_##X()
#ifdef __cplusplus
#define CTEST(X) extern "C" void TestC_##X ()
#else
#define CTEST(X) void TestC_##X ()
#endif




TEST(List);
CTEST(Bitset);
TEST(Bitset);


#endif