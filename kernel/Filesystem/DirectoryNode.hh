#ifndef INCLUDED_FILESYSTEM_DIRECTORYNODE_HH
#define INCLUDED_FILESYSTEM_DIRECTORYNODE_HH

#include <Common.h>
#include "Node.hh"

namespace Kernel { namespace FS {
	
	
	
	class DirectoryNode_v : public virtual Node
    {
        protected:

        public:
        DirectoryNode_v(const NodeType = NodeType::Directory);

        virtual void addChild(Node*) = 0;
        
        virtual Node* findChild(const Utils::string& relativePath);
        
        virtual size_t size() const noexcept = 0;
        virtual Node* at(size_t index) const = 0;
        virtual Node* at(const Utils::string& name) const;
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