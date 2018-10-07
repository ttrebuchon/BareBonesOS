#ifndef INCLUDED_UTILS_SET_HH
#define INCLUDED_UTILS_SET_HH

#include <Common.h>
#include <Utils/Compare.hh>
#include <Utils/Allocator.hh>
#include <Utils/Allocator_Traits.hh>
#include <Utils/pair>
#include "RBTree.hh"

namespace Utils
{
	template <class T, class Comp = less<T>, class Alloc = allocator<const T>>
	class set
	{
		public:
		// Member Types
		typedef Alloc allocator_type;
		typedef T key_type;
		typedef T value_type;
		typedef Comp key_compare;
		typedef Comp value_compare;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef typename Allocator_Traits<allocator_type>::pointer pointer;
		typedef typename Allocator_Traits<allocator_type>::const_pointer const_pointer;
		
		
		
		
		typedef size_t size_type;
		
		
		private:
		
		
		typedef detail::rb_tree::RBTree<value_type, value_compare, allocator_type> Tree_t;
		typedef detail::rb_tree::NodeBase _Node;
		typedef detail::rb_tree::Node<value_type, value_compare> _VNode;
		
		
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
			
			/*bool operator==(const const_iterator) const;
			bool operator!=(const const_iterator) const;*/
		};
		
		private:
		
		typedef Allocator_Traits<allocator_type> ATraits;
		
		protected:
		
		
		
		void initTree();
		
		
		// Members
		allocator_type alloc;
		Tree_t* tree;
		value_compare cmp;
		
		
		public:
		set();
		explicit set(const value_compare& comp, const allocator_type& = allocator_type());
		
		explicit set(const allocator_type&);
		
		~set();
		
		
		allocator_type get_allocator() const;
		bool empty() const noexcept;
		size_type size() const noexcept;
		void clear() noexcept;
		pair<iterator, bool> insert(const value_type&);
		pair<iterator, bool> insert(value_type&&);
		
		
		size_type count(const value_type&) const;
		
		
		iterator begin() noexcept;
		const_iterator cbegin() const noexcept;
		const_iterator begin() const noexcept
		{ return cbegin(); }
		
		
		iterator end() noexcept;
		const_iterator cend() const noexcept;
		const_iterator end() const noexcept
		{ return cend(); }
	};
}

#endif