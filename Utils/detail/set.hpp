#ifndef INCLUDED_UTILS_SET_HPP
#define INCLUDED_UTILS_SET_HPP

#include "set.hh"
#include "RBTree.hpp"
#include "functexcept.hh"

namespace Utils
{
	template <class T, class Comp, class Alloc>
	void set<T, Comp, Alloc>::initTree()
	{
		typedef typename Alloc::template rebind<Tree_t>::other TAlloc;
		TAlloc ta(get_allocator());
		tree = Allocator_Traits<TAlloc>::allocate(ta, 1);
		Allocator_Traits<TAlloc>::construct(ta, tree, alloc, cmp);
		
	}
	
	
	
	
	template <class T, class Comp, class Alloc>
	set<T, Comp, Alloc>::set() : set(key_compare(), allocator_type())
	{
		
	}
	
	template <class T, class Comp, class Alloc>
	set<T, Comp, Alloc>::set(const key_compare& cmp, const allocator_type& a) : alloc(a), tree(nullptr), cmp(cmp)
	{
		
	}
	
	
	
	
	template <class T, class Comp, class Alloc>
	set<T, Comp, Alloc>::~set()
	{
		if (tree)
		{
			auto a = get_allocator();
			tree->destroy(a);
			
			typedef typename Alloc::template rebind<Tree_t>::other TAlloc;
			TAlloc ta;
			Allocator_Traits<TAlloc>::destroy(ta, tree);
			Allocator_Traits<TAlloc>::deallocate(ta, tree, 1);
		}
	}
	
	
	
	
	
	
	// Member Functions
	
	template <class T, class Comp, class Alloc>
	bool set<T, Comp, Alloc>::empty() const noexcept
	{
		return size() == 0;
	}
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::size() const noexcept -> size_type
	{
		return tree != nullptr ? tree->size() : 0;
	}
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::get_allocator() const -> allocator_type
	{
		return alloc;
	}
	
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::count(const key_type& key) const -> size_type
	{
		if (tree)
		{
			return (size_type)(tree->find(key) != nullptr);
		}
		else
		{
			return 0;
		}
	}
	
	template <class T, class Comp, class Alloc>
	void set<T, Comp, Alloc>::clear() noexcept
	{
		if (tree)
		{
			tree->clear();
		}
	}
	
	
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::insert(const value_type& k) -> pair<iterator, bool>
	{
		if (!tree)
		{
			initTree();
		}
		typename Tree_t::node* parent;
		auto ptr = tree->findCreate(k, &parent);
		ASSERT(ptr != nullptr);
		
		if (*ptr == nullptr)
		{
			auto ret = tree->create(parent, *ptr, k);
			ASSERT(ret != nullptr);
			return pair<iterator, bool>(iterator(ret), true);
		}
		
		
		return pair<iterator, bool>(iterator(*ptr), false);
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// Iterators
	
	template <class T, class Comp, class Alloc>
	template <class N>
	void set<T, Comp, Alloc>::set::iterator_base<N>::increment()
	{
		node = detail::rb_tree::iterator_increment(node);
	}
	
	
	
	
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::begin() noexcept -> iterator
	{
		if (tree)
		{
			return iterator(tree->leftbound());
		}
		return iterator();
	}
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::end() noexcept -> iterator
	{
		if (tree)
		{
			return iterator((_VNode*)tree->endNode());
		}
		
		return iterator();
	}
	
	
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::cbegin() const noexcept -> const_iterator
	{
		if (tree)
		{
			return const_iterator(tree->leftbound());
		}
		return const_iterator();
	}
	
	template <class T, class Comp, class Alloc>
	auto set<T, Comp, Alloc>::cend() const noexcept -> const_iterator
	{
		if (tree)
		{
			return const_iterator((_VNode*)tree->endNode());
		}
		
		return const_iterator();
	}
}

#endif