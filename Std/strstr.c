#include "string.h"

/*char* strstr(const char* hay, const char* needle)
{
	size_t hlen = strlen(hay);
	size_t nlen = strlen(needle);
	
	if (nlen > hlen)
	{
		return 0;
	}
	
	if (nlen == 0)
	{
		// Return nullptr or hay?
		return (char*)hay;
	}
	
	for (int i = 0; i <= hlen - nlen; ++i)
	{
		if (hay[i] == needle[0])
		{
			int j;
			for (j = 1; j < nlen; ++j)
			{
				if (hay[i+j] != needle[j])
				{
					break;
				}
			}
			if (j == nlen)
			{
				return (char*)&hay[i];
			}
			else
			{
				i += j - 1;
			}
		}
	}
	
	return 0;
}*/