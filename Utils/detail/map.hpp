#ifndef INCLUDED_MAP_HPP
#define INCLUDED_MAP_HPP

#include "Map.hh"
#include "BinaryTree.hpp"

namespace Utils
{
	template <class Key, class T, class Comp, class Alloc>
	map<Key, T, Comp, Alloc>::map() : alloc(), tree(nullptr)
	{
		typedef typename Alloc::template rebind<Tree_t>::other TAlloc;
		TAlloc ta;
		tree = Allocator_Traits<TAlloc>::allocate(ta, 1);
		Allocator_Traits<TAlloc>::construct(ta, tree);
		
	}
	
	
	
	
	template <class Key, class T, class Comp, class Alloc>
	map<Key, T, Comp, Alloc>::~map()
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
	
	template <class Key, class T, class Comp, class Alloc>
	auto map<Key, T, Comp, Alloc>::get_allocator() const noexcept -> allocator_type
	{
		
		return alloc;
	}
	
	
	
	
	
	
	
	
	
	// Operators
	
	template <class Key, class T, class Comp, class Alloc>
	T& map<Key, T, Comp, Alloc>::operator[](Key&& k)
	{
		typename Tree_t::node* parent;
		auto ptr = tree->findCreate(k, &parent);
		if (*ptr == nullptr)
		{
			tree->create(parent, *ptr, forward<Key>(k)/*, forward<T>((T&&)T())*/);
			//get_allocator().construct((Key*)&(*ptr)->value.first, forward<Key>(k));
			//get_allocator().construct((T*)&(*ptr)->value.second);
		}
		
		
		return (*ptr)->value.second;
	}
}

#endif