#ifndef INCLUDED_STRING_HPP
#define INCLUDED_STRING_HPP

#include "String.hh"

namespace Utils
{
#define STRTEMP template <class Char_t, class T, class Alloc>
	
	STRTEMP constexpr Char_t basic_string<Char_t, T, Alloc>::_Rep::_S_terminal;
	
	STRTEMP template <class InIt>
	Char_t* basic_string<Char_t, T, Alloc>::_S_construct(InIt _begin, InIt _end, const Alloc& a)//, input_iterator_tag)
	{
		/*#ifndef GLIBXX_FULLY_DYNAMIC_STRING
		if (_begin == _end && a == Alloc())
		{
			return _S_empty_rep()._M_refdata();
		}
		#endif*/
		
		Char_t buf[128];
		size_type len = 0;
		while (_begin != _end && len < sizeof(buf) / sizeof(Char_t))
		{
			buf[len++] = *_begin;
			++_begin;
		}
		
		_Rep* r = _Rep::_S_create(len, size_type(0), a);
		_M_copy(r->_M_refdata(), buf, len);
		__try
		{
			while (_begin != _end)
			{
				if (len = r->_M_capacity)
				{
					_Rep* another = _Rep::_S_create(len + 1, len, a);
					_M_copy(another->_M_refdata(), r->_M_refdata(), len);
					r->_M_destroy(a);
					r = another;
				}
				r->_M_refdata()[len++] = *_begin;
				++_begin;
			}
		}
		__catch(...)
		{
			
		}
		r->_M_set_length_and_sharable(len);
		return r->_M_refdata();
	}
	
	STRTEMP basic_string<Char_t, T, Alloc>::basic_string(const Char_t* cstr, const Alloc& a) : _M_dataplus(_S_construct(cstr, cstr ? cstr + traits_type::length(cstr) : cstr + npos, a), a)
	{
		
	}
	
	STRTEMP basic_string<Char_t, T, Alloc>::basic_string(const basic_string& str) : _M_dataplus(str._M_rep()->_M_grab(Alloc(str.get_allocator()), str.get_allocator()), str.get_allocator())
	{
		
	}
	
	STRTEMP template <class It>
	basic_string<Char_t, T, Alloc>::basic_string(It first, It last, const Alloc& a) : _M_dataplus(_S_construct(first, last, a), a)
	{
		
	}
	
	STRTEMP typename basic_string<Char_t, T, Alloc>::_Rep* basic_string<Char_t, T, Alloc>::_Rep::_S_create(size_type _capacity, size_type oldCapacity, const Alloc& a)
	{
		if (_capacity > _S_max_size)
		{
			__throw_length_error("basic_string::_S_create");
		}
		
		const size_type pageSize = 4096;
		const size_type mallocHeaderSize = 4 * sizeof(void*);
		
		if (_capacity > oldCapacity && _capacity < 2 * oldCapacity)
		{
			_capacity = 2*oldCapacity;
		}
		
		size_type _size = (_capacity + 1) * sizeof(Char_t) + sizeof(_Rep);
		
		const size_type adjSize = _size + mallocHeaderSize;
		if (adjSize > pageSize && _capacity > oldCapacity)
		{
			const size_type extra = pageSize - (adjSize % pageSize);
			_capacity += extra;
			
			if (_capacity > _S_max_size)
			{
				_capacity = _S_max_size;
			}
			
			_size = (_capacity + 1)*sizeof(Char_t) + sizeof(_Rep);
		}
		
		void* _place = _Raw_bytes_alloc(a).allocate(_size);
		_Rep* r = new (_place) _Rep;
		r->_M_capacity = _capacity;
		
		r->_M_set_sharable();
		return r;
	}
	
	
	
	
	STRTEMP basic_string<Char_t, T, Alloc>& basic_string<Char_t, T, Alloc>::assign(const basic_string<Char_t, T, Alloc>& s)
	{
		if (_M_rep() != s._M_rep())
		{
			const allocator_type a = this->get_allocator();
			Char_t* tmp = s._M_rep()->_M_grab(a, s.get_allocator());
			_M_rep()->_M_dispose(a);
			_M_data(tmp);
		}
		return *this;
	}
	
	STRTEMP Char_t* basic_string<Char_t, T, Alloc>::_Rep::_M_clone(const Alloc& a, size_type res)
	{
		const size_type req_cap = this->_M_length + res;
		_Rep* r = _Rep::_S_create(req_cap, this->_M_capacity, a);
		if (this->_M_length)
		{
			_M_copy(r->_M_refdata(), _M_refdata(), this->_M_length);
		}
		
		r->_M_set_length_and_sharable(this->_M_length);
		return r->_M_refdata();
	}
	
	
	
	
	STRTEMP basic_string<Char_t, T, Alloc>& basic_string<Char_t, T, Alloc>::append(const Char_t* s, size_type n)
	{
		if (n)
		{
			_M_check_length(size_type(0), n, "basic_string::append");
			const size_type len = n + this->size();
			if (len > this->capacity() || _M_rep()->_M_is_shared())
			{
				if (_M_disjunct(s))
				{
					this->reserve(len);
				}
				else
				{
					const size_type off = s - _M_data();
					this->reserve(len);
					s = _M_data() + off;
					
				}
			}
			_M_copy(_M_data() + this->size(), s, n);
			_M_rep()->_M_set_length_and_sharable(len);
		}
		return *this;
	}
	
	
	
	STRTEMP basic_string<Char_t, T, Alloc>& basic_string<Char_t, T, Alloc>::append(const basic_string& s)
	{
		const size_type _size = s.size();
		if (_size)
		{
			const size_type len = this->size() + _size;
			if (len > this->capacity() || _M_rep()->_M_is_shared())
			{
				this->reserve(len);
			}
			
			_M_copy(_M_data() + this->size(), s._M_data(), _size);
			_M_rep()->_M_set_length_and_sharable(len);
		}
		return *this;
	}
	
	STRTEMP basic_string<Char_t, T, Alloc>& basic_string<Char_t, T, Alloc>::append(size_type n, Char_t c)
	{
		if (n)
		{
			_M_check_length(size_type(0), n, "basic_string::append");
			
			const size_type len = this->size() + n;
			if (len > capacity() || _M_rep()->_M_is_shared())
			{
				this->reserve(len);
			}
			_M_assign(_M_data() + this->size(), n, c);
			_M_rep()->_M_set_length_and_sharable(len);
		}
		return *this;
	}
	
	STRTEMP void basic_string<Char_t, T, Alloc>::reserve(size_type _res)
	{
		if (_res != this->capacity() || _M_rep()->_M_is_shared())
		{
			if (_res < this->size())
			{
				_res = this->size();
			}
			const allocator_type a = get_allocator();
			Char_t* tmp = _M_rep()->_M_clone(a, _res - this->size());
			_M_rep()->_M_dispose(a);
			_M_data(tmp);
		}
	}
	
	STRTEMP void basic_string<Char_t, T, Alloc>::swap(basic_string& str)
	{
		if (_M_rep()->_M_is_leaked())
		{
			_M_rep()->_M_set_sharable();
		}
		if (str._M_rep()->_M_is_leaked())
		{
			str._M_rep()->_M_set_sharable();
		}
		if (this->get_allocator() == str.get_allocator())
		{
			Char_t* tmp = _M_data();
			_M_data(str._M_data());
			str._M_data(tmp);
		}
		else
		{
			const basic_string tmp1(_M_ibegin(), _M_iend(), str.get_allocator());
			const basic_string tmp2(str._M_ibegin(), str._M_iend(), this->get_allocator());
			*this = tmp2;
			str = tmp1;
		}
	}
}

#endif