#ifndef INCLUDED_BITS_VECTOR_BOOL_HPP
#define INCLUDED_BITS_VECTOR_BOOL_HPP

#include "vector_bool.hh"
#include <Utils/detail/functexcept.hh>
#include <Std/cstring>

namespace Utils
{
	
	
	template <class A>
	constexpr vector<bool, A>::vector() : _data(nullptr), _size(0), _cap(0), _under_count(0), alloc()
	{
		
	}
	
	template <class A>
	constexpr vector<bool, A>::vector(const allocator_type& alloc) : _data(nullptr), _size(0), _cap(0), _under_count(0), alloc(alloc)
	{
		
	}
	
	template <class A>
	vector<bool, A>::vector(size_type n, const allocator_type& alloc) : _data(nullptr), _size(0), _cap(0), _under_count(0), alloc(alloc)
	{
		if (n > 0)
		{
			resize(n);
		}
	}
	
	template <class A>
	vector<bool, A>::vector(const vector& v) : _data(nullptr), _size(0), _cap(0), _under_count(0), alloc(v.alloc)
	{
		if (v._data)
		{
			ensure_capacity(v.size());
			memcpy(_data, v._data, v._under_count);
			_size = v.size();
			ASSERT(_cap >= v.size());
		}
	}
	
	template <class A>
	vector<bool, A>::vector(vector&& v) : _data(v._data), _size(v._size), _cap(v._cap), alloc(Utils::move(v.alloc))
	{
		v._data = nullptr;
		v._size = 0;
		v._cap = 0;
	}
	
	template <class A>
	vector<bool, A>::~vector()
	{
		if (_data)
		{
			auto a = getAlloc();
			ATraits::deallocate(a, _data, _under_count);
			_data = nullptr;
			_size = _cap = _under_count = 0;
		}
	}
	
	
	
	
	template <class A>
	void vector<bool, A>::ensure_capacity(size_type n)
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
	
	template <class A>
	void vector<bool, A>::reallocate(size_type n)
	{
		auto a = getAlloc();
		
		
		if (n == 0)
		{
			if (_data)
			{
				clear();
				ATraits::deallocate(a, _data, _under_count);
				_data = nullptr;
				_cap = 0;
				_under_count = 0;
			}
			return;
		}
		const auto count = n / (sizeof(storage_type)*8) + (n % (sizeof(storage_type)*8) != 0 ? 1 : 0);
		auto nData = static_cast<storage_type*>(ATraits::allocate(a, count));
		if (_data)
		{
			memcpy(nData, _data, _under_count);
			ATraits::deallocate(a, _data, _cap);
		}
		else
		{
			memset(nData, 0, count*sizeof(storage_type));
		}
		
		_data = nData;
		_cap = count*sizeof(storage_type)*8;
		_under_count = count;
	}
	
	
	
	
	
	
	
	template <class A>
	constexpr auto vector<bool, A>::size() const noexcept -> size_type
	{
		return _size;
	}
	
	template <class A>
	void vector<bool, A>::resize(size_type n)
	{
		if (n > _size)
		{
			auto oldCount = _under_count;
			if (n > _cap)
			{
				ensure_capacity(n);
			}
			
			memset(_data+_size, 0, _under_count - oldCount);
			_size = n;
		}
		else if (n > 0)
		{
			_size = n;
		}
		else if (_data != nullptr)
		{
			clear();
			auto a = getAlloc();
			ATraits::deallocate(a, _data, _under_count);
			_data = nullptr;
			_size = _cap = _under_count = 0;
		}
	}
	
	template <class A>
	void vector<bool, A>::resize(size_type n, const value_type& val)
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
	
	template <class A>
	constexpr auto vector<bool, A>::capacity() const noexcept -> size_type
	{
		return _cap;
	}
	
	template <class A>
	constexpr bool vector<bool, A>::empty() const noexcept
	{
		return _size == 0;
	}
	
	template <class A>
	void vector<bool, A>::reserve(size_type n)
	{
		if (capacity() < n)
		{
			reallocate(n);
		}
	}
	
	template <class A>
	void vector<bool, A>::shrink_to_fit()
	{
		if (capacity() > size())
		{
			reallocate(size());
		}
	}
	
	template <class A>
	auto vector<bool, A>::at(size_type i) -> reference
	{
		if (i < _size)
		{
			return reference(_data[i / storage_bits], 1 << (i % storage_bits));
		}
		
		__throw_out_of_range("Index out of range");
	}
	
	template <class A>
	auto vector<bool, A>::at(size_type i) const -> const_reference
	{
		if (i < _size)
		{
			return (_data[i / storage_bits]) & (1 << (i % storage_bits));
			return ((1 << (i % storage_bits)) & _data[(i / sizeof(storage_type))]) != 0;
		}
		
		__throw_out_of_range("Index out of range");
	}
	
	template <class A>
	auto vector<bool, A>::front() -> reference
	{
		return *_data;
	}
	
	template <class A>
	auto vector<bool, A>::front() const -> const_reference
	{
		return *_data;
	}
	
	template <class A>
	void vector<bool, A>::push_back(const value_type& v)
	{
		ensure_room_for(1);
		ATraits::construct(alloc, _data + _size, v);
		++_size;
	}
	
	template <class A>
	void vector<bool, A>::push_back(value_type&& v)
	{
		ensure_room_for(1);
		ATraits::construct(alloc, _data + _size, forward<value_type&&>(v));
		++_size;
	}
	
	template <class A>
	void vector<bool, A>::pop_back()
	{
		assert(!empty());
		auto i = size()-1;
		ATraits::destroy(alloc, _data + i);
		--_size;
	}
	
	template <class A>
	void vector<bool, A>::clear() noexcept
	{
		if (_data)
		{
			_size = 0;
		}
	}
	
	template <class A>
	template <class... Args>
	void vector<bool, A>::emplace_back(Args&&... args)
	{
		ensure_room_for(1);
		if (bool(forward<Args>(args)...))
		{
			_data[_size / storage_bits] |= (1 << (_size % storage_bits));
		}
		else
		{
			_data[_size / storage_bits] &= ~(1 << (_size % storage_bits));
		}
		++_size;
	}
	
	
	
	
	
	
	template <class A>
	auto vector<bool, A>::operator[](size_type i) -> reference
	{
		return reference(_data[i / storage_bits], 1 << (i % storage_bits));
	}
	
	template <class A>
	auto vector<bool, A>::operator[](size_type i) const -> const_reference
	{
		return ((1 << (i % storage_bits)) & _data[(i / sizeof(storage_type))]) != 0;
	}
	
	
	
	
	
	
	
	
	template <class A>
	auto vector<bool, A>::begin() noexcept -> iterator
	{
		return _data;
	}
	
	template <class A>
	auto vector<bool, A>::end() noexcept -> iterator
	{
		return _data + _size;
	}
	
	template <class A>
	auto vector<bool, A>::begin() const noexcept -> const_iterator
	{
		return _data;
	}
	
	template <class A>
	auto vector<bool, A>::end() const noexcept -> const_iterator
	{
		return _data + _size;
	}
	
	template <class A>
	auto vector<bool, A>::cbegin() const noexcept -> const_iterator
	{
		return _data;
	}
	
	template <class A>
	auto vector<bool, A>::cend() const noexcept -> const_iterator
	{
		return _data + _size;
	}
}
#endif