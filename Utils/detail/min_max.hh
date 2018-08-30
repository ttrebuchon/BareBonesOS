#ifndef INCLUDED_UTILS_MIN_MAX_HH
#define INCLUDED_UTILS_MIN_MAX_HH


namespace Utils
{
	
	template <class T>
	constexpr const T& min(const T& lhs, const T& rhs)
	{
		return (lhs < rhs ? lhs : rhs);
	}
	
	template <class T, class Compare>
	constexpr const T& min(const T& lhs, const T& rhs, Compare cmp)
	{
		return (cmp(lhs, rhs) ? lhs : rhs);
	}
	
	
	template <class T>
	constexpr T min(std::initializer_list<T> ilist)
	{
		return *min_element(ilist.begin(), ilist.end());
	}
	
	template <class T, class Compare>
	constexpr T min(std::initializer_list<T> ilist, Compare cmp)
	{
		return *min_element(ilist.begin(), ilist.end(), cmp);
	}
	
	
	
	
	
	
	
	
	
	
	
	
	template <class T>
	constexpr const T& max(const T& lhs, const T& rhs)
	{
		return (lhs > rhs ? lhs : rhs);
	}
	
	template <class T, class Compare>
	constexpr const T& max(const T& lhs, const T& rhs, Compare cmp)
	{
		return (cmp(lhs, rhs) ? rhs : lhs);
	}
	
	
	template <class T>
	constexpr T max(std::initializer_list<T> ilist)
	{
		return *max_element(ilist.begin(), ilist.end());
	}
	
	template <class T, class Compare>
	constexpr T max(std::initializer_list<T> ilist, Compare cmp)
	{
		return *max_element(ilist.begin(), ilist.end(), cmp);
	}
	
}

#endif