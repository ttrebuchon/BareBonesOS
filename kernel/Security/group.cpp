#include "group.h"
#include <Std/errno.h>
#include <Utils/set>

namespace Kernel
{
	typedef Utils::set<gid_t> gset_t;
	
	#define NNULL(...) __attribute__((nonnull(__VA_ARGS__)))
	
	extern "C" {
		
		int NNULL(1) groupset_init(groupset_t* groups) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t provided to groupset_init");
				return EINVAL;
			}
			
			groups->data = new gset_t();
			
			return 0;
		}
		
		int NNULL(1) groupset_destroy(groupset_t* groups) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t provided to groupset_init");
				return EINVAL;
			}
			
			auto set = (gset_t*)groups->data;
			if (!set)
			{
				tset_error(EINVAL, "Uninitialized groupset_t provided to groupset_destroy");
				return EINVAL;
			}
			
			delete set;
			groups->data = nullptr;
			return 0;
		}
		
		
		gid_t NNULL(1) get_groupset_id(const groupset_t* groups, uint32_t index) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t provided to get_groupset_id");
				return NOGROUP;
			}
			
			gid_t id;
			if (groupset_get_groups_from(groups, 1, &id, index) != 1)
			{
				return NOGROUP;
			}
			else
			{
				return id;
			}
			
			
			
			/*auto set = (const gset_t*)groups->data;
			if (!set)
			{
				tset_error(EINVAL, "Uninitialized groupset_t provided to get_groupset_id");
				return NOGROUP;
			}
			
			if (set->size() > index)
			{
				return set->at(index);
			}
			
			return NOGROUP;*/
		}
		
		int NNULL(1) groupset_has_id(const groupset_t* groups, gid_t id) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t provided to groupset_has_id");
				return 0;
			}
			
			auto set = (const gset_t*)groups->data;
			if (!set)
			{
				tset_error(EINVAL, "Uninitialized groupset_t provided to groupset_has_id");
				return 0;
			}
			
			if (set->count(id) > 0)
			{
				goto found;
			}
			
			
			return 0;
			
			
			
			found:
			return 1;
		}
		
		
		
		int groupset_add_id(groupset_t* groups, gid_t id) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t provided to groupset_add_id");
				return EINVAL;
			}
			
			if (id == NOGROUP)
			{
				tset_error(EINVAL, "Can not add NOGROUP to groupset_t");
				return EINVAL;
			}
			
			auto set = (gset_t*)groups->data;
			if (!set)
			{
				tset_error(EINVAL, "Uninitialized groupset_t provided to groupset_has_id");
				return EINVAL;
			}
			
			set->insert(id);
			
			return 0;
		}
		
		int groupset_get_groups(const groupset_t* groups, int count, gid_t* buf) _NOEX
		{
			return groupset_get_groups_from(groups, count, buf, 0);
		}
		
		int groupset_get_groups_from(const groupset_t* groups, int count, gid_t* buf, int start_index) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t provided to groupset_add_id");
				return EINVAL;
			}
			
			auto set = (const gset_t*)groups->data;
			if (!set)
			{
				tset_error(EINVAL, "Uninitialized groupset_t provided to groupset_get_groups_from");
				return -1;
			}
			
			
			if (count == 0)
			{
				return set->size();
			}
			
			if (start_index >= set->size())
			{
				tset_error(EINVAL, "Provided start_index is larger than the total number of groups.");
				return -1;
			}
			
			/*if (set->size() - start_index > count)
			{
				tset_error(EINVAL, "Provided gid_t[] buffer is of insufficient size.");
				return -1;
			}*/
			
			size_t i;
			auto it = set->begin();
			
			
			for (i = 0; i < start_index; ++i, ++it)
			{}
			
			for (i = 0; i < count && it != set->end(); ++it, ++i)
			{
				buf[i] = *it;
			}
			
			return i;
		}
		
		size_t __attribute__((nonnull(1))) groupset_count(const groupset_t* groups) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t provided to groupset_count");
				return 0;
			}
			
			auto set = (const gset_t*)groups->data;
			if (!set)
			{
				tset_error(EINVAL, "Uninitialized groupset_t provided to groupset_count");
				return 0;
			}
			
			return set->size();
		}
		
		 int NNULL(1, 2) groupset_dup(groupset_t* target, const groupset_t* groups) _NOEX
		{
			assert(groups);
			if (!groups)
			{
				tset_error(EINVAL, "NULL groupset_t source provided to groupset_copy");
				return EINVAL;
			}
			
			assert(target);
			if (!target)
			{
				tset_error(EINVAL, "NULL groupset_t target provided to groupset_copy");
				return EINVAL;
			}
			
			auto set = (const gset_t*)groups->data;
			if (!set)
			{
				tset_error(EINVAL, "Uninitialized groupset_t source provided to groupset_copy");
				return EINVAL;
			}
			
			if (target->data)
			{
				int res = groupset_destroy(target);
				if (res != 0)
				{
					return res;
				}
			}
			
			
			auto nset = new gset_t(*set);
			assert(nset);
			
			target->data = nset;
			
			return 0;
		}
	}
	
	
}