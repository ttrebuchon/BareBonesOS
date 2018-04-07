#include "Filesystem.hh"
#include "Node.hh"
#include "DirectoryNode.hh"
#include "Path.hh"

namespace Kernel { namespace Filesystem {
	
	Filesystem* Filesystem::Current = nullptr;
	
	Node* Filesystem::getNode(const Utils::string& path, const Utils::string& relative)
	{
		if (path.length() <= 0)
		{
			return nullptr;
		}
		
		if (!Path::IsAbsolute(path))
		{
			if (relative.empty())
			{
				return nullptr;
			}
			return getNode(Path::Combine(path, relative));
		}
		
		ASSERT(root);
		ASSERT(root->isKind(NodeType::Directory));
		
		const auto droot = (DirectoryNode*)root;
		
		/*auto index = Path::IndexForRelativeToRoot(path);
		ASSERT(path.length() > index);
		const char* cstr = path.c_str();*/
		
		return droot->findChild(Path::MakeRelative(path, Path::Root));
	}
	
}
}