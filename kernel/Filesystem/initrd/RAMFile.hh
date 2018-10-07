#ifndef INCLUDED_FILESYSTEM_INITRD_RAMFILE_HH
#define INCLUDED_FILESYSTEM_INITRD_RAMFILE_HH

#include <Common.h>
#include <kernel/Filesystem/FileNode.hh>
#include "RefNode.hh"

namespace Kernel { namespace FS { 

class InitRD_FS;

namespace Init_RD {

    class RAMFileNode : public FS::FileNode, public RefNode
    {
        protected:
        size_t _size;
        uint8_t* _data;
        InitRD_FS* fs;
        
        void resize(uint64_t);
        
        public:
        RAMFileNode(InitRD_FS*, const char* name) noexcept;
        ~RAMFileNode();


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