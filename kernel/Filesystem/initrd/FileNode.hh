#ifndef INCLUDED_FILESYSTEM_INITRD_FILENODE_HH
#define INCLUDED_FILESYSTEM_INITRD_FILENODE_HH

#include <Common.h>
#include <kernel/Filesystem/FileNode.hh>

namespace Kernel { namespace Filesystem { namespace Init_RD {

    class FileNode : public Filesystem::FileNode
    {
        protected:
        size_t _size;
        void* _data;
        
        public:
        FileNode(const char* name, void* data, uint32_t size) noexcept;


        virtual uint32_t read(uint32_t, uint32_t, uint8_t*) override;
        virtual uint32_t write(uint32_t, uint32_t, const uint8_t*) override;
        virtual void open() override;
        virtual void close() override;
        virtual DirEnt* readDir(uint32_t) override;
        virtual Node* findDir(const char* name) override;
        
        virtual size_t size() const noexcept override;
    };

}
}
}
#endif