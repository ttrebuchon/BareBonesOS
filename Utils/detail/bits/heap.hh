#ifndef INCLUDED_BITS_HEAP_HH
#define INCLUDED_BITS_HEAP_HH

namespace Utils
{
	template <class It>
	void make_heap(It first, It last);
	template <class It, class Compare>
	void make_heap(It first, It last, Compare);
	
	template <class It>
	void push_heap(It first, It last);
	template <class It, class Compare>
	void push_heap(It first, It last, Compare);
	
	template <class It>
	void pop_heap(It first, It last);
	template <class It, class Compare>
	void pop_heap(It first, It last, Compare);
	
	
	
	
	template <class It>
	void make_heap(It first, It last)
	{
		Utils::make_heap(first, last, [](const auto& lhs, const auto& rhs) -> bool
		{
			return lhs < rhs;
		});
	}
	
	template <class It>
	void push_heap(It first, It last)
	{
		Utils::push_heap(first, last, [](const auto& lhs, const auto& rhs) -> bool
		{
			return lhs < rhs;
		});
	}
	
	template <class It>
	void pop_heap(It first, It last)
	{
		Utils::pop_heap(first, last, [](const auto& lhs, const auto& rhs) -> bool
		{
			return lhs < rhs;
		});
	}
	
	
	
	
	namespace detail
	{
		template <class It, class Compare>
		void __push_heap_back(It first, It last, const Compare& c, typename iterator_traits<It>::difference_type len)
		{
			typedef typename iterator_traits<It>::difference_type difference_type;
			typedef typename iterator_traits<It>::value_type value_type;
			
			if (len > 1)
			{
				len = (len - 2)/2;
				It i = first + len;
				if (c(*i, *--last))
				{
					value_type t(Utils::move(*last));
					do
					{
						*last = Utils::move(*i);
						last = i;
						if (len == 0)
						{
							break;
						}
						len = (len - 1)/2;
						i = first + len;
					}
					while (c(*i, t));
					*last = Utils::move(t);
				}
			}
		}
		
		
		template <class It, class Compare>
		void __push_heap_front(It first, It last, const Compare& comp, typename iterator_traits<It>::difference_type len)
		{
			typedef typename iterator_traits<It>::difference_type difference_type;
			typedef typename iterator_traits<It>::value_type value_type;
			
			if (len > 1)
			{
				difference_type p = 0;
				It it = first;
				difference_type c = 2;
				It cit = first + c;
				if (c == len || comp(*cit, *(cit - 1)))
				{
					--c;
					--cit;
				}
				
				if (comp(*it, *cit))
				{
					value_type t(Utils::move(*it));
					do
					{
						*it = Utils::move(*cit);
						it = cit;
						p = c;
						c = (p+1)*2;
						if (c > len)
						{
							break;
						}
						cit = first + c;
						if (c == len || comp(*cit, *(cit - 1)))
						{
							--c;
							--cit;
						}
					}
					while (comp(t, *cit));
					*it = Utils::move(t);
				}
			}
		}
		
		
		
		template <class It, class Compare>
		void __pop_heap(It first, It last, const Compare& c, typename iterator_traits<It>::difference_type len)
		{
			typedef typename iterator_traits<It>::value_type value_type;
			if (len > 1)
			{
				value_type t(Utils::move(*first));
				*first = Utils::move(*--last);
				*last = Utils::move(t);
				//swap(*first, *--last);
				__push_heap_front(first, last, c, len-1);
			}
		}
		
	}
	
	
	
	
	template <class It, class Compare>
	void push_heap(It first, It last, Compare c)
	{
		detail::__push_heap_back(first, last, c, last - first);
	}
	
	
	template <class It, class Compare>
	void pop_heap(It first, It last, Compare c)
	{
		detail::__pop_heap(first, last, c, last - first);
	}
	
	
	
	
	template <class It, class Compare>
	void make_heap(It first, It last, Compare c)
	{
		typedef typename iterator_traits<It>::difference_type difference_type;
		difference_type n = last - first;
		
		if (n > 1)
		{
			last = first;
			++last;
			for (difference_type i = 1; i < n;)
			{
				detail::__push_heap_back(first, ++last, c, ++i);
			}
		}
	}
}

#endif