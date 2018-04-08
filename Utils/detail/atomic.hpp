#ifndef INCLUDED_ATOMIC_HPP
#define INCLUDED_ATOMIC_HPP

#include "atomic.hh"

namespace Utils
{
	namespace detail
	{
		inline memory_order __calculate_memory_order(memory_order order)
		{
			const bool cond1 = order == memory_order_release;
			const bool cond2 = order == memory_order_acq_rel;
			memory_order o1(cond1 ? memory_order_relaxed : order);
			memory_order o2(cond2 ? memory_order_acquire : o1);
			return o2;
		}
		
		
		
		
		template <class T>
		constexpr atomic_base<T>::atomic_base() noexcept : _val()
		{
			
		}
		
		template <class T>
		constexpr atomic_base<T>::atomic_base(T desired) noexcept : _val(desired)
		{
			
		}
		
		template <class T>
		void atomic_base<T>::store(T desired, memory_order order) noexcept
		{
			if (order == memory_order_relaxed)
			{
				_val = desired;
			}
			else
			{
				// write_mem_barrier();
				_val = desired;
				if (order == memory_order_seq_cst)
				{
					__sync_synchronize();
				}
			}
		}
		
		template <class T>
		void atomic_base<T>::store(T desired, memory_order order) volatile noexcept
		{
			if (order == memory_order_relaxed)
			{
				_val = desired;
			}
			else
			{
				// write_mem_barrier();
				_val = desired;
				if (order == memory_order_seq_cst)
				{
					__sync_synchronize();
				}
			}
		}
		
		template <class T>
		T atomic_base<T>::load(memory_order order) const noexcept
		{
			__sync_synchronize();
			T v = _val;
			__sync_synchronize();
			return v;
		}
		
		template <class T>
		T atomic_base<T>::load(memory_order order) const volatile noexcept
		{
			__sync_synchronize();
			T v = _val;
			__sync_synchronize();
			return v;
		}
		
		template <class T>
		T atomic_base<T>::exchange(T desired, memory_order order) noexcept
		{
			return __sync_lock_test_and_set(&_val, desired);
		}
		
		template <class T>
		bool atomic_base<T>::compare_exchange_weak(T& expected, T desired, memory_order success, memory_order failure) noexcept
		{
			return compare_exchange_strong(expected, desired, success, failure);
		}
		
		template <class T>
		bool atomic_base<T>::compare_exchange_weak(T& expected, T desired, memory_order success, memory_order failure) volatile noexcept
		{
			return compare_exchange_strong(expected, desired, success, failure);
		}
		
		template <class T>
		bool atomic_base<T>::compare_exchange_weak(T& expected, T desired, memory_order order) noexcept
		{
			return compare_exchange_weak(expected, desired, order, __calculate_memory_order(order));
		}
		
		template <class T>
		bool atomic_base<T>::compare_exchange_weak(T& expected, T desired, memory_order order) volatile noexcept
		{
			return compare_exchange_weak(expected, desired, order, __calculate_memory_order(order));
		}
		
		
		template <class T>
		bool atomic_base<T>::compare_exchange_strong(T& expected, T desired, memory_order success, memory_order failure) noexcept
		{
			ASSERT(failure != memory_order_release);
			ASSERT(failure != memory_order_acq_rel);
			// ASSERT(failure <= success);
			
			T eold = expected;
			T enew = __sync_val_compare_and_swap(&_val, eold, desired);
			expected = enew;
			return eold == enew;
		}
		
		template <class T>
		bool atomic_base<T>::compare_exchange_strong(T& expected, T desired, memory_order success, memory_order failure) volatile noexcept
		{
			ASSERT(failure != memory_order_release);
			ASSERT(failure != memory_order_acq_rel);
			// ASSERT(failure <= success);
			
			T eold = expected;
			T enew = __sync_val_compare_and_swap(&_val, eold, desired);
			expected = enew;
			return eold == enew;
		}
		
		template <class T>
		bool atomic_base<T>::compare_exchange_strong(T& expected, T desired, memory_order order) noexcept
		{
			return compare_exchange_strong(expected, desired, order, __calculate_memory_order(order));
		}
		
		template <class T>
		bool atomic_base<T>::compare_exchange_strong(T& expected, T desired, memory_order order) volatile noexcept
		{
			return compare_exchange_strong(expected, desired, order, __calculate_memory_order(order));
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		// atomic_arithmetic<T, Y>
		
		template <class T, class Y>
		T atomic_arithmetic<T, Y, true>::fetch_add(Y arg, memory_order order) noexcept
		{
			return __sync_fetch_and_add(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T, class Y>
		T atomic_arithmetic<T, Y, true>::fetch_add(Y arg, memory_order order) volatile noexcept
		{
			return __sync_fetch_and_add(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T, class Y>
		T atomic_arithmetic<T, Y, true>::fetch_sub(Y arg, memory_order order) noexcept
		{
			return __sync_fetch_and_sub(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T, class Y>
		T atomic_arithmetic<T, Y, true>::fetch_sub(Y arg, memory_order order) volatile noexcept
		{
			return __sync_fetch_and_sub(&((atomic_base<T>*)this)->_val, arg);
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		// atomic_integral<T>
		
		template <class T>
		T atomic_integral<T, true>::operator++() noexcept
		{
			return this->fetch_add(1) + 1;
		}
		
		template <class T>
		T atomic_integral<T, true>::operator++() volatile noexcept
		{
			return this->fetch_add(1) + 1;
		}
		
		template <class T>
		T atomic_integral<T, true>::operator--() noexcept
		{
			return this->fetch_sub(1) - 1;
		}
		
		template <class T>
		T atomic_integral<T, true>::operator--() volatile noexcept
		{
			return this->fetch_sub(1) - 1;
		}
		
		template <class T>
		T atomic_integral<T, true>::operator++(int) noexcept
		{
			return this->fetch_add(1);
		}
		
		template <class T>
		T atomic_integral<T, true>::operator++(int) volatile noexcept
		{
			return this->fetch_add(1);
		}
		
		template <class T>
		T atomic_integral<T, true>::operator--(int) noexcept
		{
			return this->fetch_sub(1);
		}
		
		template <class T>
		T atomic_integral<T, true>::operator--(int) volatile noexcept
		{
			return this->fetch_sub(1);
		}
		
		
		
		template <class T>
		T atomic_integral<T, true>::fetch_and(T arg, memory_order order) noexcept
		{
			return __sync_fetch_and_and(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T>
		T atomic_integral<T, true>::fetch_and(T arg, memory_order order) volatile noexcept
		{
			return __sync_fetch_and_and(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T>
		T atomic_integral<T, true>::fetch_or(T arg, memory_order order) noexcept
		{
			return __sync_fetch_and_or(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T>
		T atomic_integral<T, true>::fetch_or(T arg, memory_order order) volatile noexcept
		{
			return __sync_fetch_and_or(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T>
		T atomic_integral<T, true>::fetch_xor(T arg, memory_order order) noexcept
		{
			return __sync_fetch_and_xor(&((atomic_base<T>*)this)->_val, arg);
		}
		
		template <class T>
		T atomic_integral<T, true>::fetch_xor(T arg, memory_order order) volatile noexcept
		{
			return __sync_fetch_and_xor(&((atomic_base<T>*)this)->_val, arg);
		}
		
	}
	
}
#endif