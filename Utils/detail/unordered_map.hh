#ifndef INCLUDED_UNORDERED_MAP_HH
#define INCLUDED_UNORDERED_MAP_HH

#include <Common.h>
#include <Utils/Compare.hh>
#include <Utils/Allocator.hh>
#include <Utils/Allocator_Traits.hh>
#include <Utils/pair>
#include "hash_table.hh"
#include "hash_table.hpp"
#include "selectn.hh"


namespace Utils
{
	template <class Key, class T, class Hash = hash<Key>, class KeyEqual = equal_to<Key>, class Alloc = Allocator<pair<const Key, T>>>
	class unordered_map
	{
		public:
		// Member Types
		typedef pair<const Key, T> value_type;
		typedef Alloc allocator_type;
		typedef Key key_type;
		typedef T mapped_type;
		typedef Hash hasher;
		typedef KeyEqual key_equal;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef typename Allocator_Traits<allocator_type>::pointer pointer;
		typedef typename Allocator_Traits<allocator_type>::const_pointer const_pointer;
		
		
		
		
		typedef size_t size_type;
		
		
		protected:
		
		struct value_type_equal
		{
			key_equal key;
			
			value_type_equal(const key_equal& e) : key(e)
			{}
			value_type_equal() = default;
			
			bool operator()(const value_type& l, const value_type& r) const noexcept
			{
				return key(l.first, r.first);
			}
			
			bool operator()(const key_type& l, const value_type& r) const noexcept
			{
				return key(l, r.first);
			}
			
			bool operator()(const value_type& l, const key_type& r) const noexcept
			{
				return key(l.first, r);
			}
		};
		
		
		/*struct value_type_hasher
		{
			hasher key;
			
			value_type_hasher(const hasher& h) : key(h)
			{}
			value_type_hasher() = default;
			
			bool operator()(const value_type& v) const noexcept
			{
				return key(v);
			}
		};*/
		
		//typedef hash_table<key_type, value_type, Select1st, value_type_hasher, value_type_equal, allocator_type> Table;
		typedef hash_table<key_type, value_type, Select1st, hasher, key_equal, allocator_type> Table;
		
		Table _table;
		
		public:
		class iterator// : public iterator_base<_VNode*>
		{
			private:
			//typedef iterator_base<_VNode*> _Base;
			
			public:
			/*iterator() : _Base(nullptr)
			{}
			
			iterator(_VNode* n) : _Base(n)
			{}*/
			
			iterator& operator++();
			iterator operator++(int);
			iterator& operator--();
			iterator operator--(int);
			value_type& operator*();
			
			bool operator==(const iterator&) const;
			bool operator!=(const iterator&) const;
		};
		
		
		class const_iterator// : public iterator_base<const _VNode*>
		{
			protected:
			
			private:
			//typedef iterator_base<const _VNode*> _Base;
			
			public:
			/*const_iterator() : _Base(nullptr)
			{}
			const_iterator(const _VNode* n) : _Base(n)
			{}*/
			
			const_iterator& operator++();
			const_iterator operator++(int);
			const_iterator& operator--();
			const_iterator operator--(int);
			bool operator==(const const_iterator) const;
			bool operator!=(const const_iterator) const;
		};
		
		
		// Constructors / Destructors
		unordered_map();
		explicit unordered_map(size_t bucketHint, const hasher& = hasher(), const key_equal& = key_equal(), const allocator_type& alloc = allocator_type());
		unordered_map(const unordered_map&);
		~unordered_map() = default;
		
		
		
		
		
		// Member Functions
		bool empty() const;
		size_type size() const;
		size_type max_size() const;
		allocator_type get_allocator() const noexcept;
		mapped_type& at(const key_type&);
		mapped_type& at(key_type&&);
		const mapped_type& at(const key_type&) const;
		const mapped_type& at(key_type&&) const;
		size_type count(const key_type&) const;
		size_type count(key_type&&) const;
		void clear();
		
		
		
		
		// Operators
		mapped_type& operator[](const key_type&);
		mapped_type& operator[](key_type&&);
		
		
		// Iteration
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
	};
}


#endif