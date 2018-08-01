#include "NodeFactory.hh"


namespace Kernel::FS
{
	
	LinkNode* Factory::create_link(DirectoryNode_v* parent, const Utils::string& name, const Node* target) noexcept
	{
		if (!parent || !target)
		{
			return nullptr;
			
		}
		if (target->get_parent() == parent)
		{
			return create_link(parent, name, target->name);
		}
		else
		{
			return create_link(parent, name, target->get_path().str());
		}
	}
	
}