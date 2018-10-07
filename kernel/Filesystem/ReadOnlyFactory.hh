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
		
		virtual LinkNode* create_link(DirectoryNode_v* parent, const Utils::string&, const Utils::string&) noexcept override
		{
			return nullptr;
		}
		
		virtual BlockDeviceNode* create_block_device(DirectoryNode_v* parent, const Utils::string&, const devtarget_t&) noexcept override
		{
			return nullptr;
		}
		
		virtual CharDeviceNode* create_char_device(DirectoryNode_v* parent, const Utils::string&, const devtarget_t&) noexcept override
		{
			return nullptr;
		}
	};
	
}

#endif