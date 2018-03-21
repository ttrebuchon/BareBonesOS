#ifndef INCLUDED_VECTOR_HPP
#define INCLUDED_VECTOR_HPP

#include "vector.hh"

namespace Utils
{
	namespace detail
	{
		template <class T, class = void>
		struct vector_realloc_helper
		{
			constexpr static bool moved = false;
			
			inline static void call(T* dest, T* src)
			{
				new (dest) T(*src);
			}
		};
		
		template <class T>
		struct vector_realloc_helper<T, void_t<decltype(T(move(declval<T>())))>>
		{
			constexpr static bool moved = true;
			inline static void call(T* dest, T* src)
			{
				new (dest) T(move(*src));
			}
		};
	}
	
	
	template <class T, class A>
	constexpr vector<T, A>::vector() : _data(nullptr), _size(0), _cap(0), alloc()
	{
		
	}
	
	template <class T, class A>
	constexpr vector<T, A>::vector(const allocator_type& alloc) : _data(nullptr), _size(0), _cap(0), alloc(alloc)
	{
		
	}
	
	template <class T, class A>
	vector<T, A>::~vector()
	{
		if (_data)
		{
			for (size_type i = 0; i < _size; ++i)
			{
				alloc.destroy(_data + i);
			}
			ATraits::deallocate(alloc, _data, _cap);
			_data = nullptr;
			_size = _cap = 0;
		}
	}
	
	
	
	
	template <class T, class A>
	void vector<T, A>::ensure_capacity(size_type n)
	{
		if (_cap < n)
		{
			if (_cap > 0)
			{
				reallocate(_cap*2);
			}
			else
			{
				reallocate(2);
			}
		}
	}
	
	template <class T, class A>
	void vector<T, A>::reallocate(size_type n)
	{
		auto nData = static_cast<T*>(ATraits::allocate(alloc, n));
		for (size_type i = 0; i < _size; ++i)
		{
			detail::vector_realloc_helper<T>::call(nData + i, _data + i);
		}
		
		if (!detail::vector_realloc_helper<T>::moved)
		{
			for (size_type i = 0; i < _size; ++i)
			{
				ATraits::destroy(alloc, _data + i);
			}
		}
		
		ATraits::deallocate(alloc, _data, _cap);
		
		_data = nData;
		_cap = n;
	}
	
	
	
	
	
	
	
	template <class T, class A>
	constexpr auto vector<T, A>::size() const -> size_type
	{
		return _size;
	}
	
	template <class T, class A>
	constexpr auto vector<T, A>::capacity() const -> size_type
	{
		return _cap;
	}
	
	template <class T, class A>
	constexpr bool vector<T, A>::empty() const
	{
		return _size == 0;
	}
	
	template <class T, class A>
	void vector<T, A>::reserve(size_type n)
	{
		if (capacity() < n)
		{
			reallocate(n);
		}
	}
	
	template <class T, class A>
	template <class... Args>
	void vector<T, A>::emplace_back(Args&&... args)
	{
		ensure_room_for(1);
		ATraits::template construct<T>(alloc, _data + _size, forward<Args>(args)...);
		++_size;
	}
}
#endif