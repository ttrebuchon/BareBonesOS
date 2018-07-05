#include "choice_allocator.hh"

namespace Kernel::Memory
{
	namespace detail
	{
		int alloc_spec_cmp::operator()(const alloc_spec_t& lhs, const alloc_spec_t& rhs) const noexcept
		{
			if (lhs.size > rhs.size)
			{
				return 1;
			}
			else if (lhs.size < rhs.size)
			{
				return -1;
			}
			else
			{
				if (lhs.align > rhs.align)
				{
					return 1;
				}
				else if (lhs.align < rhs.align)
				{
					return -1;
				}
				else
				{
					return 0;
				}
			}
		}
	}
}