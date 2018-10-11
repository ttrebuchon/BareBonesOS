#ifndef INCLUDED_FILESYSTEM_INITRD_LINKNODE_HH
#define INCLUDED_FILESYSTEM_INITRD_LINKNODE_HH

#include <Common.h>
#include <kernel/Filesystem/LinkNode.hh>
#include "RefNode.hh"
#include <kernel/Filesystem/node_ptr.hh>

namespace Kernel { namespace FS {

class InitRD_FS;

namespace Init_RD {

    class LinkNode : public FS::LinkNode, public RefNode
    {
        protected:
        InitRD_FS* fs;
        node_ptr<> _target;
        
        
        public:
        LinkNode(InitRD_FS*, const char* name, const node_ptr<>& target);
        
        ~LinkNode() = default;


        virtual node_ptr<> target() const noexcept override
        { return _target; }
        
        virtual Filesystem* get_filesystem() const noexcept override;
    };

}
}
}
#endif