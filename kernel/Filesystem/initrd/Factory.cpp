#include "Factory.hh"
#include <kernel/Filesystem/initrd.hh>
#include "LinkNode.hh"
#include "RAMFile.hh"

namespace Kernel { namespace FS { namespace Init_RD {
	
	
	node_ptr<FileNode_v> Factory::create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept
	{
		auto file = new RAMFileNode(fs, name.c_str());
		//parent->add(file);
		//file->set_parent(parent);
		return node_ptr<FileNode_v>(Utils::wrap_shared(file));
		
	}
	
	
	node_ptr<FS::LinkNode> Factory::create_link(DirectoryNode_v* parent, const Utils::string& name, const node_ptr<const Node>& target) noexcept
	{
		auto link = new LinkNode(fs, name.c_str(), (node_ptr<>)target);
		//auto link = new LinkNode(fs, name.c_str(), const_cast<FS::Node*>(target));
		//parent->add(link);
		//link->set_parent(parent);
		return node_ptr<FS::LinkNode>(Utils::wrap_shared(link));
	}
	
	node_ptr<FS::LinkNode> Factory::create_link(DirectoryNode_v* parent, const Utils::string& name, const Utils::string& target) noexcept
	{
		assert(fs);
		auto targetn = fs->getNode(target);
		assert(targetn);
		return create_link(parent, name, targetn);
	}
	
	
}
}
}