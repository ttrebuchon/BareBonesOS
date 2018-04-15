#include "stdlib.h"
#include "string.h"

C_CODE

long int strtol(const char* str, char** endptr, int base)
{
	if (!str)
	{
		return 0L;
	}
	
	auto len = strlen(str);
	bool pos = true;
	if (len > 0)
	{
		if (str[0] == '-')
		{
			pos = false;
			str += 1;
			--len;
		}
		else if (str[0] == '+')
		{
			str += 1;
			--len;
		}
	}
	
	if (base == 0)
	{
		if (len > 0)
		{
			if (str[0] == '0')
			{
				if (len > 1 && (str[1] == 'x' || str[1] == 'X'))
				{
					base = 16;
					str += 2;
					len -= 2;
				}
				else
				{
					base = 8;
					str += 1;
					--len;
				}
			}
		}
	}
	
	if (base < 2 || base > 36 || len == 0)
	{
		return 0L;
	}
	
	long int n = 0;
	int val;
	
	do
	{
		char c = str[0];
		if (c < '0')
		{
			break;
		}
		else if (c - '0' < 10)
		{
			c -= '0';
		}
		else if (c < 'A')
		{
			break;
		}
		else if (c <= 'Z')
		{
			c -= 'A' + 10;
		}
		else if (c < 'a')
		{
			break;
		}
		else if (c <= 'z')
		{
			c -= 'a' + 10;
		}
		else
		{
			break;
		}
		
		assert(c >= 0);
		assert(c < 36);
		
		if (c >= base)
		{
			break;
		}
		
		n *= base;
		n += c;
		str = str + 1;
	}
	while (true);
	
	
	
	if (!pos)
	{
		n *= -1;
	}
	
	if (endptr)
	{
		*endptr = const_cast<char*>(str);
	}
	
	return n;
}

__attribute__((noreturn))
void abort()
{
	// TODO
	KPANIC("abort()");
}

[[ noreturn ]]
void exit(int code)
{
	// TODO
	assert(false);
}

C_END