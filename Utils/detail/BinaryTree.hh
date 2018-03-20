#ifndef INCLUDED_BINARYTREE_HH
#define INCLUDED_BINARYTREE_HH

namespace Utils { namespace detail
{
	namespace binary_tree
	{
		class NodeBase
		{
			public:
			size_t size;
			mutable NodeBase *left, *right, *parent;
			NodeBase();
		};
		
		template <class T, class Comp>
		class Node : public NodeBase
		{
			private:
			typedef Node<T, Comp> Self;
			
			public:
			
			T value;
			
			template <class NAlloc, class Alloc>
			void destroyChildren(NAlloc&, Alloc&);
		};
		
		
		template <class T, class Comp, class Alloc = Allocator<T>>
		class BinaryTree
		{
			protected:
			typedef typename Alloc::template rebind<NodeBase>::other node_allocator_type;
			typedef Alloc allocator_type;
			typedef Node<T, Comp> _Node;
			
			_Node* root;
			Comp comp;
			allocator_type alloc;
			node_allocator_type nalloc;
			
			
			
			public:
			typedef _Node node;
			
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
		};
	}
}
}
#endif