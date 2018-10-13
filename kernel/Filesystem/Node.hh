#ifndef INCLUDED_FILESYSTEM_NODE_HH
#define INCLUDED_FILESYSTEM_NODE_HH

#include <Common.h>
#include "NodeType.hh"
#include <Utils/string>
#include "Path.hh"
#include <Utils/shared_ptr>
#include "node_ptr.hh"

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
	
	class Node : public Utils::enable_shared_from_this<Node>
	{
		
		private:
		static uint32_t counter;
		
		protected:
		mutable NodeType _type;
		DirectoryNode_v* _parent;
		Utils::string _name;
		
		Node();
		Node(const NodeType);
		
		mutable bool type_initted = false;
		//virtual void init_type() noexcept {}
		
		public:
		uint32_t inode;
		const Utils::string& name;
		
		virtual ~Node();
		
		virtual NodeType type() const noexcept;
		//const NodeType& type;

		virtual uint64_t read(uint64_t start, uint64_t len, void*) = 0;
		virtual uint64_t write(uint64_t, uint64_t, const void*) = 0;
		virtual void open() = 0;
		virtual void close() = 0;
		
		
		/*bool isKind(const NodeType n) const noexcept
		{
			return (type() & n) != 0;
		}*/
		
		bool isKind(const NodeType n, bool strict = false) const noexcept
		{
			if (unlikely(strict))
			{
				return (type() == n);
			}
			else
			{
				return (type() & n) == n;
			}
		}
		
		virtual void set_parent(DirectoryNode_v*);
		virtual DirectoryNode_v* get_parent() const noexcept
		{ return _parent; }
		
		virtual Filesystem* get_filesystem() const noexcept = 0;
		virtual Path_t get_path() const;
		
		node_ptr<> node_ptr_from_this();
		node_ptr<const Node> node_ptr_from_this() const;
		
		
		virtual const FileNode_v* as_file() const noexcept
		{ return nullptr; }
		virtual FileNode_v* as_file() noexcept
		{ return nullptr; }
		
		virtual const DirectoryNode_v* as_directory() const noexcept
		{ return nullptr; }
		virtual DirectoryNode_v* as_directory() noexcept
		{ return nullptr; }
		
		virtual const BlockDeviceNode* as_block_device() const noexcept
		{ return nullptr; }
		virtual BlockDeviceNode* as_block_device() noexcept
		{ return nullptr; }
		
		virtual const CharDeviceNode* as_char_device() const noexcept
		{ return nullptr; }
		virtual CharDeviceNode* as_char_device() noexcept
		{ return nullptr; }
		
		virtual const LinkNode* as_link() const noexcept
		{ return nullptr; }
		virtual LinkNode* as_link() noexcept
		{ return nullptr; }
		
		
		virtual const PipeNode* as_pipe() const noexcept
		{ return nullptr; }
		virtual PipeNode* as_pipe() noexcept
		{ return nullptr; }
		
		virtual const FunctionNode* as_function() const noexcept
		{ return nullptr; }
		virtual FunctionNode* as_function() noexcept
		{ return nullptr; }

    };




}
}
#endif