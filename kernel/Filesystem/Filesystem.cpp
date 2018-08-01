#include "Filesystem.hh"
#include "Node.hh"
#include "DirectoryNode.hh"
#include "Path.hh"

namespace Kernel { namespace FS {
	
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
				return getNode(Path::Combine(Path::Root, path));
				return nullptr;
			}
			return getNode(Path::Combine(relative, path));
		}
		
		auto root = this->root();
		
		ASSERT(root);
		ASSERT(root->isKind(NodeType::Directory));
		
		const auto droot = root->as_directory();
		
		/*auto index = Path::IndexForRelativeToRoot(path);
		ASSERT(path.length() > index);
		const char* cstr = path.c_str();*/
		
		
		return droot->findChild(Path::MakeRelative(path, Path::Root));
	}
	
	Node* Filesystem::getNode(const Path_t& path)
	{
		if (path.empty())
		{
			return nullptr;
		}
		
		auto root = this->root();
		assert(root);
		auto it = root->as_directory();
		const auto droot = it;
		assert(droot);
		
		for (size_t i = 0; i < path.parts_length() - 1 && it; ++i)
		{
			auto next = it->at(path.part(i));
			if (next)
			{
				it = next->as_directory();
			}
			else
			{
				return nullptr;
			}
		}
		
		if (it)
		{
			return it->at(path.back());
		}
		
		return nullptr;
	}
	
	DirectoryNode_v* Filesystem::rootd() const noexcept
	{
		auto r = root();
		if (r)
		{
			return r->as_directory();
		}
		else
		{
			return nullptr;
		}
	}
	
}
}