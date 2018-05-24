#ifndef INCLUDED_ATOMIC_HH
#define INCLUDED_ATOMIC_HH

#include "type_traits.hh"
#include "enable_if.hh"

namespace Utils
{
	#define MEM_ORD_APP(X) memory_order_##X
	
	#if __cplusplus >= 201703L
	#define MEM_ORD_DEF(X) X
	enum class memory_order
	#else
	#define MEM_ORD_DEF(X) MEM_ORD_APP(X)
	enum memory_order
	#endif
	{
		MEM_ORD_DEF(relaxed),
		MEM_ORD_DEF(consume),
		MEM_ORD_DEF(acquire),
		MEM_ORD_DEF(release),
		MEM_ORD_DEF(acq_rel),
		MEM_ORD_DEF(seq_cst),
	};
	
	#undef MEM_ORD_DEF
	
	
	#if __cplusplus >= 201703L
	#define VAR_DEF(X) \
	inline constexpr memory_order MEM_ORD_APP(X) = memory_order:: X
	
	VAR_DEF(relaxed);
	VAR_DEF(consume);
	VAR_DEF(acquire);
	VAR_DEF(release);
	VAR_DEF(acq_rel);
	VAR_DEF(seq_cst);
		
	#undef VAR_DEF
	#endif
	
	#undef MEM_ORDER_APP
	
	
	
	
	namespace detail
	{
	
	template <class T>
	struct atomic_base
	{
		private:
		T _val;
		
		public:
		constexpr atomic_base() noexcept;
		constexpr atomic_base(T desired) noexcept;
		atomic_base(const atomic_base&) = delete;
		
		void store(T desired, memory_order = memory_order_seq_cst) noexcept;
		void store(T desired, memory_order = memory_order_seq_cst) volatile noexcept;
		
		T load(memory_order = memory_order_seq_cst) const noexcept;
		T load(memory_order = memory_order_seq_cst) const volatile noexcept;
		
		T exchange(T desired, memory_order = memory_order_seq_cst) noexcept;
		T exchange(T desired, memory_order = memory_order_seq_cst) volatile noexcept;
		
		inline operator T() const noexcept
		{
			return load();
		}
		inline operator T() const volatile noexcept
		{
			return load();
		}
		
		bool compare_exchange_weak(T& expected, T desired, memory_order success, memory_order failure) noexcept;
		bool compare_exchange_weak(T& expected, T desired, memory_order success, memory_order failure) volatile noexcept;
		bool compare_exchange_weak(T& expected, T desired, memory_order order = memory_order_seq_cst) noexcept;
		bool compare_exchange_weak(T& expected, T desired, memory_order order = memory_order_seq_cst) volatile noexcept;
		
		bool compare_exchange_strong(T& expected, T desired, memory_order success, memory_order failure) noexcept;
		bool compare_exchange_strong(T& expected, T desired, memory_order success, memory_order failure) volatile noexcept;
		bool compare_exchange_strong(T& expected, T desired, memory_order order = memory_order_seq_cst) noexcept;
		bool compare_exchange_strong(T& expected, T desired, memory_order order = memory_order_seq_cst) volatile noexcept;
		
		
		template <class, class, bool>
		friend class atomic_arithmetic;
	};
	
	
	template <class, class, bool>
	struct atomic_arithmetic;
	
	template <class T, class G>
	struct atomic_arithmetic<T, G, false> { };
	
	template <class T, class G>
	struct atomic_arithmetic<T, G, true>
	{
		constexpr atomic_arithmetic() noexcept = default;
		T fetch_add(G arg, memory_order = memory_order_seq_cst) noexcept;
		T fetch_add(G arg, memory_order = memory_order_seq_cst) volatile noexcept;
		T fetch_sub(G arg, memory_order = memory_order_seq_cst) noexcept;
		T fetch_sub(G arg, memory_order = memory_order_seq_cst) volatile noexcept;
	};
	
	
	
	template <class, bool>
	struct atomic_integral;
	
	template <class T>
	struct atomic_integral<T, false> //: public atomic_arithmetic<T, T, is_arithmetic<T>::value> 
	{ };
	
	template <class T>
	struct atomic_integral<T, true> : public atomic_arithmetic<T, T, is_arithmetic<T>::value>
	{
		
		constexpr atomic_integral() noexcept = default;
		T operator++() noexcept;
		T operator++() volatile noexcept;
		T operator--() noexcept;
		T operator--() volatile noexcept;
		T operator++(int) noexcept;
		T operator++(int) volatile noexcept;
		T operator--(int) noexcept;
		T operator--(int) volatile noexcept;
		
		
		T fetch_and(T arg, memory_order = memory_order_seq_cst) noexcept;
		T fetch_and(T arg, memory_order = memory_order_seq_cst) volatile noexcept;
		T fetch_or(T arg, memory_order = memory_order_seq_cst) noexcept;
		T fetch_or(T arg, memory_order = memory_order_seq_cst) volatile noexcept;
		T fetch_xor(T arg, memory_order = memory_order_seq_cst) noexcept;
		T fetch_xor(T arg, memory_order = memory_order_seq_cst) volatile noexcept;
	};
	
	template <class T>
	struct atomic_integral<T*, false> : public atomic_arithmetic<T*, ptrdiff_t, true>
	{
		
		constexpr atomic_integral() noexcept = default;
		T* operator++() noexcept;
		T* operator++() volatile noexcept;
		T* operator--() noexcept;
		T* operator--() volatile noexcept;
		T* operator++(int) noexcept;
		T* operator++(int) volatile noexcept;
		T* operator--(int) noexcept;
		T* operator--(int) volatile noexcept;
	};
	
	}
	
	template <class T>
	struct atomic : 
		public detail::atomic_base<T>,
		public detail::atomic_integral<T, is_integral<T>::value>
	{
		private:
		typedef detail::atomic_base<T> _base;
		typedef detail::atomic_integral<T, is_integral<T>::value> _int_base;
		
		public:
		
		constexpr atomic() noexcept {} //= default;
		constexpr atomic(T desired) noexcept : _base(desired), _int_base() { }
		atomic(const atomic&) = delete;
	};
	
	
	
	
	static_assert(is_integral<int>::value);
	static_assert(is_integral<bool>::value);
	static_assert(is_integral<long>::value);
	
	typedef atomic<int> atomic_int;
	typedef atomic<bool> atomic_bool;
	typedef atomic<long> atomic_long;
	typedef atomic<float> atomic_float;
	
}

#endif