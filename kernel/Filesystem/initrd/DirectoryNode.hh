#ifndef INCLUDED_FILESYSTEM_INITRD_DIRECTORYNODE_HH
#define INCLUDED_FILESYSTEM_INITRD_DIRECTORYNODE_HH

#include <Common.h>
#include <kernel/Filesystem/DirectoryNode.hh>
#include <kernel/utils/List.hh>

namespace Kernel { namespace Filesystem { namespace Init_RD {

    class DirectoryNode : public Filesystem::DirectoryNode
    {
        protected:
        
        
        public:
        Utils::List<Node*> children;
        DirectoryNode(const char* name);


        virtual uint32_t read(uint32_t, uint32_t, uint8_t*) override;
        virtual uint32_t write(uint32_t, uint32_t, uint8_t*) override;
        virtual void open() override;
        virtual void close() override;
        virtual DirEnt* readDir(uint32_t) override;
        virtual Node* findDir(const char* name) override;
        virtual void addChild(Node*) override;
    };

}
}
}
#endif