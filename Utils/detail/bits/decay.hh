#ifndef INCLUDED_BITS_DECAY_HH
#define INCLUDED_BITS_DECAY_HH

namespace Utils
{
	template <class T>
	struct decay
	{
		typedef T type;
	};
	
	template <class T>
	struct decay<T&> : public decay<T>
	{};
	
	template <class T>
	struct decay<T&&> : public decay<T>
	{};
	
	template <class T>
	struct decay<const T> : public decay<T>
	{};
	
	template <class T>
	struct decay<T[]> : public decay<T*>
	{};
	
	
	
	
	
	
	
	
	template <class T>
	using decay_t = typename decay<T>::type;
	
	
	namespace detail
	{
		template <class T, class T2 = typename decay<T>::type>
		struct can_decay;
		
		template <class T>
		struct can_decay<T, T> : false_type
		{};
		
		template <class T, class T2>
		struct can_decay : true_type
		{};
		
		template <class T>
		constexpr bool can_decay_v = can_decay<T>::value;
		
		
		static_assert(!can_decay_v<int>);
		static_assert(can_decay_v<int&>);
	}
}

#endif