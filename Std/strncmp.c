
#include <Common.h>

int strncmp(const char* str1, const char* str2, size_t num)
{
	for (size_t i = 0; i < num; ++i)
	{
		if (str1[i] != str2[i])
		{
			return str1[i] - str2[i];
		}
		else if (str1[i] == 0)
		{
			return 0;
		}
	}
	
	return 0;
}