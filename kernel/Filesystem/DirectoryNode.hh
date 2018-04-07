#ifndef INCLUDED_FILESYSTEM_DIRECTORYNODE_HH
#define INCLUDED_FILESYSTEM_DIRECTORYNODE_HH

#include <Common.h>
#include "Node.hh"

namespace Kernel { namespace Filesystem {

    class DirectoryNode : public Node
    {
        protected:

        public:
        DirectoryNode(const NodeType = NodeType::Directory);

        virtual void addChild(Node*) = 0;
        
        virtual Node* findChild(const Utils::string& relativePath);
        
        virtual size_t size() const noexcept = 0;
        virtual Node* at(size_t index) const = 0;
        virtual Node* at(const Utils::string& name) const;
    };
}
}
#endif