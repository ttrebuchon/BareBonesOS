#ifndef INCLUDED_FILESYSTEM_INITRD_LINKNODE_HH
#define INCLUDED_FILESYSTEM_INITRD_LINKNODE_HH

#include <Common.h>
#include <kernel/Filesystem/LinkNode.hh>
#include "RefNode.hh"

namespace Kernel { namespace FS {

class InitRD_FS;

namespace Init_RD {

    class LinkNode : public FS::LinkNode, public RefNode
    {
        protected:
        InitRD_FS* fs;
        FS::Node* _target;
        
        
        public:
        LinkNode(InitRD_FS*, const char* name, FS::Node* target);
        
        ~LinkNode() = default;


        virtual Node* target() const noexcept override
        { return _target; }
        
        virtual Filesystem* get_filesystem() const noexcept override;
    };

}
}
}
#endif