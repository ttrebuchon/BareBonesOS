#ifndef INCLUDED_FILESYSTEM_INITRD_FILENODE_HH
#define INCLUDED_FILESYSTEM_INITRD_FILENODE_HH

#include <Common.h>
#include <kernel/Filesystem/FileNode.hh>
#include "RefNode.hh"

namespace Kernel { namespace FS { 

class InitRD_FS;

namespace Init_RD {

    class FileNode : public FS::FileNode, public RefNode
    {
        protected:
        size_t _size;
        void* _data;
        InitRD_FS* fs;
        
        public:
        FileNode(InitRD_FS*, const char* name, void* data, uint64_t size) noexcept;


        virtual uint64_t read(uint64_t, uint64_t, void*) override;
        virtual uint64_t write(uint64_t, uint64_t, const void*) override;
        virtual void open() override;
        virtual void close() override;
        
        virtual size_t size() const noexcept override;
        
        virtual Filesystem* get_filesystem() const noexcept override;
    };

}
}
}
#endif