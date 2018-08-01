#ifndef INCLUDED_FS_EXT2_HPP
#define INCLUDED_FS_EXT2_HPP

#include "EXT2.hh"

namespace Kernel::FS
{
	template <class Y>
	size_t EXT2::expand_indirect_1(Utils::shared_ptr<block_t> indir, Y& blocks, size_t start, size_t count, bool include_nulls)
	{
		assert(indir);
		if (unlikely(!indir))
		{
			return 0;
		}
		
		const uint32_t* ptrs = (const uint32_t*)indir->data;
		
		const auto pperblk = block_size()/sizeof(uint32_t);
		
		size_t i = start;
		size_t j = 0;
		
		while (i < pperblk && j < count)
		{
			if (ptrs[i] != 0 || include_nulls)
			{
				blocks.push_back(get_block(ptrs[i]));
				++j;
			}
			++i;
		}
		
		
		return j;
	}
	
	
	template <class Y>
	size_t EXT2::expand_indirect_2(Utils::shared_ptr<block_t> indir2, Y& blocks, size_t start, size_t count, bool include_nulls)
	{
		assert(indir2);
		if (unlikely(!indir2))
		{
			return 0;
		}
		
		const uint32_t* ptrs = (const uint32_t*)indir2->data;
		
		const auto pperblk = block_size()/sizeof(uint32_t);
		
		size_t i = start/pperblk;
		start %= pperblk;
		size_t j = 0;
		
		while (i < pperblk && j < count)
		{
			if (ptrs[i] == 0 && include_nulls)
			{
				for (size_t k = start; k < pperblk && k < count; ++k, ++j)
				{
					blocks.push_back(nullptr);
				}
			}
			else
			{
				j += expand_indirect_1(get_block(ptrs[i]), blocks, start, count-j, include_nulls);
			}
			start = 0;
			++i;
		}
		
		
		return j;
	}
	
	template <class Y>
	size_t EXT2::expand_indirect_3(Utils::shared_ptr<block_t> indir3, Y& blocks, size_t start, size_t count, bool include_nulls)
	{
		assert(indir3);
		if (unlikely(!indir3))
		{
			return 0;
		}
		
		const uint32_t* ptrs = (const uint32_t*)indir3->data;
		
		const auto pperblk = block_size()/sizeof(uint32_t);
		
		size_t i = start/(pperblk*pperblk);
		start %= (pperblk*pperblk);
		size_t j = 0;
		
		while (i < pperblk && j < count)
		{
			if (ptrs[i] == 0 && include_nulls)
			{
				for (size_t k = start; k < pperblk*pperblk && k < count; ++k, ++j)
				{
					blocks.push_back(nullptr);
				}
			}
			else
			{
				j += expand_indirect_2(get_block(ptrs[i]), blocks, start, count-j, include_nulls);
			}
			start = 0;
			++i;
		}
		
		
		return j;
	}
	
}

#endif