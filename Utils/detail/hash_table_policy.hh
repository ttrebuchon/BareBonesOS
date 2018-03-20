#ifndef INCLUDED_HASH_TABLE_POLICY_HH
#define INCLUDED_HASH_TABLE_POLICY_HH

#include <Common.h>
#include <Utils/pair>
#include <Utils/tuple>

namespace Utils
{
	namespace detail
	{
		struct hash_table_policy
		{
			protected:
			mutable size_t nextResize;
			mutable size_t prevResize;
			float maxLoad;
			
			static float GROWTH_FACTOR;
			
			public:
			
			pair<bool, size_t> needRehash(const size_t bucketCount, const size_t elemCount, const size_t insertingCount) const;
			
			size_t nextBuckets(size_t n) const;
			
			hash_table_policy(float load = 1.0);
		};
	}
}

#endif