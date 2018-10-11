#include "LinkNode.hh"
#include <kernel/Filesystem/initrd.hh>

namespace Kernel { namespace FS { namespace Init_RD {
	
	LinkNode::LinkNode(InitRD_FS* fs, const char* name, const node_ptr<>& target) : fs(fs), _target(target)
	{
		assert(target);
		this->_name = name;
	}
	
	Filesystem* LinkNode::get_filesystem() const noexcept
	{
		return fs;
	}
	
}
}
}