#ifndef INCLUDED_PAGEDIR_HPP
#define INCLUDED_PAGEDIR_HPP

#include "PageDir.hh"
#include <Utils/utility>

extern "C" void* kmemset(void*, int, size_t);


namespace Kernel { namespace Memory
{
	namespace detail
	{
		template <class Alloc, class = void>
		struct v2phys_alloc_helper
		{
			static void* call(const Alloc&, void* addr)
			{
				if (PageDirectory::Current)
				{
					return PageDirectory::Current->physical(addr);
				}
				
				return addr;
			}
		};
		
		template <class Alloc>
		struct v2phys_alloc_helper<Alloc, Utils::void_t<decltype(Utils::declval<Alloc>().physical((void*)nullptr))>>
		{
			static void* call(const Alloc& a, void* addr)
			{
				return a.physical(addr);
			}
		};
	}
	
	
	
	template <class _Alloc>
	PageDirectory::PageDirectory(const _Alloc& _alloc, size_t n) : PageDirectory()
	{
		dir_phys = detail::v2phys_alloc_helper<_Alloc>::call(_alloc, &dir);
		
		typedef typename _Alloc::template rebind<Table>::other Alloc;
		Alloc alloc(_alloc);
		
		
		const auto tableCount = n / 1024 + (n % 1024 != 0 ? 1 : 0);
		
		for (auto i = 0; i < tableCount; ++i)
		{
			tables[i] = new Table(dir->tables[i], alloc);
		}
	}
	
	
	template <class _Alloc>
	PageDirectory::Table::Table(_Table& t, const _Alloc& _alloc) : table(&t), _pages(nullptr), pages(), _pages_phys(nullptr)
	{
		typedef typename _Alloc::template rebind<_Pages>::other Alloc;
		Alloc alloc(_alloc);
		_pages = alloc.allocate(1);
		_pages_phys = detail::v2phys_alloc_helper<Alloc>::call(alloc, _pages);
		kmemset(_pages, 0, sizeof(_Pages));
		
		
		for (auto i = 0; i < 1024; ++i)
		{
			pages[i].page = &_pages->pages[i];
		}
		
		table->frame = (((addr_t)_pages_phys) >> 12);
		table->rw = 1;
		table->user = 1;
		table->present = 1;
	}
	
}
}


#endif