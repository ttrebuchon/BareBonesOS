#ifndef INCLUDED_BINARYTREE_HPP
#define INCLUDED_BINARYTREE_HPP

#include "BinaryTree.hh"

namespace Utils { namespace detail
{
namespace binary_tree
{
	NodeBase::NodeBase() : size(1), left(nullptr), right(nullptr), parent(nullptr)
	{
		
	}
	
	
	template <class T, class Comp>
	template <class NAlloc, class Alloc>
	void Node<T, Comp>::destroyChildren(NAlloc& na, Alloc& a)
	{
		if (left)
		{
			((Self*)left)->destroyChildren(na, a);
			a.destroy(&((Self*)left)->value);
			na.destroy(left);
			na.deallocate((Self*)left, 1);
			left = nullptr;
		}
		
		if (right)
		{
			((Self*)right)->destroyChildren(na, a);
			a.destroy(&((Self*)right)->value);
			na.destroy(right);
			na.deallocate((Self*)right, 1);
			right = nullptr;
		}
	}
	
	
	
	
	
	
	
	template <class T, class Comp, class Alloc>
	BinaryTree<T, Comp, Alloc>::~BinaryTree()
	{
		if (root)
		{
			destroy(alloc);
		}
	}
	
	
	
	
	
	
	
	template <class T, class Comp, class Alloc>
	template <class Key>
	auto BinaryTree<T, Comp, Alloc>::find(const Key& k) const -> _Node*
	{
		if (root)
		{
			_Node* it = root;
			bool choice;
			while (it != nullptr)
			{
				choice = comp(k, it->value);
				if (comp(it->value, k))
				{
					if (choice)
					{
						return it;
					}
					
					it = (_Node*)it->right;
				}
				else
				{
					if (!choice)
					{
						return it;
					}
					
					it = (_Node*)it->left;
				}
			}
		}
		return nullptr;
	}
	
	template <class T, class Comp, class Alloc>
	template <class Key>
	auto BinaryTree<T, Comp, Alloc>::findCreate(const Key& k, _Node** parent) -> _Node**
	{
		if (root)
		{
			_Node **link, *ptr, *prev;
			prev = root;
			bool choice;
			if (choice = comp(k, prev->value))
			{
				ptr = (_Node*)prev->left;
				link = (_Node**)&prev->left;
			}
			else
			{
				ptr = (_Node*)prev->right;
				link = (_Node**)&prev->right;
			}
			
			if (choice == comp(prev->value, k))
			{
				if (parent)
				{
					*parent = prev;
				}
				return &root;
			}
			
			
			while (ptr != nullptr)
			{
				choice = comp(k, ptr->value);
				if (comp(ptr->value, k))
				{
					if (choice)
					{
						if (parent)
						{
							*parent = prev;
						}
						return link;
					}
					
					link = (_Node**)&ptr->right;
					prev = ptr;
					ptr = (_Node*)ptr->right;
				}
				else
				{
					if (!choice)
					{
						if (parent)
						{
							*parent = prev;
						}
						return link;
					}
					
					link = (_Node**)&ptr->left;
					prev = ptr;
					ptr = (_Node*)ptr->left;
				}
			}
			
			if (parent)
			{
				*parent = prev;
			}
			return link;
		}
		else
		{
			if (parent)
			{
				*parent = nullptr;
			}
			return &root;
		}
	}
	
	
	
	template <class T, class Comp, class Alloc>
	template <class... Args>
	void BinaryTree<T, Comp, Alloc>::create(_Node* parent, _Node*& branch, Args&&... args)
	{
		typedef typename node_allocator_type:: template rebind<_Node>::other _Node_Alloc;
		_Node_Alloc na;
		branch = na.allocate(1);
		nalloc.construct((NodeBase*)branch);
		alloc.template construct<T>(&branch->value, forward<Args>(args)...);
		
		NodeBase* it = parent;
		while (it != nullptr)
		{
			++it->size;
			it = it->parent;
		}
	}
	
	
	
	template <class T, class Comp, class Alloc>
	void BinaryTree<T, Comp, Alloc>::destroy(allocator_type& a)
	{
		if (root)
		{
			root->destroyChildren(nalloc, a);
			a.destroy(&root->value);
			nalloc.destroy((NodeBase*)root);
			nalloc.deallocate(root, 1);
			root = nullptr;
		}
	}
	
	template <class T, class Comp, class Alloc>
	size_t BinaryTree<T, Comp, Alloc>::size() const
	{
		return root != nullptr ? root->size : 0;
	}
	
	
}
}
}

#endif