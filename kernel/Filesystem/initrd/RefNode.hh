#ifndef INCLUDED_FILESYSTEM_INITRD_REFNODE_HH
#define INCLUDED_FILESYSTEM_INITRD_REFNODE_HH

#include <Common.h>


namespace Kernel { namespace FS {
namespace Init_RD {
	
	class RefNode
	{
		public:
		typedef unsigned long count_type;
		
		
		private:
		count_type _ref_count;
		
		protected:
		
		constexpr RefNode() : _ref_count(0)
		{
			
		}
		
		public:
		
		virtual ~RefNode() = default;
		
		constexpr void new_ref()
		{
			++_ref_count;
		}
		
		constexpr count_type ref_count() const noexcept
		{
			return _ref_count;
		}
		
		constexpr bool remove_ref() noexcept
		{
			if (ref_count() > 0)
			{
				return (--_ref_count == 0);
			}
			else
			{
				return true;
			}
		}
		
		public:
		
	};
}
}
}

#endif