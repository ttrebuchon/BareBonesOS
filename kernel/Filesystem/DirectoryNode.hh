#ifndef INCLUDED_FILESYSTEM_DIRECTORYNODE_HH
#define INCLUDED_FILESYSTEM_DIRECTORYNODE_HH

#include <Common.h>
#include "Node.hh"

namespace Kernel { namespace FS {
	
	class Factory;
	class DeviceTarget;
	
	class DirectoryNode_v : public virtual Node
    {
        protected:

        public:
        DirectoryNode_v(const NodeType = NodeType::Directory);
        
        virtual Node* findChild(const Utils::string& relativePath);
        virtual DirEnt* readDir(uint32_t);
		virtual Node* findDir(const char* name);
        
        virtual size_t size() const noexcept = 0;
        virtual Node* at(size_t index) const = 0;
        virtual Node* at(const Utils::string& name) const;
        
        virtual FileNode_v* add_file(const Utils::string& name);
        virtual DirectoryNode_v* add_directory(const Utils::string& name);
        virtual BlockDeviceNode* add_block_device(const Utils::string&, DeviceTarget* = nullptr);
        virtual CharDeviceNode* add_char_device(const Utils::string&, DeviceTarget* = nullptr);
        virtual LinkNode* add_link(const Utils::string& name, const Node* target);
        virtual LinkNode* add_link(const Utils::string& name, const Path_t&);
        virtual LinkNode* add_link(const Utils::string& name, const Utils::string& target);
        virtual Node* add(Node*) = 0;
        
        virtual Node* find_node(const Path_t&);
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