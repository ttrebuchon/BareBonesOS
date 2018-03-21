#include "hash_table_policy.hh"
#include "bits/primes_list.hh"
#include <Utils/algorithm>

namespace Utils
{
namespace detail
{
	
	float hash_table_policy::GROWTH_FACTOR = 2;
	hash_table_policy::hash_table_policy(float load) : nextResize(0), prevResize(0), maxLoad(load)
	{
		
	}
	
	pair<bool, size_t> hash_table_policy::needRehash(const size_t bucketCount, const size_t elemCount, const size_t insertingCount) const
	{
		pair<bool, size_t> result(false, 0);
		if (elemCount + insertingCount < nextResize)
		{
			return result;
		}
		
		long double minBuckets = (elemCount + insertingCount)/(long double)maxLoad;
		
		if (minBuckets >= bucketCount)
		{
			result.first = true;
			if ((size_t)minBuckets + 1 >= bucketCount*GROWTH_FACTOR)
			{
				result.second = (size_t)minBuckets + 1;
			}
			else
			{
				result.second = bucketCount*GROWTH_FACTOR;
			}
			result.second = nextBuckets(result.second);
		}
		else
		{
			nextResize = bucketCount * (long double)maxLoad;
		}
		
		return result;
	}
	
	
	static constexpr unsigned char firstPrimes[] = { 2, 2, 3, 5, 5, 7, 7, 11, 11, 11, 11, 13, 13 };
	
	size_t hash_table_policy::nextBuckets(const size_t n) const
	{
		static_assert(firstPrimes[0] == 2);
		static_assert(firstPrimes[12] == 13);
		if (n < 13)
		{
			nextResize = firstPrimes[n]*(long double)maxLoad + 1;
			return firstPrimes[n];
		}
		
		
		constexpr auto nPrimes = sizeof(_primes_list)/sizeof(unsigned long) - 1;
		
		constexpr auto lastPrime = _primes_list + nPrimes - 1;
		
		// 6 = Number of primes used
		// in firstPrimes[]
		const unsigned long* next = lower_bound(_primes_list + 6, lastPrime, n + 1);
		
		if (next == lastPrime)
		{
			nextResize = (size_t)-1;
		}
		else
		{
			nextResize = *next * (long double)maxLoad + 1;
		}
		
		return *next;
	}
	
}
}