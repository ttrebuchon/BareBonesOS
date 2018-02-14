#include "memcpy.h"

void* memcpy(void* dest, const void* src, size_t n)
{
	return kmemcpy(dest, src, n);
}

void* kmemcpy(void* dest, const void* src, size_t n)
{
	uint64_t* d64 = (uint64_t*)dest;
	const uint64_t* s64 = (const uint64_t*)src;
	
	for (; n >= sizeof(uint64_t); n -= sizeof(uint64_t))
	{
		*d64 = *s64;
		++d64;
		++s64;
	}
	
	
	
	
	uint32_t* d32 = (uint32_t*)d64;
	const uint32_t* s32 = (const uint32_t*)s64;
	
	for (; n >= sizeof(uint32_t); n -= sizeof(uint32_t))
	{
		*d32 = *s32;
		++d32;
		++s32;
	}
	
	uint8_t* d8 = (uint8_t*)d32;
	const uint8_t* s8 = (const uint8_t*)s32;
	
	while (n > 0)
	{
		*d8 = *s8;
		++d8;
		++s8;
		n -= sizeof(uint8_t);
	}
	
	return dest;
	
}