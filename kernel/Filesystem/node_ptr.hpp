#ifndef INCLUDED_FILESYSTEM_NODE_PTR_HPP
#define INCLUDED_FILESYSTEM_NODE_PTR_HPP

#include "node_ptr.hh"

namespace Kernel { namespace FS {
	
	template <class T>
	node_ptr<T>::node_ptr() : _node(nullptr)
	{
		
	}
	
	template <class T>
	node_ptr<T>::node_ptr(decltype(nullptr)) : node_ptr()
	{
		
	}
	
	template <class T>
	node_ptr<T>::node_ptr(const spointer_type& ptr) : _node(ptr)
	{
		
	}
	
	template <class T>
	node_ptr<T>::node_ptr(const node_ptr& other) : node_ptr(other._node)
	{
		
	}
	
	template <class T>
	node_ptr<T>::node_ptr(node_ptr&& other) : _node(Utils::move(other._node))
	{
		
	}
	
	
	
	
	
	template <class T>
	node_ptr<T>& node_ptr<T>::operator=(const node_ptr& rhs)
	{
		_node = rhs._node;
		return *this;
	}
	
	template <class T>
	node_ptr<T>& node_ptr<T>::operator=(node_ptr&& rhs)
	{
		_node = Utils::move(rhs._node);
		return *this;
	}
	
	template <class T>
	node_ptr<T>::operator node_ptr<>() const
	{
		return this->template cast<Node>();
	}
	
	
	
	
	
	template <class T>
	node_ptr<typename detail::node_ptr::apply_const_if_const<T, FileNode_v>::type> node_ptr<T>::as_file() const noexcept
	{
		typedef typename detail::node_ptr::apply_const_if_const<T, FileNode_v>::type ntype;
		if (_node)
		{
			auto other = _node->as_file();
			if (other)
			{
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other));
			}
		}
		return node_ptr<ntype>();
	}
	
	


	template <class T>
	node_ptr<typename detail::node_ptr::apply_const_if_const<T, DirectoryNode_v>::type> node_ptr<T>::as_directory() const noexcept
	{
		typedef typename detail::node_ptr::apply_const_if_const<T, DirectoryNode_v>::type ntype;
		if (_node)
		{
			auto other = _node->as_directory();
			if (other)
			{
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other));
			}
		}
		return node_ptr<ntype>();
	}
	
	template <class T>
	node_ptr<typename detail::node_ptr::apply_const_if_const<T, BlockDeviceNode>::type> node_ptr<T>::as_block_device() const noexcept
	{
		typedef typename detail::node_ptr::apply_const_if_const<T, BlockDeviceNode>::type ntype;
		if (_node)
		{
			auto other = _node->as_block_device();
			if (other)
			{
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other));
			}
		}
		return node_ptr<ntype>();
	}
	
	

	template <class T>
	node_ptr<typename detail::node_ptr::apply_const_if_const<T, CharDeviceNode>::type> node_ptr<T>::as_char_device() const noexcept
	{
		typedef typename detail::node_ptr::apply_const_if_const<T, CharDeviceNode>::type ntype;
		if (_node)
		{
			auto other = _node->as_char_device();
			if (other)
			{
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other));
			}
		}
		return node_ptr<ntype>();
	}
	
	

	template <class T>
	node_ptr<typename detail::node_ptr::apply_const_if_const<T, LinkNode>::type> node_ptr<T>::as_link() const noexcept
	{
		typedef typename detail::node_ptr::apply_const_if_const<T, LinkNode>::type ntype;
		if (_node)
		{
			auto other = _node->as_link();
			if (other)
			{
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other));
			}
		}
		return node_ptr<ntype>();
	}
	
	

	template <class T>
	node_ptr<typename detail::node_ptr::apply_const_if_const<T, PipeNode>::type> node_ptr<T>::as_pipe() const noexcept
	{
		typedef typename detail::node_ptr::apply_const_if_const<T, PipeNode>::type ntype;
		if (_node)
		{
			auto other = _node->as_pipe();
			if (other)
			{
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other));
			}
		}
		return node_ptr<ntype>();
	}
	
	

	template <class T>
	node_ptr<typename detail::node_ptr::apply_const_if_const<T, FunctionNode>::type> node_ptr<T>::as_function() const noexcept
	{
		typedef typename detail::node_ptr::apply_const_if_const<T, FunctionNode>::type ntype;
		if (_node)
		{
			auto other = _node->as_function();
			if (other)
			{
				return node_ptr<ntype>(typename node_ptr<ntype>::spointer_type(_node, other));
			}
		}
		return node_ptr<ntype>();
	}


	
	
	
	
	
	
	
	
	/* Comparison Operators */
	
	#define COMP_OP_DEF(OP) \
	template <class T> \
	bool node_ptr<T>::operator OP (const node_ptr& rhs) const noexcept \
	{ \
		return _node.get() OP rhs._node.get(); \
	} \
	\
	template <class T> \
	template <class Y> \
	bool node_ptr<T>::operator OP (const node_ptr<Y>& rhs) const noexcept \
	{ \
		return _node.get() OP rhs._node.get(); \
	} \
	\
	template <class T> \
	bool node_ptr<T>::operator OP (decltype(nullptr)) const noexcept \
	{ \
		return _node.get() OP nullptr; \
	}
	
	COMP_OP_DEF(==)
	COMP_OP_DEF(!=)
	COMP_OP_DEF(>)
	COMP_OP_DEF(>=)
	COMP_OP_DEF(<)
	COMP_OP_DEF(<=)
	
	#undef COMP_OP_DEF
}
}
#endif