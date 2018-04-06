#include "FileNode.hh"

namespace Kernel { namespace Filesystem
{
	
	FileNode::FileNode(const NodeType t) : Node(t | NodeType::File)
	{
		
	}
	
	FileNode::FileNode() : FileNode(NodeType::File)
	{
		
	}
	
	FileNode::~FileNode()
	{
		// TODO
		//Utils::unique_lock<mutex> lock(lock_m);
		if (file != nullptr)
		{
			// TODO
			//file->sync();
			delete file;
		}
	}
}
}