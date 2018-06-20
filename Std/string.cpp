#include "cstring"

extern "C" {

int memcmp(const void*  ptr1, const void* ptr2, size_t n)
{
	if (ptr1 == ptr2)
	{
		return 0;
	}
	
	const uint64_t* d64_1 = (const uint64_t*)ptr1;
	const uint64_t* d64_2 = (const uint64_t*)ptr2;
	
	for (; n >= sizeof(uint64_t); n -= sizeof(uint64_t))
	{
		if (*d64_1 != *d64_2)
		{
			if (*d64_1 < *d64_2)
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
		++d64_1;
		++d64_2;
	}
	
	
	
	
	const uint32_t* d32_1 = (const uint32_t*)d64_1;
	const uint32_t* d32_2 = (const uint32_t*)d64_2;
	
	for (; n >= sizeof(uint32_t); n -= sizeof(uint32_t))
	{
		if (*d32_1 != *d32_2)
		{
			if (*d32_1 < *d32_2)
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
		++d32_1;
		++d32_2;
	}
	
	const uint8_t* d8_1 = (const uint8_t*)d32_1;
	const uint8_t* d8_2 = (const uint8_t*)d32_2;
	
	while (n > 0)
	{
		if (*d8_1 != *d8_2)
		{
			if (*d8_1 < *d8_2)
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
		++d8_1;
		++d8_2;
		n -= sizeof(uint8_t);
	}
	
	return 0;
}




void* memmove(void* dest, const void* src, size_t num)
{
	unsigned char* cd = (unsigned char*)dest;
	const unsigned char* cs = (const unsigned char*)src;
	
	if (dest == src)
	{
	}
	else if (dest > src)
	{
		if (cd > cs + num)
		{
			memcpy(dest, src, num);
		}
		else
		{
			while (num != 0)
			{
				cd[num-1] = cs[num-1];
				--num;
			}
		}
	}
	else // if (dest < src) <- Implication
	{
		const size_t diff = cs - cd;
		if (cd + num < cs)
		{
			memcpy(dest, src, num);
		}
		else if (diff < sizeof(uint64_t))
		{
			for (size_t i = 0; i < num; ++i)
			{
				cd[i] = cs[i];
			}
		}
		else
		{
			memcpy(dest, src, num);
		}
	}
	
	return dest;
}



size_t strcspn(const char* str, const char* chars)
{
	// TODO
	assert(false);
}

char* strdup(const char* s1)
{
	auto len = strlen(s1) + 1;
	char* s2 = new char[len];
	strcpy(s2, s1);
	return s2;
}

}

char* strstr(const char* hay, const char* needle)
{
	auto hlen = strlen(hay);
	auto nlen = strlen(needle);
	
	if (nlen > hlen)
	{
		return nullptr;
	}
	
	if (nlen == 0)
	{
		// Return nullptr or hay?
		return (char*)hay;
	}
	
	for (size_t i = 0; i <= hlen - nlen; ++i)
	{
		if (hay[i] == needle[0])
		{
			size_t j;
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
	
	return nullptr;
}

char* strstr(char* hay, const char* needle)
{
	auto hlen = strlen(hay);
	auto nlen = strlen(needle);
	
	if (nlen > hlen)
	{
		return nullptr;
	}
	
	if (nlen == 0)
	{
		// Return nullptr or hay?
		return hay;
	}
	
	for (size_t i = 0; i <= hlen - nlen; ++i)
	{
		if (hay[i] == needle[0])
		{
			size_t j;
			for (j = 1; j < nlen; ++j)
			{
				if (hay[i+j] != needle[j])
				{
					break;
				}
			}
			if (j == nlen)
			{
				return &hay[i];
			}
			else
			{
				i += j - 1;
			}
		}
	}
	
	return nullptr;
}