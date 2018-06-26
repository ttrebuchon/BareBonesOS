#ifndef INCLUDED_BINARYTREE_HH
#define INCLUDED_BINARYTREE_HH

namespace Utils { namespace detail
{
	namespace binary_tree
	{
		template <class = void>
		class _NodeBase
		{
			public:
			size_t size;
			mutable _NodeBase *left, *right, *parent;
			_NodeBase();
		};
		
		typedef _NodeBase<void> NodeBase;
		
		template <class T, class Comp>
		class Node : public NodeBase
		{
			private:
			typedef Node<T, Comp> Self;
			
			public:
			
			T value;
			
			Node() = default;
			
			template <class... Args>
			Node(Args... args) : NodeBase(), value(Utils::forward<Args>(args)...)
			{}
			
			template <class NAlloc>
			void destroyChildren(NAlloc&);
			
			Node* sibling() noexcept
			{
				if (parent)
				{
					if (parent->left == this)
					{
						return (Node*)parent->right;
					}
					else if (parent->right == this)
					{
						return (Node*)parent->left;
					}
				}
				return nullptr;
			}
			
			const Node* sibling() const noexcept
			{
				if (parent)
				{
					if (parent->left == this)
					{
						return (Node*)parent->right;
					}
					else if (parent->right == this)
					{
						return (Node*)parent->left;
					}
				}
				return nullptr;
			}
			
			bool is_left() const noexcept
			{
				return (parent && parent->left == this);
			}
			
			bool is_right() const noexcept
			{
				return (parent && parent->right == this);
			}
			
			__attribute__((__always_inline__))
			Node* r() noexcept
			{
				return ((Node*)right);
			}
			
			__attribute__((__always_inline__))
			const Node* r() const noexcept
			{
				return ((const Node*)right);
			}
			
			__attribute__((__always_inline__))
			Node* l() noexcept
			{
				return ((Node*)left);
			}
			
			__attribute__((__always_inline__))
			const Node* l() const noexcept
			{
				return ((const Node*)left);
			}
			
			__attribute__((__always_inline__))
			Node* p() noexcept
			{
				return ((Node*)parent);
			}
			
			__attribute__((__always_inline__))
			const Node* p() const noexcept
			{
				return ((const Node*)parent);
			}
		};
		
		
		template <class T, class Comp, class Alloc = Allocator<T>>
		class BinaryTree
		{
			protected:
			typedef Node<T, Comp> _Node;
			typedef typename Alloc::template rebind<_Node>::other node_allocator_type;
			typedef Alloc allocator_type;
			
			_Node* root;
			Comp comp;
			allocator_type alloc;
			node_allocator_type nalloc;
			
			
			
			public:
			typedef _Node node;
			
			BinaryTree(const allocator_type& = allocator_type());
			
			~BinaryTree();
			
			template <class Key>
			_Node* find(const Key&) const;
			template <class Key>
			_Node** findCreate(const Key&, _Node** parent = nullptr);
			
			template <class... Args>
			void create(_Node* parent, _Node*& branch, Args&&...);
			
			BinaryTree* clone() const;
			void destroy(allocator_type&);
			
			size_t size() const;
			
			_Node* root_node() noexcept
			{
				return root;
			}
			
			const _Node* root_node() const noexcept
			{
				return root;
			}
			
			const node_allocator_type& get_node_allocator() const noexcept
			{ return nalloc; }
			
			const allocator_type& get_allocator() const noexcept
			{ return alloc; }
		};
	}
}
}
#endif