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
		
		
		struct pair_allocator : public allocator_type
		{
			pair_allocator() : allocator_type()
			{
				
			}
			
			pair_allocator(const allocator_type& a) : allocator_type(a)
			{
				
			}
			
			template <class U, class... Args>
			void construct(U* ptr, Args... args)
			{
				allocator_type::construct(ptr, forward<Args>(args)...);
			}
			
			
			void construct(value_type* ptr, const key_type k)
			{
				allocator_type::construct(ptr, piecewise_construct, make_tuple(k), make_tuple());
			}
			
			
		};
		
		
		typedef hash_table<key_type, value_type, Select1st, hasher, key_equal, pair_allocator> Table;
		
		Table _table;
		
		public:
		
		// Iterator Types
		
		class const_iterator;
		class iterator
		{
			protected:
			typedef typename Table::iterator Table_it;
			
			Table_it _it;
			
			iterator(const Table_it& it) : _it(it)
			{}
			
			
			public:
			iterator() : _it()
			{}
			
			iterator(const iterator& it) : _it(it._it)
			{}
			
			iterator(iterator&& it) : _it(move(it._it))
			{}
			
			iterator& operator++();
			iterator operator++(int);
			value_type& operator*() const;
			value_type* operator->() const;
			
			explicit operator const_iterator() const;
			
			bool operator==(const iterator&) const;
			bool operator!=(const iterator&) const;
			bool operator==(const const_iterator&) const;
			bool operator!=(const const_iterator&) const;
			
			friend class const_iterator;
			friend class unordered_map;
		};
		
		
		class const_iterator
		{
			protected:
			typedef typename Table::const_iterator Table_it;
			
			Table_it _it;
			
			const_iterator(const Table_it& it) : _it(it)
			{}
			
			
			
			public:
			const_iterator() : _it()
			{}
			
			const_iterator(const const_iterator& it) : _it(it._it)
			{}
			
			const_iterator(const_iterator&& it) : _it(move(it._it))
			{}
			
			explicit const_iterator(const iterator& it) : _it((Table_it)it._it)
			{}
			
			
			
			const_iterator& operator++();
			const_iterator operator++(int);
			const value_type& operator*();
			
			bool operator==(const const_iterator&) const;
			bool operator!=(const const_iterator&) const;
			bool operator==(const iterator&) const;
			bool operator!=(const iterator&) const;
			
			
			friend class iterator;
			friend class unordered_map;
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
		allocator_type get_allocator() const;
		mapped_type& at(const key_type&);
		mapped_type& at(key_type&&);
		const mapped_type& at(const key_type&) const;
		const mapped_type& at(key_type&&) const;
		size_type count(const key_type&) const;
		size_type count(key_type&&) const;
		void clear();
		float max_load_factor() const noexcept;
		void max_load_factor(float);
		float load_factor() const noexcept;
		hasher hash_function() const;
		key_equal key_eq() const noexcept;
		size_type bucket_count() const noexcept;
		size_type max_bucket_count() const noexcept;
		size_type bucket_size(size_type n) const;
		void rehash(size_type);
		void reserve(size_type);
		
		
		
		// Operators
		mapped_type& operator[](const key_type&);
		mapped_type& operator[](key_type&&);
		
		unordered_map& operator=(const unordered_map&);
		unordered_map& operator=(unordered_map&&);
		
		
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