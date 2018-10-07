#ifndef INCLUDED_FILESYSTEM_INITRD_FUNCTIONNODE_HH
#define INCLUDED_FILESYSTEM_INITRD_FUNCTIONNODE_HH

#include <Common.h>
#include <kernel/Filesystem/FunctionNode.hh>
#include "RefNode.hh"

namespace Kernel { namespace FS { 

class InitRD_FS;

namespace Init_RD {

    class FunctionNode : public FS::FunctionNode, public RefNode
    {
    	public:
        constexpr static size_t function_name_max= 255;
        
        protected:
        char fn_name[function_name_max];
        InitRD_FS* fs;
        
        public:
        FunctionNode(InitRD_FS*, const char* name, void* data, uint64_t size) noexcept;
        
        virtual uint64_t read(uint64_t, uint64_t, void*) override;
        virtual uint64_t write(uint64_t, uint64_t, const void*) override;
        virtual void open() override;
        virtual void close() override;
        
        virtual const char* function() const noexcept override;
        
        size_t size() const noexcept;
        
        virtual Filesystem* get_filesystem() const noexcept override;
    };

}
}
}
#endif