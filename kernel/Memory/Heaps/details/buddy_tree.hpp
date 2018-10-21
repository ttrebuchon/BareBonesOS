#ifndef INCLUDED_MEMORY_HEAPS_BUDDY_TREE_HPP
#define INCLUDED_MEMORY_HEAPS_BUDDY_TREE_HPP

#include "../BuddyHeap.hh"
#include <Support/misc/bits.hh>

namespace Kernel::Memory::buddy_heap_detail
{
	template <class Alloc>
	buddy_tree<Alloc>::buddy_tree(void* start, uint8_t order, const n_allocator_type& a) : alloc(a), _root(nullptr), root(_root)
	{
		assert(order < 128);
		assert(order > 0);
		
		_root = alloc.allocate(1);
		alloc.construct(_root, start, order);
	}
	
	
	template <class Alloc>
	buddy_tree<Alloc>::~buddy_tree()
	{
		clear_tree();
	}
	
	template <class Alloc>
	auto buddy_tree<Alloc>::find_leaf_node(void* addr) noexcept -> node_type*
	{
		if (!_root)
		{
			return nullptr;
		}
		auto n = _root;
		while (n && !n->is_leaf())
		{
			n = n->next(addr);
		}
		
		assert(n);
		assert(n->is_leaf());
		assert(n->start() <= addr);
		assert(n->end() >= addr);
		//assert(n->start() == addr);
		return n;
	}
	
	template <class Alloc>
	auto buddy_tree<Alloc>::find_leaf_node(void* addr) const noexcept -> const node_type*
	{
		if (unlikely(!_root))
		{
			return nullptr;
		}
		
		if (unlikely((addr < _root->start()) || (addr > _root->end())))
		{
			return nullptr;
		}
		
		auto n = _root;
		while (n && !n->is_leaf())
		{
			n = n->next(addr);
		}
		
		assert(n);
		assert(n->is_leaf());
		assert(n->start() <= addr);
		assert(n->end() >= addr);
		//assert(n->start() == addr);
		return n;
	}
	
	template <class Alloc>
	void buddy_tree<Alloc>::release_node(node_type* n)
	{
		assert(n);
		assert(n->is_leaf());
		assert(n->full);
		if (n->parent)
		{
			auto p = n->parent;
			node_type* sibling = nullptr;
			if (p->left == n)
			{
				sibling = p->right;
			}
			else
			{
				assert(p->right == n);
				sibling = p->left;
			}
			
			if (sibling->is_leaf() && !sibling->full)
			{
				n = p;
				alloc.destroy(n->left);
				alloc.deallocate(n->left, 1);
				n->left = nullptr;
				alloc.destroy(n->right);
				alloc.deallocate(n->right, 1);
				n->right = nullptr;
				n->full = 0;
			}
			
			while (n)
			{
				n->full = 0;
				n = n->parent;
			}
		}
		else
		{
			n->full = 0;
		}
	}
	
	namespace
	{
		template <class Node>
		Node* allocate_node_recurse(Node* const n, const uint8_t order)
		{
			if (unlikely(n->full))
			{
				return nullptr;
			}
			else if (unlikely(n->order < order))
			{
				return nullptr;
			}
			else if (n->is_leaf())
			{
				return n;
			}
			else if (likely(n->order > order))
			{
				assert(!n->is_leaf());
				assert(n->left);
				assert(n->right);
				
				if (n->left && !n->left->full)
				{
					auto result = allocate_node_recurse(n->left, order);
					if (result)
					{
						return result;
					}
				}
				if (n->right && !n->right->full)
				{
					auto result = allocate_node_recurse(n->right, order);
					if (result)
					{
						return result;
					}
				}
				assert(n->left);
				assert(n->right);
			}
			
			return nullptr;
		}
	}
	
	template <class Alloc>
	auto buddy_tree<Alloc>::allocate_node(uint8_t order) -> node_type*
	{
		assert(order > 0);
		assert(order <= root->order);
		
		auto n = allocate_node_recurse(_root, order);
		
		if (n)
		{
			while (n->order > order)
			{
				split_node(n);
				n = n->left;
			}
		}
		
		//auto n = _root;
		/*while (n && n->order >= order)
		{
			if (n->full)
			{
				return nullptr;
			}
			else if (n->is_leaf())
			{
				while (n->order > order)
				{
					split_node(n);
					n = n->left;
				}
				break;
			}
			else if (n->left && !n->left->full && n->order >= order)
			{
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}*/
		
		if (!n)
		{
			return nullptr;
		}
		
		
		assert(n);
		n->full = 1;
		auto p = n->parent;
		while (p && p->update_full())
		{
			p = p->parent;
		}
		
		return n;
	}
	
	
	
	
	
	
	
	namespace
	{
		void recurse_clear_tree(auto& alloc, auto n)
		{
			if (n->left)
			{
				recurse_clear_tree(alloc, n->left);
				n->left = nullptr;
			}
			
			if (n->right)
			{
				recurse_clear_tree(alloc, n->right);
				n->right = nullptr;
			}
			
			alloc.destroy(n);
			alloc.deallocate(n, 1);
		}
		
	}
	
	template <class Alloc>
	void buddy_tree<Alloc>::clear_tree()
	{
		if (root)
		{
			recurse_clear_tree(alloc, _root);
		}
	}
	
	
	template <class Alloc>
	uint8_t buddy_tree<Alloc>::order_for(size_t sz) const noexcept
	{
		assert(sz > 0);
		int order = Support::misc::highest_bit_index(sz);
		size_t val = 1;
		val <<= order;
		if (val != sz)
		{
			++order;
			val <<= 1;
		}
		assert(val >= sz);
		return order;
	}
	
	
	template <class Alloc>
	void buddy_tree<Alloc>::split_node(node_type* n)
	{
		assert(n);
		assert(n->order > 0);
		assert(n->is_leaf());
		assert(!n->full);
		
		n->left = alloc.allocate(1);
		alloc.construct(n->left, n->start(), n->order-1);
		n->left->parent = n;
		
		
		n->right = alloc.allocate(1);
		alloc.construct(n->right, n->mid(), n->order-1);
		n->right->parent = n;
	}
	
}

#endif