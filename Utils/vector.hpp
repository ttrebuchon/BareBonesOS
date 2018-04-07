#ifndef INCLUDED_VECTOR_HPP
#define INCLUDED_VECTOR_HPP

#include "vector.hh"
#include "detail/functexcept.hh"

namespace Utils
{
	namespace detail
	{
		template <class T, class = void>
		struct vector_realloc_helper
		{
			constexpr static bool moved = false;
			
			inline static void call(T* dest, const T* src)
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
	vector<T, A>::vector(size_type n, const allocator_type& alloc) : _data(nullptr), _size(0), _cap(0), alloc(alloc)
	{
		if (n > 0)
		{
			resize(n);
		}
	}
	
	template <class T, class A>
	vector<T, A>::vector(const vector& v) : _data(nullptr), _size(0), _cap(0), alloc(v.alloc)
	{
		if (v._data)
		{
			ensure_capacity(v.size());
			for (size_type i = 0; i < v._size; ++i)
			{
				alloc.construct(_data + i, v._data[i]);
			}
			_size = v.size();
			ASSERT(_cap >= v.size());
		}
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
				reallocate(_cap*2 > n ? _cap*2 : n);
			}
			else
			{
				reallocate(n);
			}
		}
	}
	
	template <class T, class A>
	void vector<T, A>::reallocate(size_type n)
	{
		if (n == 0)
		{
			if (_data)
			{
				clear();
				ATraits::deallocate(alloc, _data, _cap);
				_data = nullptr;
				_cap = 0;
			}
			return;
		}
		auto nData = static_cast<T*>(ATraits::allocate(alloc, n));
		for (size_type i = 0; i < _size; ++i)
		{
			detail::vector_realloc_helper<T>::call(nData + i, _data + i);
		}
		
		/*if (!detail::vector_realloc_helper<T>::moved)
		{*/
			for (size_type i = 0; i < _size; ++i)
			{
				ATraits::destroy(alloc, _data + i);
			}
		//}
		
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
	void vector<T, A>::resize(size_type n)
	{
		if (n > _size)
		{
			if (n > _cap)
			{
				ensure_capacity(n);
			}
			
			while (_size < n)
			{
				alloc.construct(_data+_size);
				++_size;
			}
		}
		else if (n > 0)
		{
			for (size_type i = n; i < _size; ++i)
			{
				alloc.destroy(_data + i);
			}
			_size = n;
		}
		else if (_data != nullptr)
		{
			clear();
			ATraits::deallocate(alloc, _data, _cap);
			_data = nullptr;
			_size = _cap = 0;
		}
	}
	
	template <class T, class A>
	void vector<T, A>::resize(size_type n, const value_type& val)
	{
		if (n > _size)
		{
			if (n > _cap)
			{
				ensure_capacity(n);
			}
			
			while (_size < n)
			{
				alloc.construct(_data+_size, val);
				++_size;
			}
		}
		else if (n > 0)
		{
			for (size_type i = n; i < _size; ++i)
			{
				alloc.destroy(_data + i);
			}
			_size = n;
		}
		else if (_data != nullptr)
		{
			clear();
			ATraits::deallocate(alloc, _data, _cap);
			_data = nullptr;
			_size = _cap = 0;
		}
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
	void vector<T, A>::shrink_to_fit()
	{
		if (capacity() > size())
		{
			reallocate(size());
		}
	}
	
	template <class T, class A>
	auto vector<T, A>::at(size_type i) -> reference
	{
		if (i < _size)
		{
			return _data[i];
		}
		
		__throw_out_of_range("Index out of range");
	}
	
	template <class T, class A>
	auto vector<T, A>::at(size_type i) const -> const_reference
	{
		if (i < _size)
		{
			return _data[i];
		}
		
		__throw_out_of_range("Index out of range");
	}
	
	template <class T, class A>
	void vector<T, A>::push_back(const value_type& v)
	{
		ensure_room_for(1);
		ATraits::construct(alloc, _data + _size, v);
		++_size;
	}
	
	template <class T, class A>
	void vector<T, A>::push_back(value_type&& v)
	{
		ensure_room_for(1);
		ATraits::construct(alloc, _data + _size, forward<value_type&&>(v));
		++_size;
	}
	
	template <class T, class A>
	void vector<T, A>::clear() noexcept
	{
		if (_data)
		{
			for (size_type i = 0; i < _size; ++i)
			{
				ATraits::destroy(alloc, _data + i);
			}
			_size = 0;
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
	
	
	
	
	
	
	template <class T, class A>
	auto vector<T, A>::operator[](size_type i) -> reference
	{
		return _data[i];
	}
	
	template <class T, class A>
	auto vector<T, A>::operator[](size_type i) const -> const_reference
	{
		return _data[i];
	}
	
	
	
	
	
	
	
	
	template <class T, class A>
	auto vector<T, A>::begin() noexcept -> iterator
	{
		return _data;
	}
	
	template <class T, class A>
	auto vector<T, A>::end() noexcept -> iterator
	{
		return _data + _size;
	}
	
	template <class T, class A>
	auto vector<T, A>::begin() const noexcept -> const_iterator
	{
		return _data;
	}
	
	template <class T, class A>
	auto vector<T, A>::end() const noexcept -> const_iterator
	{
		return _data + _size;
	}
	
	template <class T, class A>
	auto vector<T, A>::cbegin() const noexcept -> const_iterator
	{
		return _data;
	}
	
	template <class T, class A>
	auto vector<T, A>::cend() const noexcept -> const_iterator
	{
		return _data + _size;
	}
}
#endif