#ifndef INCLUDED_FILESYSTEM_INITRD_FACTORY_HH
#define INCLUDED_FILESYSTEM_INITRD_FACTORY_HH

#include <Common.h>
#include <kernel/Filesystem/ReadOnlyFactory.hh>

namespace Kernel { namespace FS { 

class InitRD_FS;

namespace Init_RD {

    class Factory : public FS::ReadOnlyFactory
    {
    	protected:
    	InitRD_FS* fs;
    	
    	
    	public:
    	Factory(InitRD_FS* fs) : fs(fs)
    	{
    		
    	}
        
        virtual FileNode_v* create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept override;
        
        virtual LinkNode* create_link(DirectoryNode_v* parent, const Utils::string&, const Node* target) noexcept;
        virtual LinkNode* create_link(DirectoryNode_v* parent, const Utils::string& name, const Utils::string& target) noexcept override;
        
    };

}
}
}
#endif