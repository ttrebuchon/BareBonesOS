#ifndef INCLUDED_FILESYSTEM_DIRECTORYNODE_HH
#define INCLUDED_FILESYSTEM_DIRECTORYNODE_HH

#include <Common.h>
#include "Node.hh"
#include <Utils/shared_ptr>
#include "node_ptr.hh"

namespace Kernel { namespace FS {
	
	class Factory;
	class DeviceTarget;
	
	class DirectoryNode_v : public Node
    {
		protected:

		public:
		DirectoryNode_v(const NodeType = NodeType::Directory);
		
		virtual node_ptr<> findChild(const Utils::string& relativePath);
		virtual DirEnt* readDir(uint32_t);
		virtual node_ptr<> findDir(const char*);
		
		virtual size_t size() const noexcept = 0;
		virtual node_ptr<> at(size_t index) const = 0;
		virtual node_ptr<> at(const Utils::string& name) const;
		
		
		
		virtual node_ptr<FileNode_v> add_file(const Utils::string& name);
		virtual node_ptr<DirectoryNode_v> add_directory(const Utils::string& name);
		virtual node_ptr<BlockDeviceNode> add_block_device(const Utils::string&, const Utils::shared_ptr<DeviceTarget>& = nullptr);
		virtual node_ptr<CharDeviceNode> add_char_device(const Utils::string&, const Utils::shared_ptr<DeviceTarget>& = nullptr);
		virtual node_ptr<LinkNode> add_link(const Utils::string& name, const node_ptr<const Node>&);
		virtual node_ptr<LinkNode> add_link(const Utils::string& name, const Path_t&);
		virtual node_ptr<LinkNode> add_link(const Utils::string& name, const Utils::string& target);
		virtual node_ptr<Node> add(const node_ptr<>&) = 0;
		virtual bool has_child(const node_ptr<>&) const;
		
		
		
		
		virtual bool remove(const Utils::string& name);
		virtual bool remove(size_t i);
		virtual bool remove(const node_ptr<>&) = 0;
		template <class T>
		bool remove(const node_ptr<T>& n)
		{
			return remove((node_ptr<>)n);
		}
		
		virtual node_ptr<> find_node(const Path_t&);
    };
	

    class DirectoryNode : public DirectoryNode_v
    {
		protected:

		public:
		DirectoryNode(const NodeType = NodeType::Directory);
		
		
		
		
		
		
		virtual const FileNode_v* as_file() const noexcept override
		{ return nullptr; }
		virtual FileNode_v* as_file() noexcept override
		{ return nullptr; }
		
		virtual const DirectoryNode* as_directory() const noexcept override
		{ return this; }
		virtual DirectoryNode* as_directory() noexcept override
		{ return this; }
    };
}
}
#endif