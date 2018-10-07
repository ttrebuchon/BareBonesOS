#include "Factory.hh"
#include <kernel/Filesystem/initrd.hh>
#include "LinkNode.hh"
#include "RAMFile.hh"

namespace Kernel { namespace FS { namespace Init_RD {
	
	
	FileNode_v* Factory::create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept
	{
		auto file = new RAMFileNode(fs, name.c_str());
		parent->add(file);
		file->set_parent(parent);
		return file;
		
	}
	
	
	FS::LinkNode* Factory::create_link(DirectoryNode_v* parent, const Utils::string& name, const Node* target) noexcept
	{
		auto link = new LinkNode(fs, name.c_str(), const_cast<FS::Node*>(target));
		parent->add(link);
		link->set_parent(parent);
		return link;
	}
	
	FS::LinkNode* Factory::create_link(DirectoryNode_v* parent, const Utils::string& name, const Utils::string& target) noexcept
	{
		assert(fs);
		auto targetn = fs->getNode(target);
		assert(targetn);
		return create_link(parent, name, targetn);
	}
	
	
}
}
}