#ifndef INCLUDED_VECTOR_HH
#define INCLUDED_VECTOR_HH

#include <Common.h>
#include "Allocator.hh"
#include "Allocator_Traits.hh"

namespace Utils
{
	
	template <class T, class Alloc = Allocator<T>>
	class vector
	{
		protected:
		typedef Allocator_Traits<Alloc> ATraits;
		
		public:
		
		typedef T               value_type;
		typedef Alloc           allocator_type;
        typedef typename Allocator_Traits<Alloc>::pointer pointer;
        typedef typename Allocator_Traits<Alloc>::const_pointer const_pointer;
        typedef typename Allocator_Traits<Alloc>::reference reference;
        typedef typename Allocator_Traits<Alloc>::const_reference const_reference;
        typedef size_t          size_type;
		
		typedef value_type* iterator;
		typedef const value_type* const_iterator;
		
		protected:
		T* _data;
		size_type _size;
		size_type _cap;
		Alloc alloc;
		
		void ensure_capacity(size_type);
		void ensure_room_for(size_type n)
		{
			ensure_capacity(_size + n);
		}
		void reallocate(size_type);
		
		public:
		constexpr vector();
		constexpr explicit vector(const allocator_type&);
		explicit vector(size_type n, const allocator_type& alloc = allocator_type());
		vector(size_type n, const value_type& v, const allocator_type& alloc = allocator_type());
		template <class InputIt>
		vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type());
		vector(const vector&);
		vector(const vector&, const allocator_type&);
		vector(vector&&);
		vector(vector&&, const allocator_type&);
		//vector(initializer_list<value_type> il, const allocator_type& = allocator_type());
		~vector();
		
		// Member Functions
		constexpr size_type size() const noexcept;
		constexpr size_type max_size() const noexcept;
		void resize(size_type);
		void resize(size_type, const value_type&);
		constexpr size_type capacity() const noexcept;
		constexpr bool empty() const noexcept;
		void reserve(size_type);
		void shrink_to_fit();
		reference at(size_type);
		const_reference at(size_type) const;
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		value_type* data()
		{ return _data; }
		const value_type* data() const
		{ return _data; }
		
		template <class InputIt>
		void assign(InputIt first, InputIt last);
		void assign(size_type n, const value_type&);
		//void assign(initializer_list<value_type>);
		void push_back(const value_type&);
		void push_back(value_type&&);
		void pop_back();
		iterator insert(const_iterator pos, const value_type&);
		iterator insert(const_iterator pos, size_type n, const value_type&);
		template <class InputIt>
		iterator insert(const_iterator pos, InputIt first, InputIt last);
		iterator insert(const_iterator, value_type&&);
		//iterator insert(const_iterator, initializer_list<value_type>);
		iterator erase(const_iterator);
		iterator erase(const_iterator, const_iterator);
		void swap(vector&);
		void clear() noexcept;
		template <class... Args>
		iterator emplace(const_iterator, Args&&... args);
		template <class... Args>
		void emplace_back(Args&&... args);
		allocator_type get_allocator() const noexcept;
		
		
		// Operators
		reference operator[](size_type i);
		const_reference operator[](size_type) const;
		
		
		// Iterators
		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;
		
	};
	
	
	
	template <class T, class A>
	bool operator==(const vector<T, A>& l, const vector<T, A>& r)
	{
		if (l.size() != r.size())
		{
			return false;
		}
		auto lIt = l.begin();
		auto rIt = r.begin();
		while (lIt != l.end())
		{
			if (*lIt != *rIt)
			{
				return false;
			}
			++lIt;
			++rIt;
		}
		return true;
	}
	
	template <class T, class A>
	bool operator!=(const vector<T, A>& l, const vector<T, A>& r)
	{
		return !(l == r);
	}
}
#endif