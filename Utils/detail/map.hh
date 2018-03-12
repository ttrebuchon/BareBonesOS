#ifndef INCLUDED_MAP_HH
#define INCLUDED_MAP_HH

#include <Common.h>
#include <Utils/Compare.hh>
#include <Utils/Allocator.hh>
#include <Utils/Allocator_Traits.hh>
#include <Utils/pair>
#include "BinaryTree.hh"

namespace Utils
{
	template <class Key, class T, class Comp = less<Key>, class Alloc = Allocator<pair<const Key, T>>>
	class map
	{
		public:
		// Member Types
		typedef pair<const Key, T> value_type;
		typedef Alloc allocator_type;
		typedef Key key_type;
		typedef T mapped_type;
		typedef Comp key_compare;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef typename Allocator_Traits<allocator_type>::pointer pointer;
		typedef typename Allocator_Traits<allocator_type>::const_pointer const_pointer;
		
		
		
		
		typedef size_t size_type;
		
		private:
		typedef Allocator_Traits<allocator_type> ATraits;
		
		protected:
		
		struct value_type_compare
		{
			key_compare key;
			
			bool operator()(const value_type& l, const value_type& r)
			{
				return key(l.first, r.first);
			}
			
			bool operator()(const key_type& l, const value_type& r)
			{
				return key(l, r.first);
			}
			
			bool operator()(const value_type& l, const key_type& r)
			{
				return key(l.first, r);
			}
		};
		
		typedef detail::binary_tree::BinaryTree<value_type, value_type_compare, allocator_type> Tree_t;
		
		
		// Members
		allocator_type alloc;
		Tree_t* tree;
		
		public:
		map();
		explicit map(const key_compare&, const allocator_type& = allocator_type());
		explicit map(const allocator_type&);
		template <class InputIt>
		map(InputIt first, InputIt last, const key_compare& = key_compare(), const allocator_type& = allocator_type());
		template <class InputIt>
		map(InputIt first, InputIt last, const allocator_type& = allocator_type());
		map(const map&);
		map(const map&, const allocator_type&);
		map(map&&);
		map(map&&, const allocator_type&);
		
		~map();
		
		
		
		// Member Functions
		bool empty() const;
		size_type size() const;
		size_type max_size() const;
		allocator_type get_allocator() const noexcept;
		mapped_type& at(const key_type&);
		mapped_type& at(key_type&&);
		const mapped_type& at(const key_type&) const;
		const mapped_type& at(key_type&&) const;
		
		
		
		// Operators
		mapped_type& operator[](const key_type&);
		mapped_type& operator[](key_type&&);
		
		
	};
}

#endif