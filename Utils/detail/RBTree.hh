#ifndef INCLUDED_RBTREE_HH
#define INCLUDED_RBTREE_HH

#include <Common.h>
#include <Utils/Allocator.hh>

namespace Utils { namespace detail
{
	namespace rb_tree
	{
		enum struct Color
		{
			Red,
			Black
		};
		
		class NodeBase
		{
			private:
			
			
			public:
			Color color;
			size_t size;
			mutable NodeBase *left, *right, *parent;
			
			
			constexpr NodeBase();
			
			/*constexpr Color color() const;
			Color color(const Color);*/
			
			inline NodeBase* sibling();
			inline const NodeBase* sibling() const;
			inline NodeBase* ancestor(size_t n);
			inline const NodeBase* ancestor(size_t n) const;
			
			inline bool rotateLeft();
			inline bool rotateRight();
			inline void rebalance();
			
			/*constexpr size_t size() const
			{
				return rsize + bsize;
			}*/
			
			#ifdef DEBUG_VERIFY
			inline bool verify() const;
			#endif
		};
		
		template <class T, class Comp>
		class Node : public NodeBase
		{
			private:
			typedef Node<T, Comp> Self;
			
			public:
			Node() = delete;
			Node(const Node&) = delete;
			Node(Node&&) = delete;
			
			T value;
			
			template <class NAlloc, class Alloc>
			void destroyChildren(NAlloc&, Alloc&);
			
			template <class BaseAlloc, class ThisAlloc, class ValueAlloc>
			Node* clone(BaseAlloc&, ThisAlloc&, ValueAlloc&) const;
		};
		
		
		template <class T, class Comp, class Alloc = Utils::allocator<T>>
		class RBTree
		{
			protected:
			typedef typename Alloc::template rebind<NodeBase>::other node_allocator_type;
			typedef Alloc allocator_type;
			typedef Node<T, Comp> _Node;
			
			NodeBase* _header;
			Comp comp;
			mutable allocator_type alloc;
			mutable node_allocator_type nalloc;
			_Node* leftmost;
			
			
			
			public:
			typedef _Node node;
			typedef const _Node* const_node_ptr;
			
			RBTree();
			RBTree(const allocator_type&, const node_allocator_type& = node_allocator_type());
			RBTree(const RBTree&);
			
			~RBTree();
			
			template <class Key>
			const _Node* find(const Key&) const;
			template <class Key>
			_Node* find(const Key&);
			template <class Key>
			_Node** findCreate(const Key&, _Node** parent = nullptr);
			
			template <class... Args>
			_Node* create(_Node* parent, _Node*& branch, Args&&...);
			
			template <class TreeAlloc>
			RBTree* clone(TreeAlloc) const;
			void destroy(allocator_type&);
			void clear();
			
			template <class Key>
			_Node** findErase(const Key&, _Node** parent);
			template <class Key>
			bool erase(const Key&);
			
			template <class Key>
			_Node* deleteNode(_Node* base, const Key& k);
			bool recalcLeftmost() noexcept;
			
			size_t size() const;
			
			#ifdef DEBUG_VERIFY
			bool verify() const;
			#endif
			
			inline _Node* leftbound()
			{
				return leftmost;
			}
			
			inline const _Node* leftbound() const
			{
				return leftmost;
			}
			
			_Node*& root()
			{
				return *(_Node**)(void**)&_header->parent;
			}
			
			const const_node_ptr& root() const
			{
				typedef const void* cvoid;
				
				return *(const const_node_ptr*)(const cvoid*)(&_header->parent);
			}
			
			inline NodeBase* endNode()
			{
				return nullptr;
				return _header;
			}
			
			inline const NodeBase* endNode() const
			{
				return nullptr;
				return _header;
			}
		};
		
		template <class N>
		N iterator_increment(N node)
		{
			auto sNode = node;
			if (node->right)
			{
				node = static_cast<N>(node->right);
				while (node->left)
				{
					node = static_cast<N>(node->left);
				}
			}
			else
			{
				N parent = static_cast<N>(node->parent);
				while (parent != nullptr && node == parent->right)
				{
					node = parent;
					parent = static_cast<N>(node->parent);
				}
				if (node->right != parent)
				{
					node = parent;
					assert(node == parent);
				}
			}
			if (node == sNode)
			{
				node = nullptr;
			}
			return node;
		}
	}
}
}
#endif