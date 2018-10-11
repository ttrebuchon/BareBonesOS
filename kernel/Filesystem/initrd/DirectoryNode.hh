#ifndef INCLUDED_FILESYSTEM_INITRD_DIRECTORYNODE_HH
#define INCLUDED_FILESYSTEM_INITRD_DIRECTORYNODE_HH

#include <Common.h>
#include <kernel/Filesystem/DirectoryNode.hh>
#include <Utils/vector>
#include "RefNode.hh"

namespace Kernel { namespace FS {

class InitRD_FS;

namespace Init_RD {

    class DirectoryNode : public FS::DirectoryNode, public RefNode
    {
        protected:
        InitRD_FS* fs;
        
        
        public:
        Utils::vector<node_ptr<>> children;
        DirectoryNode(InitRD_FS*, const char* name);
        ~DirectoryNode();


        virtual uint64_t read(uint64_t, uint64_t, void*) override;
        virtual uint64_t write(uint64_t, uint64_t, const void*) override;
        virtual void open() override;
        virtual void close() override;
        virtual DirEnt* readDir(uint32_t) override;
        virtual node_ptr<> findDir(const char* name) override;
        virtual node_ptr<Node> add(const node_ptr<>&) override;
        virtual bool remove(const node_ptr<>&) override;
        
        virtual size_t size() const noexcept override
        { return children.size(); }
        virtual node_ptr<> at(size_t index) const override
        { return children.at(index); }
        
        virtual Filesystem* get_filesystem() const noexcept override;
    };

}
}
}
#endif