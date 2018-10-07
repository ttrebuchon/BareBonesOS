#include "node_ptr.hh"
#include "Node.hh"
#include "../Filesystem.hh"

namespace Kernel { namespace FS {
	
	node_ptr<Node>::node_ptr() : node_ptr(nullptr)
	{
		
	}
	
	node_ptr<Node>::node_ptr(decltype(nullptr) p) : _node(p)
	{
		
	}
	
	node_ptr<Node>::node_ptr(const spointer_type& ptr) : _node(ptr)
	{
		
	}
	
	node_ptr<Node>::node_ptr(const node_ptr& other) : _node(other._node)
	{
		
	}
	
	node_ptr<Node>::node_ptr(node_ptr&& other) : _node(Utils::move(other._node))
	{
		
	}
	
	
	
	
	node_ptr<FileNode_v> node_ptr<Node>::as_file() const noexcept
	{
		typedef FileNode_v ntype;
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
	
	


	node_ptr<DirectoryNode_v> node_ptr<Node>::as_directory() const noexcept
	{
		typedef DirectoryNode_v ntype;
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
	
	


	node_ptr<BlockDeviceNode> node_ptr<Node>::as_block_device() const noexcept
	{
		typedef BlockDeviceNode ntype;
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
	
	


	node_ptr<CharDeviceNode> node_ptr<Node>::as_char_device() const noexcept
	{
		typedef CharDeviceNode ntype;
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
	
	


	node_ptr<LinkNode> node_ptr<Node>::as_link() const noexcept
	{
		typedef LinkNode ntype;
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
	
	


	node_ptr<PipeNode> node_ptr<Node>::as_pipe() const noexcept
	{
		typedef PipeNode ntype;
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
	
	


	node_ptr<FunctionNode> node_ptr<Node>::as_function() const noexcept
	{
		typedef FunctionNode ntype;
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
	
	





	node_ptr<const FileNode_v> node_ptr<const Node>::as_file() const noexcept
	{
		typedef const FileNode_v ntype;
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
	
	


	node_ptr<const DirectoryNode_v> node_ptr<const Node>::as_directory() const noexcept
	{
		typedef const DirectoryNode_v ntype;
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
	
	


	node_ptr<const BlockDeviceNode> node_ptr<const Node>::as_block_device() const noexcept
	{
		typedef const BlockDeviceNode ntype;
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
	
	


	node_ptr<const CharDeviceNode> node_ptr<const Node>::as_char_device() const noexcept
	{
		typedef const CharDeviceNode ntype;
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
	
	


	node_ptr<const LinkNode> node_ptr<const Node>::as_link() const noexcept
	{
		typedef const LinkNode ntype;
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
	
	


	node_ptr<const PipeNode> node_ptr<const Node>::as_pipe() const noexcept
	{
		typedef const PipeNode ntype;
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
	
	


	node_ptr<const FunctionNode> node_ptr<const Node>::as_function() const noexcept
	{
		typedef const FunctionNode ntype;
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
	
	
	static void foo(const node_ptr<>& ptr)
	{
		auto lnk = ptr.as_link();
		if (lnk)
		{
			TRACE("Is a link.");
		}
		
		if (lnk == ptr)
		{
			TRACE("Is sane");
		}
		
		auto fi = lnk.as_file();
		if (fi)
		{
			TRACE("Is a file.");
		}
		else if (fi == nullptr)
		{
			TRACE("Not a file.");
		}
		else if (fi != nullptr)
		{
			TRACE("What?!");
		}
		
		if (fi == ptr)
		{
			TRACE("Is sane.");
		}
	}
	
	
	void test_node_ptr()
	{
		auto fs = Filesystem::Current;
		auto _r = fs->root();
		auto sr = Utils::shared_ptr<Node>(_r, [](auto r)
		{
			
		});
		
		node_ptr<> r(sr);
		assert(r);
		assert(r != nullptr);
		auto rd = r.as_directory();
		assert(rd);
		assert(rd != nullptr);
		assert(rd == r);
		
		node_ptr<> null;
		assert(!null);
		
		assert(null != r);
		assert(r != null);
		assert(!(r != rd));
		assert(rd != null);
		assert(null != rd);
		
		auto null_f = null.as_file().as_link().as_directory().as_pipe().as_file();
		assert(null_f == null);
	}
}
}