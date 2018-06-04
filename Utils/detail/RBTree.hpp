#ifndef INCLUDED_RBTREE_HPP
#define INCLUDED_RBTREE_HPP

#include "RBTree.hh"
#ifdef DEBUG_VERIFY
#include <iostreM>
#endif

namespace Utils { namespace detail
{
namespace rb_tree
{
	constexpr NodeBase::NodeBase() : color(Color::Red), size(1), left(nullptr), right(nullptr), parent(nullptr)
	{
		
	}
	
	/*constexpr Color NodeBase::color() const
	{
		return _color;
	}
	
	Color NodeBase::color(const Color c)
	{
		if (c != _color)
		{
			Color old = _color;
			if (old == Color::Black)
			{
				--bsize;
				++rsize;
			}
			else
			{
				--rsize;
				++bsize;
			}
			_color = c;
			NodeBase* it = parent;
			if (old == Color::Black)
			{
				while (it != nullptr)
				{
					--it->bsize;
					++it->rsize;
					it = it->parent;
				}
			}
			else
			{
				while (it != nullptr)
				{
					++it->bsize;
					--it->rsize;
					it = it->parent;
				}
			}
			return old;
		}
		else
		{
			return c;
		}
	}*/
	
	NodeBase* NodeBase::sibling()
	{
		if (parent)
		{
			if (parent->left == this)
			{
				return parent->right;
			}
			else
			{
				return parent->left;
			}
		}
		else
		{
			return nullptr;
		}
	}
	
	const NodeBase* NodeBase::sibling() const
	{
		if (parent)
		{
			if (parent->left == this)
			{
				return parent->right;
			}
			else
			{
				return parent->left;
			}
		}
		else
		{
			return nullptr;
		}
	}
	
	NodeBase* NodeBase::ancestor(size_t n)
	{
		NodeBase* it = this;
		while (n > 0)
		{
			if (it)
			{
				it = it->parent;
			}
			else
			{
				return nullptr;
			}
			--n;
		}
		return it;
	}
	
	const NodeBase* NodeBase::ancestor(size_t n) const
	{
		const NodeBase* it = this;
		while (n > 0)
		{
			if (it)
			{
				it = it->parent;
			}
			else
			{
				return nullptr;
			}
			--n;
		}
		return it;
	}
	
	bool NodeBase::rotateLeft()
	{
		NodeBase* rep = right;
		if (!rep)
		{
			return false;
		}
		
		if (parent)
		{
			if (parent->left == this)
			{
				parent->left = rep;
			}
			else if (parent->right == this)
			{
				parent->right = rep;
			}
			else
			{
				ASSERT(false);
			}
		}
		
		right = rep->left;
		rep->left = this;
		rep->parent = parent;
		parent = rep;
		
		if (right)
		{
			right->parent = this;
		}
		
		if (left)
		{
			left->parent = this;
		}
		
		size = 1;
		
		
		if (left)
		{
			size += left->size;
		}
		
		if (right)
		{
			size += right->size;
		}
		
		rep->size = size + 1;
		
		
		NodeBase* sib = sibling();
		if (sib)
		{
			rep->size += sib->size;
		}
		
		return true;
	}
	
	bool NodeBase::rotateRight()
	{
		NodeBase* rep = left;
		if (!rep)
		{
			return false;
		}
		
		if (parent)
		{
			if (parent->left == this)
			{
				parent->left = rep;
			}
			else if (parent->right == this)
			{
				parent->right = rep;
			}
			else
			{
				ASSERT(false);
			}
		}
		
		
		left = rep->right;
		rep->right = this;
		rep->parent = parent;
		parent = rep;
		
		if (right)
		{
			right->parent = this;
		}
		
		if (left)
		{
			left->parent = this;
		}
		
		size = 1;
		
		if (left)
		{
			size += left->size;
		}
		
		if (right)
		{
			size += right->size;
		}
		
		rep->size = size + 1;
		
		
		NodeBase* sib = sibling();
		if (sib)
		{
			rep->size += sib->size;
		}
		
		return true;
	}
	
	void NodeBase::rebalance()
	{
		if (!parent)
		{
			color = Color::Black;
		}
		else if (parent->color == Color::Black)
		{
			
		}
		else if (parent->sibling() != nullptr && parent->sibling()->color == Color::Red)
		{
			parent->color = Color::Black;
			auto uncle = parent->sibling();
			if (uncle)
			{
				uncle->color = Color::Black;
			}
			auto grandp = ancestor(2);
			if (grandp)
			{
				grandp->color = Color::Red;
				grandp->rebalance();
			}
		}
		else
		{
			auto grandp = ancestor(2);
			NodeBase* n = this;
			
			ASSERT(grandp != nullptr);
			//ASSERT(grandp->left != nullptr);
			//ASSERT(grandp->left->right == this || grandp->right != nullptr);
			if (grandp->left && grandp->left->right == this)
			{
				parent->rotateLeft();
				n = left;
			}
			else if (grandp->right && grandp->right->left == this)
			{
				parent->rotateRight();
				n = right;
			}
			
			ASSERT(n != nullptr);
			
			grandp = n->ancestor(2);
			ASSERT(grandp != nullptr);
			auto p = n->parent;
			ASSERT(p != nullptr);
			if (n == p->left)
			{
				grandp->rotateRight();
			}
			else
			{
				grandp->rotateLeft();
			}
			p->color = Color::Black;
			grandp->color = Color::Red;
		}
	}
	
	#ifdef DEBUG_VERIFY
	bool NodeBase::verify() const
	{
		bool res = true;
		if (left)
		{
			if (color == Color::Red)
			{
				res &= (left->color == Color::Black);
			}
			res &= left->verify();
		}
		
		if (res && right)
		{
			if (color == Color::Red)
			{
				res &= (right->color == Color::Black);
			}
			res &= right->verify();
		}
		
		bool countCorrect = (1 + (left ? left->size : 0) + (right ? right->size : 0) == size);
		if (!countCorrect)
		{
			std::clog << "(this, size, L, R): (" << this << ",\t" << size << ",\t" << (left ? left->size : 0) << ",\t" << (right ? right->size : 0) << "\t)\n";
		}
		ASSERT(countCorrect);
		res &= countCorrect;
		ASSERT(size > 0);
		
		bool parentHasChild = true;
		if (parent)
		{
			parentHasChild = (parent->left == this || parent->right == this);
		}
		ASSERT(parentHasChild);
		res &= parentHasChild;
		
		return res;
	}
	#endif
	
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
	
	template <class T, class Comp>
	template <class BNAlloc, class NAlloc, class Alloc>
	auto Node<T, Comp>::clone(BNAlloc& nalloc, NAlloc& na, Alloc& alloc) const -> Node*
	{
		Node* ptr = na.allocate(1);
		static_assert(sizeof(*na.allocate(1)) == sizeof(Node));
		nalloc.construct((NodeBase*)ptr);
		alloc.construct(&ptr->value, this->value);
		
		ptr->size = size;
		ptr->color = color;
		
		if (left)
		{
			ptr->left = ((Node*)left)->clone(nalloc, na, alloc);
			ptr->left->parent = ptr;
		}
		
		if (right)
		{
			ptr->right = ((Node*)right)->clone(nalloc, na, alloc);
			ptr->right->parent = ptr;
		}
		
		return ptr;
	}
	
	
	
	template <class T, class Comp, class Alloc>
	RBTree<T, Comp, Alloc>::RBTree() : _header(nullptr), comp(), alloc(), nalloc(), leftmost(nullptr)
	{
		_header = nalloc.allocate(1);
		nalloc.construct(_header);
		_header->left = _header;
		_header->right = _header;
		
		
	}
	
	template <class T, class Comp, class Alloc>
	RBTree<T, Comp, Alloc>::RBTree(const RBTree& rb) : _header(nullptr), comp(rb.comp), alloc(rb.alloc), nalloc(rb.nalloc), leftmost(nullptr)
	{
		
		if (!rb.root())
		{
			return;
		}
		typedef typename node_allocator_type:: template rebind<_Node>::other _Node_Alloc;
		_Node_Alloc na(nalloc);
		
		_header = nalloc.allocate(1);
		nalloc.construct(_header);
		_header->left = _header;
		_header->right = _header;
		
		root() = rb.root()->clone(nalloc, na, alloc);
		leftmost = root();
		while (leftmost->left)
		{
			leftmost = (_Node*)leftmost->left;
		}
		#ifdef DEBUG_VERIFY
		ASSERT(root()->verify());
		#endif
		
		
	}
	
	template <class T, class Comp, class Alloc>
	RBTree<T, Comp, Alloc>::~RBTree()
	{
		if (root())
		{
			destroy(alloc);
		}
		nalloc.destroy(_header);
		nalloc.deallocate(_header, 1);
	}
	
	
	
	
	
	
	
	template <class T, class Comp, class Alloc>
	template <class Key>
	auto RBTree<T, Comp, Alloc>::find(const Key& k) const -> const _Node*
	{
		if (root())
		{
			const _Node* it = root();
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
					
					it = (const _Node*)it->right;
				}
				else
				{
					if (!choice)
					{
						return it;
					}
					
					it = (const _Node*)it->left;
				}
			}
		}
		return nullptr;
	}
	
	template <class T, class Comp, class Alloc>
	template <class Key>
	auto RBTree<T, Comp, Alloc>::find(const Key& k) -> _Node*
	{
		if (root())
		{
			_Node* it = root();
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
	auto RBTree<T, Comp, Alloc>::findCreate(const Key& k, _Node** parent) -> _Node**
	{
		if (root())
		{
			_Node **link, *ptr, *prev;
			prev = root();
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
				return &root();
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
			#ifdef DEBUG_VERIFY
			for (int i = 10; i > 0; --i)
			{
				auto gp = prev->ancestor(i);
				if (gp)
				{
					gp->verify();
					break;
				}
			}
			#endif
			return link;
		}
		else
		{
			if (parent)
			{
				*parent = nullptr;
			}
			return &root();
		}
	}
	
	
	
	template <class T, class Comp, class Alloc>
	template <class... Args>
	auto RBTree<T, Comp, Alloc>::create(_Node* parent, _Node*& branch, Args&&... args) -> _Node*
	{
		typedef typename node_allocator_type:: template rebind<_Node>::other _Node_Alloc;
		_Node_Alloc na;
		branch = na.allocate(1);
		ASSERT(branch != nullptr);
		nalloc.construct((NodeBase*)branch);
		ASSERT(branch != nullptr);
		alloc.construct(&branch->value, forward<Args>(args)...);
		ASSERT(branch != nullptr);
		
		branch->parent = parent;
		branch->color = Color::Red;
		
		NodeBase* it = parent;
		while (it != nullptr)
		{
			++it->size;
			it = it->parent;
		}
		
		ASSERT(branch != nullptr);
		_Node* ret = branch;
		ASSERT(branch != nullptr);
		branch->rebalance();
		
		
		ASSERT(root() != nullptr);
		while (root()->parent != nullptr && root()->parent != _header)
		{
			root() = (_Node*)root()->parent;
		}
		
		if (!leftmost)
		{
			leftmost = root();
			while (leftmost->left)
			{
				leftmost = (_Node*)leftmost->left;
			}
			#ifdef DEBUG_VERIFY
			verify();
			#endif
		}
		else if (parent == leftmost)
		{
			if (leftmost->left)
			{
				leftmost = (_Node*)leftmost->left;
			}
			#ifdef DEBUG_VERIFY
			verify();
			#endif
		}
		
		
		if (branch != nullptr)
		{
			ASSERT(branch->size == 1 + (branch->right ? branch->right->size : 0) + (branch->left ? branch->left->size : 0));
			#ifdef DEBUG_VERIFY
			ASSERT(branch->verify());
			#endif
		}
		
		
		
		
		return ret;
	}
	
	template <class T, class Comp, class Alloc>
	template <class TreeAlloc>
	auto RBTree<T, Comp, Alloc>::clone(TreeAlloc talloc) const -> RBTree*
	{
		RBTree* ntree = talloc.allocate(1);
		if (root())
		{
			talloc.construct(ntree, *this);
		}
		else
		{
			talloc.construct(ntree);
		}
		return ntree;
	}
	
	template <class T, class Comp, class Alloc>
	void RBTree<T, Comp, Alloc>::destroy(allocator_type& a)
	{
		if (root())
		{
			root()->destroyChildren(nalloc, a);
			a.destroy(&root()->value);
			nalloc.destroy((NodeBase*)root());
			nalloc.deallocate(root(), 1);
			root() = nullptr;
			leftmost = nullptr;
		}
	}
	
	template <class T, class Comp, class Alloc>
	void RBTree<T, Comp, Alloc>::clear()
	{
		destroy(alloc);
	}
	
	template <class T, class Comp, class Alloc>
	template <class Key>
	auto RBTree<T, Comp, Alloc>::findErase(const Key& k, _Node** parent) -> _Node**
	{
		if (root())
		{
			_Node* n = find(k);
			if (n)
			{
				_Node** result = nullptr;
				if (n == root())
				{
					result = &root();
					if (parent)
					{
						*parent = (_Node*)(*result)->parent;
					}
				}
				else
				{
					assert(n->parent);
					if (parent)
					{
						*parent = (_Node*)n->parent;
					}
					if (n->parent->left == n)
					{
						result = &(_Node*&)*&n->parent->left;
					}
					else
					{
						result = &(_Node*&)*&n->parent->right;
					}
					assert(*result == n);
				}
				
				return result;
			}
		}
		
		
		if (parent)
		{
			*parent = nullptr;
		}
		return nullptr;
	}
	
	template <class T, class Comp, class Alloc>
	template <class Key>
	bool RBTree<T, Comp, Alloc>::erase(const Key& k)
	{
		//assert(NOT_IMPLEMENTED);
		if (root())
		{
			auto oldSize = size();
			root() = deleteNode(root(), k);
			recalcLeftmost();
			#ifdef DEBUG_VERIFY
			assert(verify());
			#endif
			return (oldSize != size());
		}
		
		return false;
		
		_Node* parent;
		_Node** br = findErase(k, &parent);
		if (!br)
		{
			return false;
		}
		
		_Node* n = *br;
		
		if (!n->left && !n->right)
		{
			*br = nullptr;
			if (leftmost == n)
			{
				leftmost = parent;
			}
		}
		else if (!n->left)
		{
			*br = (_Node*)n->right;
			n->right->parent = parent;
			if (leftmost == n)
			{
				_Node* it = (_Node*)n->right;
				_Node* tmp = nullptr;
				if (it)
				{
					tmp = (_Node*)it->left;
				}
				while (tmp)
				{
					it = tmp;
					tmp = (_Node*)tmp->left;
				}
				leftmost = it;
			}
		}
		else if (!n->right)
		{
			*br = (_Node*)n->left;
			n->left->parent = parent;
		}
		else
		{
			_Node* it = (_Node*)n->right;
			_Node* tmp = nullptr;
			tmp = (_Node*)it->left;
			while (tmp)
			{
				it = tmp;
				tmp = (_Node*)tmp->left;
			}
			
			*br = it;
			it->parent = parent;
		}
		
		alloc.destroy(&n->value);
		
		assert(false);
	}
	
	template <class T, class Comp, class Alloc>
	template <class Key>
	auto RBTree<T, Comp, Alloc>::deleteNode(_Node* base, const Key& k) -> _Node*
	{
		if (!base)
		{
			return base;
		}
		
		bool choice = comp(k, base->value);
		if (choice == comp(base->value, k))
		{
			if (!base->left)
			{
				_Node* tmp = (_Node*)base->right;
				alloc.destroy(&base->value);
				nalloc.destroy((NodeBase*)base);
				nalloc.deallocate(base, 1);
				return tmp;
			}
			else if (!base->right)
			{
				_Node* tmp = (_Node*)base->left;
				alloc.destroy(&base->value);
				nalloc.destroy((NodeBase*)base);
				nalloc.deallocate(base, 1);
				return tmp;
			}
			else
			{
				_Node* it = (_Node*)base->right;
				_Node* tmp = (_Node*)it->left;
				while (tmp)
				{
					it = tmp;
					tmp = (_Node*)tmp->left;
				}
				
				auto parent = base->parent;
				int side = 0;
				if (parent)
				{
					if (parent->left == base)
					{
						side = -1;
					}
					else
					{
						side = 1;
					}
				}
				while (base->rotateRight()) ;
				_Node* nBase = nullptr;
				if (side > 0)
				{
					nBase = (_Node*)parent->right;
				}
				else if (nBase < 0)
				{
					nBase = (_Node*)parent->left;
				}
				
				nBase = deleteNode(nBase, k);
				
				if (nBase)
				{
					nBase->rebalance();
				}
				
				#ifdef DEBUG_VERIFY
				if (nBase)
				{
				if (nBase->left)
				{
					assert(nBase->left->verify());
				}
				if (nBase->right)
				{
					assert(nBase->right->verify());
				}
				assert(nBase->verify());
				}
				#endif
				
				
				return nBase;
			}
		}
		else if (choice)
		{
			base->left = deleteNode((_Node*)base->left, k);
		}
		else
		{
			base->right = deleteNode((_Node*)base->right, k);
		}
		
		base->size = 1;
		if (base->left)
		{
			base->left->parent = base;
			base->size += base->left->size;
		}
		if (base->right)
		{
			base->right->parent = base;
			base->size += base->right->size;
		}
		
		return base;
	}
	
	template <class T, class Comp, class Alloc>
	bool RBTree<T, Comp, Alloc>::recalcLeftmost() noexcept
	{
		auto it = root();
		if (it)
		{
			auto tmp = (_Node*)it->left;
			while (tmp)
			{
				it = tmp;
				tmp = (_Node*)tmp->left;
			}
		}
		if (it != leftmost)
		{
			leftmost = it;
			return true;
		}
		
		return false;
	}
	
	template <class T, class Comp, class Alloc>
	size_t RBTree<T, Comp, Alloc>::size() const
	{
		return root() != nullptr ? root()->size : 0;
	}
	
	#ifdef DEBUG_VERIFY
	template <class T, class Comp, class Alloc>
	bool RBTree<T, Comp, Alloc>::verify() const
	{
		if (root())
		{
			bool val = root()->verify();
			
			if (!val)
			{
				std::cout << "Problem in nodes.\n";
				return val;
			}
			
			val &= (leftmost != nullptr);
			
			if (!val)
			{
				std::cout << "Leftmost should not be null\n";
				return val;
			}
			
			if (val)
			{
				const NodeBase* ptr = leftbound();
				const NodeBase* parent = ptr->parent;
				while (val && parent != nullptr)
				{
					val &= (parent->left == ptr);
					if (!val)
					{
						if (parent->right == ptr)
						{
							std::cout << "Node's parent should have it as left child.\n";
							return false;
						}
						std::cout << "Node's parent does not have it as child.\n";
						return false;
					}
					ptr = parent;
					parent = parent->parent;
				}
			}
			
			return val;
		}
		else
		{
			return true;
		}
	}
	#endif
}
}
}

#endif