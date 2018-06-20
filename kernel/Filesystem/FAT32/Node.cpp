#include "Node.hh"

namespace Kernel::FS
{
	FAT32Node::FAT32Node(FAT32& fs, const NodeType kind, FAT32DirEnt_t* ent, DirectoryNode* _parent) : Node(kind), fs(&fs), cluster_no(), entity(ent)
	{
		assert(entity);
		assert(_parent);
		this->_name = entity->name;
		this->parent = _parent;
		
		if (entity->ext[0] != 0)
		{
			this->_name += FAT32::ExtensionSeparator;
			this->_name += entity->ext;
		}
	}
	
}