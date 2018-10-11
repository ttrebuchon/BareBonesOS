#ifndef INCLUDED_FILESYSTEM_NODEFACTORY_HH
#define INCLUDED_FILESYSTEM_NODEFACTORY_HH

#include <Common.h>
#include "Node.hh"
#include "Devices/DeviceTarget.hh"
#include "node_ptr.hh"

namespace Kernel::FS
{
	
	class LinkNode;
	
	class Factory
	{
		
		public:
		
		
		protected:
		
		
		public:
		
		virtual node_ptr<DirectoryNode_v> create_directory(DirectoryNode_v* parent, const Utils::string& name) noexcept = 0;
		virtual node_ptr<FileNode_v> create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept = 0;
		virtual node_ptr<LinkNode> create_link(DirectoryNode_v* parent, const Utils::string&, const node_ptr<const Node>& target) noexcept;
		virtual node_ptr<LinkNode> create_link(DirectoryNode_v* parent, const Utils::string&, const Utils::string& target) noexcept = 0;
		virtual node_ptr<BlockDeviceNode> create_block_device(DirectoryNode_v* parent, const Utils::string&, const devtarget_t&) noexcept = 0;
		virtual node_ptr<CharDeviceNode> create_char_device(DirectoryNode_v* parent, const Utils::string&, const devtarget_t&) noexcept = 0;
		
		
	};
	
}

#endif