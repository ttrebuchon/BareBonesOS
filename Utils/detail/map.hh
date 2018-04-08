#ifndef INCLUDED_MAP_HH
#define INCLUDED_MAP_HH

#include <Common.h>
#include <Utils/Compare.hh>
#include <Utils/Allocator.hh>
#include <Utils/Allocator_Traits.hh>
#include <Utils/pair>
#include "RBTree.hh"

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
		
		struct value_type_compare
		{
			key_compare key;
			
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
		
		
		typedef detail::rb_tree::RBTree<value_type, value_type_compare, allocator_type> Tree_t;
		typedef detail::rb_tree::NodeBase _Node;
		typedef detail::rb_tree::Node<value_type, value_type_compare> _VNode;
		
		
		public:
		
		
		private:
		template <class N>
		class iterator_base
		{
			protected:
			N node;
			void increment();
			void decrement();
			
			iterator_base(const N n) : node(n)
			{}
			
			public:
			
			
			const value_type& operator*() const
			{
				return node->value;
			}
			
			const value_type* operator->() const
			{
				return &node->value;
			}
			
			operator bool() const
			{
				return node != nullptr;
			}
			
			template <class N2>
			bool operator==(const iterator_base<N2>& r) const
			{
				return node == r.node;
			}
			
			template <class N2>
			bool operator!=(const iterator_base<N2>& r) const
			{
				return node != r.node;
			}
		};
		
		public:
		class iterator : public iterator_base<_VNode*>
		{
			private:
			typedef iterator_base<_VNode*> _Base;
			
			public:
			iterator() : _Base(nullptr)
			{}
			
			iterator(_VNode* n) : _Base(n)
			{}
			
			iterator& operator++()
			{
				this->increment();
				return *this;
			}
			
			iterator operator++(int)
			{
				auto old = *this;
				this->increment();
				return old;
			}
			
			iterator& operator--()
			{
				this->decrement();
				return *this;
			}
			
			iterator operator--(int)
			{
				auto old = *this;
				this->decrement();
				return old;
			}
			
			value_type& operator*()
			{
				return this->node->value;
			}
			
		};
		
		
		class const_iterator : public iterator_base<const _VNode*>
		{
			protected:
			
			private:
			typedef iterator_base<const _VNode*> _Base;
			
			public:
			const_iterator() : _Base(nullptr)
			{}
			const_iterator(const _VNode* n) : _Base(n)
			{}
			
			const_iterator& operator++()
			{
				this->increment();
				return *this;
			}
			
			const_iterator operator++(int)
			{
				auto old = *this;
				this->increment();
				return old;
			}
			
			const_iterator& operator--()
			{
				this->decrement();
				return *this;
			}
			
			const_iterator operator--(int)
			{
				auto old = *this;
				this->decrement();
				return old;
			}
			
			bool operator==(const const_iterator) const;
			bool operator!=(const const_iterator) const;
		};
		
		
		
		private:
		typedef Allocator_Traits<allocator_type> ATraits;
		
		protected:
		
		
		
		void initTree();
		
		
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
		bool empty() const noexcept;
		size_type size() const noexcept;
		size_type max_size() const noexcept;
		allocator_type get_allocator() const noexcept;
		mapped_type& at(const key_type&);
		mapped_type& at(key_type&&);
		const mapped_type& at(const key_type&) const;
		const mapped_type& at(key_type&&) const;
		size_type count(const key_type&) const noexcept;
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