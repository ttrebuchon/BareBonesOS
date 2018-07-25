#ifndef INCLUDED_FILESYSTEM_READONLYFACTORY_HH
#define INCLUDED_FILESYSTEM_READONLYFACTORY_HH

#include "NodeFactory.hh"

namespace Kernel::FS
{
	
	class ReadOnlyFactory : public Factory
	{
		
		public:
		
		
		protected:
		
		
		public:
		
		virtual DirectoryNode_v* create_directory(DirectoryNode_v* parent, const Utils::string& name) noexcept override
		{
			return nullptr;
		}
		
		virtual FileNode_v* create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept override
		{
			return nullptr;
		}
		
		virtual LinkNode* create_link(DirectoryNode_v* parent, const Utils::string&, Node* target) noexcept override
		{
			return nullptr;
		}
		
		
	};
	
}

#endif