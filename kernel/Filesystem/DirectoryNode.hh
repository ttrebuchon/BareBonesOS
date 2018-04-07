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
    };
}
}
#endif