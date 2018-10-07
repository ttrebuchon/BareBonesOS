#ifndef INCLUDED_FILESYSTEM_NODE_PTR_HH
#define INCLUDED_FILESYSTEM_NODE_PTR_HH

#include <Common.h>
#include "Node.hh"
#include <Utils/shared_ptr>

namespace Kernel { namespace FS {
	
	class DirEnt;
	class DirectoryNode;
	class FileNode;
	
	class DirectoryNode_v;
	class FileNode_v;
	
	class BlockDeviceNode;
	class CharDeviceNode;
	class LinkNode;
	class PipeNode;
	class FunctionNode;
	
	class Filesystem;
	class Node;
	
	template <class T = Node>
	class node_ptr;
	
	
	namespace detail::node_ptr
	{
		template <class T>
		struct is_const
		{
			constexpr static bool value = false;
		};
		
		template <class T>
		struct is_const<const T>
		{
			constexpr static bool value = true;
		};
		
		template <class T, class Y>
		struct apply_const_if_const
		{
			typedef Y type;
		};
		
		template <class T, class Y>
		struct apply_const_if_const<const T, Y>
		{
			typedef const Y type;
		};
	}
	
	
	
	template <>
	class node_ptr<Node>
	{
		public:
		typedef Node node_type;
		typedef node_type* pointer_type;
		typedef Utils::shared_ptr<node_type> spointer_type;
		
		
		protected:
		spointer_type _node;
		
		public:
		node_ptr();
		node_ptr(decltype(nullptr));
		node_ptr(const spointer_type&);
		node_ptr(const node_ptr&);
		node_ptr(node_ptr&&);
		
		
		operator bool() const noexcept __attribute__((always_inline))
		{
			return (bool)_node;
		}
		
		pointer_type operator->() const noexcept
		{
			return _node.operator->();
		}
		
		node_ptr& operator=(const node_ptr& rhs)
		{
			_node = rhs._node;
			return *this;
		}
		
		node_ptr& operator=(node_ptr&& rhs)
		{
			_node = Utils::move(rhs._node);
			return *this;
		}
		
		node_ptr<FileNode_v> as_file() const noexcept;
		node_ptr<DirectoryNode_v> as_directory() const noexcept;
		node_ptr<BlockDeviceNode> as_block_device() const noexcept;
		node_ptr<CharDeviceNode> as_char_device() const noexcept;
		node_ptr<LinkNode> as_link() const noexcept;
		node_ptr<PipeNode> as_pipe() const noexcept;
		node_ptr<FunctionNode> as_function() const noexcept;
		
		
		/*
			Comparison Operators
		*/
		bool __attribute__((always_inline)) operator==(const node_ptr& rhs) const noexcept
		{ return _node.get() == rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator==(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() == rhs._node.get(); }
		bool __attribute__((always_inline)) operator==(decltype(nullptr)) const noexcept
		{ return _node == nullptr; }
		bool __attribute__((always_inline)) operator!=(const node_ptr& rhs) const noexcept
		{ return _node.get() != rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator!=(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() != rhs._node.get(); }
		bool __attribute__((always_inline)) operator!=(decltype(nullptr)) const noexcept
		{ return _node != nullptr; }
		bool __attribute__((always_inline)) operator<(const node_ptr& rhs) const noexcept
		{ return _node.get() < rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator<(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() < rhs._node.get(); }
		bool __attribute__((always_inline)) operator<(decltype(nullptr)) const noexcept
		{ return _node.get() < nullptr; }
		bool __attribute__((always_inline)) operator<=(const node_ptr& rhs) const noexcept
		{ return _node.get() <= rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator<=(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() <= rhs._node.get(); }
		bool __attribute__((always_inline)) operator<=(decltype(nullptr)) const noexcept
		{ return _node.get() <= nullptr; }
		bool __attribute__((always_inline)) operator>(const node_ptr& rhs) const noexcept
		{ return _node.get() > rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator>(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() > rhs._node.get(); }
		bool __attribute__((always_inline)) operator>(decltype(nullptr)) const noexcept
		{ return _node.get() > nullptr; }
		bool __attribute__((always_inline)) operator>=(const node_ptr& rhs) const noexcept
		{ return _node.get() >= rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator>=(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() >= rhs._node.get(); }
		bool __attribute__((always_inline)) operator>=(decltype(nullptr)) const noexcept
		{ return _node.get() >= nullptr; }
		
		template <class>
		friend class node_ptr;
	};
	
	
	
	
	template <>
	class node_ptr<const Node>
	{
		public:
		typedef const Node node_type;
		typedef node_type* pointer_type;
		typedef Utils::shared_ptr<node_type> spointer_type;
		
		
		protected:
		spointer_type _node;
		
		public:
		node_ptr();
		node_ptr(decltype(nullptr));
		node_ptr(const spointer_type&);
		node_ptr(const node_ptr&);
		node_ptr(node_ptr&&);
		
		operator bool() const noexcept
		{
			return (bool)_node;
		}
		
		pointer_type operator->() const noexcept
		{
			return _node.operator->();
		}
		
		
		node_ptr<const FileNode_v> as_file() const noexcept;
		node_ptr<const DirectoryNode_v> as_directory() const noexcept;
		node_ptr<const BlockDeviceNode> as_block_device() const noexcept;
		node_ptr<const CharDeviceNode> as_char_device() const noexcept;
		node_ptr<const LinkNode> as_link() const noexcept;
		node_ptr<const PipeNode> as_pipe() const noexcept;
		node_ptr<const FunctionNode> as_function() const noexcept;
		
		
		/*
			Comparison Operators
		*/
		bool __attribute__((always_inline)) operator==(const node_ptr& rhs) const noexcept
		{ return _node.get() == rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator==(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() == rhs._node.get(); }
		bool __attribute__((always_inline)) operator==(decltype(nullptr)) const noexcept
		{ return _node == nullptr; }
		bool __attribute__((always_inline)) operator!=(const node_ptr& rhs) const noexcept
		{ return _node.get() != rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator!=(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() != rhs._node.get(); }
		bool __attribute__((always_inline)) operator!=(decltype(nullptr)) const noexcept
		{ return _node != nullptr; }
		bool __attribute__((always_inline)) operator<(const node_ptr& rhs) const noexcept
		{ return _node.get() < rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator<(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() < rhs._node.get(); }
		bool __attribute__((always_inline)) operator<(decltype(nullptr)) const noexcept
		{ return _node.get() < nullptr; }
		bool __attribute__((always_inline)) operator<=(const node_ptr& rhs) const noexcept
		{ return _node.get() <= rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator<=(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() <= rhs._node.get(); }
		bool __attribute__((always_inline)) operator<=(decltype(nullptr)) const noexcept
		{ return _node.get() <= nullptr; }
		bool __attribute__((always_inline)) operator>(const node_ptr& rhs) const noexcept
		{ return _node.get() > rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator>(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() > rhs._node.get(); }
		bool __attribute__((always_inline)) operator>(decltype(nullptr)) const noexcept
		{ return _node.get() > nullptr; }
		bool __attribute__((always_inline)) operator>=(const node_ptr& rhs) const noexcept
		{ return _node.get() >= rhs._node.get(); }
		template <class Y>
		bool __attribute__((always_inline)) operator>=(const node_ptr<Y>& rhs) const noexcept
		{ return _node.get() >= rhs._node.get(); }
		bool __attribute__((always_inline)) operator>=(decltype(nullptr)) const noexcept
		{ return _node.get() >= nullptr; }
		
		
		template <class>
		friend class node_ptr;
	};
	
	
	template <class T>
	class node_ptr
	{
		public:
		typedef T node_type;
		typedef node_type* pointer_type;
		typedef Utils::shared_ptr<node_type> spointer_type;
		
		private:
		spointer_type _node;
		
		template <class Y>
		using const_mod = typename detail::node_ptr::apply_const_if_const<node_type, Y>::type;
		
		protected:
		
		
		public:
		node_ptr();
		node_ptr(decltype(nullptr));
		node_ptr(const spointer_type&);
		node_ptr(const node_ptr&);
		node_ptr(node_ptr&&);
		
		
		operator bool() const noexcept
		{
			return (bool)_node;
		}
		
		pointer_type operator->() const noexcept
		{
			return _node.operator->();
		}
		
		node_ptr<const_mod<FileNode_v>> as_file() const noexcept;
		node_ptr<const_mod<DirectoryNode_v>> as_directory() const noexcept;
		node_ptr<const_mod<BlockDeviceNode>> as_block_device() const noexcept;
		node_ptr<const_mod<CharDeviceNode>> as_char_device() const noexcept;
		node_ptr<const_mod<LinkNode>> as_link() const noexcept;
		node_ptr<const_mod<PipeNode>> as_pipe() const noexcept;
		node_ptr<const_mod<FunctionNode>> as_function() const noexcept;
		
		/*
			Comparison Operators
		*/
		bool operator==(const node_ptr& rhs) const noexcept;
		template <class Y>
		bool operator==(const node_ptr<Y>&) const noexcept;
		bool operator==(decltype(nullptr)) const noexcept;
		bool operator!=(const node_ptr& rhs) const noexcept;
		template <class Y>
		bool operator!=(const node_ptr<Y>&) const noexcept;
		bool operator!=(decltype(nullptr)) const noexcept;
		bool operator<(const node_ptr& rhs) const noexcept;
		template <class Y>
		bool operator<(const node_ptr<Y>&) const noexcept;
		bool operator<(decltype(nullptr)) const noexcept;
		bool operator<=(const node_ptr& rhs) const noexcept;
		template <class Y>
		bool operator<=(const node_ptr<Y>&) const noexcept;
		bool operator<=(decltype(nullptr)) const noexcept;
		bool operator>(const node_ptr& rhs) const noexcept;
		template <class Y>
		bool operator>(const node_ptr<Y>&) const noexcept;
		bool operator>(decltype(nullptr)) const noexcept;
		bool operator>=(const node_ptr& rhs) const noexcept;
		template <class Y>
		bool operator>=(const node_ptr<Y>&) const noexcept;
		bool operator>=(decltype(nullptr)) const noexcept;
		
		
		template <class>
		friend class node_ptr;

	};




}
}

#include "node_ptr.hpp"

#endif