#ifndef INCLUDED_FILESYSTEM_NODE_HH
#define INCLUDED_FILESYSTEM_NODE_HH

#include <Common.h>
#include "NodeType.hh"
#include <Utils/string>
#include "Path.hh"

namespace Kernel { namespace FS {
	
	class DirEnt;
	class DirectoryNode;
	class FileNode;
	
	class DirectoryNode_v;
	class FileNode_v;
	
	class BlockDeviceNode;
	class CharDeviceNode;
	class LinkNode;
	
	class Filesystem;
	
	class Node
	{
		
		private:
		static uint32_t counter;
		
		protected:
		mutable NodeType _type;
		DirectoryNode_v* parent;
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

		virtual uint64_t read(uint64_t start, uint64_t len, uint8_t*) = 0;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) = 0;
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
		{ return parent; }
		
		virtual Filesystem* get_filesystem() const noexcept = 0;
		virtual Path_t get_path() const;
		
		
		virtual const FileNode_v* as_file() const noexcept = 0;
		virtual FileNode_v* as_file() noexcept = 0;
		virtual const DirectoryNode_v* as_directory() const noexcept = 0;
		virtual DirectoryNode_v* as_directory() noexcept = 0;
		
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

    };




}
}
#endif