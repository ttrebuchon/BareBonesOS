#ifndef INCLUDED_FILESYSTEM_NODEFACTORY_HH
#define INCLUDED_FILESYSTEM_NODEFACTORY_HH

#include <Common.h>
#include "Node.hh"

namespace Kernel::FS
{
	
	class LinkNode;
	
	class Factory
	{
		
		public:
		
		
		protected:
		
		
		public:
		
		virtual DirectoryNode_v* create_directory(DirectoryNode_v* parent, const Utils::string& name) noexcept = 0;
		virtual FileNode_v* create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept = 0;
		virtual LinkNode* create_link(DirectoryNode_v* parent, const Utils::string&, Node* target) noexcept = 0;
		
		
	};
	
}

#endif