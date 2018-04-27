#ifndef INCLUDED_MAP_HPP
#define INCLUDED_MAP_HPP

#include "Map.hh"
#include "RBTree.hpp"
#include "functexcept.hh"

namespace Utils
{
	template <class Key, class T, class Comp, class Alloc>
	void map<Key, T, Comp, Alloc>::initTree()
	{
		typedef typename Alloc::template rebind<Tree_t>::other TAlloc;
		TAlloc ta(get_allocator());
		tree = Allocator_Traits<TAlloc>::allocate(ta, 1);
		Allocator_Traits<TAlloc>::construct(ta, tree);
		
	}
	
	
	
	
	template <class Key, class T, class Comp, class Alloc>
	map<Key, T, Comp, Alloc>::map() : alloc(), tree(nullptr)
	{
		
	}
	
	template <class Key, class T, class Comp, class Alloc>
	map<Key, T, Comp, Alloc>::map(map&& m) : tree(m.tree)
	{
		m.tree = nullptr;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	map<Key, T, Comp, Alloc>::map(const map& m) : tree(m.tree->clone(typename Alloc::template rebind<Tree_t>::other()))
	{
		
	}
	
	
	
	
	template <class Key, class T, class Comp, class Alloc>
	map<Key, T, Comp, Alloc>::~map()
	{
		if (tree)
		{
			auto a = get_allocator();
			pair_allocator p(a);
			tree->destroy(p);
			
			typedef typename Alloc::template rebind<Tree_t>::other TAlloc;
			TAlloc ta;
			Allocator_Traits<TAlloc>::destroy(ta, tree);
			Allocator_Traits<TAlloc>::deallocate(ta, tree, 1);
		}
	}
	
	
	
	
	
	
	
	
	// Member Functions
	
	template <class Key, class T, class Comp, class Alloc>
	bool map<Key, T, Comp, Alloc>::empty() const noexcept
	{
		return size() == 0;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::size() const noexcept -> size_type
	{
		return tree != nullptr ? tree->size() : 0;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::get_allocator() const noexcept -> allocator_type
	{
		return alloc;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::at(const key_type& key) -> mapped_type&
	{
		if (!tree)
		{
			__throw_out_of_range("key not in map");
		}
		auto ptr = tree->find(key);
		ASSERT(ptr != nullptr);
		
		return ptr->value.second;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::at(key_type&& key) -> mapped_type&
	{
		if (!tree)
		{
			__throw_out_of_range("key not in map");
		}
		auto ptr = tree->find(forward<key_type>(key));
		ASSERT(ptr != nullptr);
		
		return ptr->value.second;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::at(const key_type& key) const -> const mapped_type&
	{
		if (!tree)
		{
			__throw_out_of_range("key not in map");
		}
		auto ptr = tree->find(key);
		ASSERT(ptr != nullptr);
		
		return ptr->value.second;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::at(key_type&& key) const -> const mapped_type&
	{
		if (!tree)
		{
			__throw_out_of_range("key not in map");
		}
		auto ptr = tree->find(forward<key_type>(key));
		ASSERT(ptr != nullptr);
		
		return ptr->value.second;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::count(const key_type& key) const noexcept -> size_type
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
	
	template <class Key, class T, class Comp, class Alloc>
	void map<Key, T, Comp, Alloc>::clear()
	{
		if (tree)
		{
			tree->clear();
		}
	}
	
	
	
	
	
	
	
	
	
	// Operators
	
	template <class Key, class T, class Comp, class Alloc>
	T& map<Key, T, Comp, Alloc>::operator[](const key_type& k)
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
			#ifdef DEBUG_VERIFY
			auto oldSize = tree->size();
			#endif
			auto ret = tree->create(parent, *ptr, k);
			ASSERT(ret != nullptr);
			#ifdef DEBUG_VERIFY
			ASSERT(ret->size >= 1);
			ASSERT(tree->verify());
			//ASSERTEQ(oldSize+1, tree->size());
			//ASSERT(oldSize + 1 == tree->size());
			#endif
			return ret->value.second;
		}
		
		
		return (*ptr)->value.second;
	}
	
	template <class Key, class T, class Comp, class Alloc>
	T& map<Key, T, Comp, Alloc>::operator[](key_type&& k)
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
			#ifdef DEBUG_VERIFY
			auto oldSize = tree->size();
			#endif
			auto ret = tree->create(parent, *ptr, forward<key_type&&>(k));
			ASSERT(ret != nullptr);
			#ifdef DEBUG_VERIFY
			ASSERT(ret->size >= 1);
			ASSERT(tree->verify());
			ASSERTEQ(oldSize+1, tree->size());
			ASSERT(oldSize + 1 == tree->size());
			#endif
			return ret->value.second;
		}
		
		
		return (*ptr)->value.second;
	}
	
	
	
	
	
	
	
	
	
	
	// Iterators
	
	template <class Key, class T, class Comp, class Alloc>
	template <class N>
	void map<Key, T, Comp, Alloc>::map::iterator_base<N>::increment()
	{
		node = detail::rb_tree::iterator_increment(node);
	}
	
	
	
	
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::begin() -> iterator
	{
		if (tree)
		{
			return iterator(tree->leftbound());
		}
		return iterator();
	}
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::end() -> iterator
	{
		if (tree)
		{
			return iterator((_VNode*)tree->endNode());
		}
		
		return iterator();
	}
}

#endif