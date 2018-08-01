#ifndef INCLUDED_FILESYSTEM_DEVICENODE_HH
#define INCLUDED_FILESYSTEM_DEVICENODE_HH

#include <Common.h>
#include "Node.hh"





namespace Kernel { namespace FS {
	
	class DeviceTarget;
	
	
	class DeviceNode : public virtual Node
	{
		protected:
		DeviceTarget* _device;
		
		
		DeviceNode(DeviceTarget* device, const Utils::string& name, const NodeType);
		DeviceNode(DeviceTarget* dev, const NodeType);
		DeviceNode(DeviceTarget* device, const Utils::string& name);
		DeviceNode(DeviceTarget* dev);
		
		
		public:
		
		
		virtual ~DeviceNode() = default;
		
		
		virtual DeviceTarget* device() noexcept;
		virtual const DeviceTarget* device() const noexcept;
		virtual bool has_device() const noexcept;
		
		
		virtual const FileNode_v* as_file() const noexcept override
		{ return nullptr; }
		virtual FileNode_v* as_file() noexcept override
		{ return nullptr; }
		virtual const DirectoryNode_v* as_directory() const noexcept override
		{ return nullptr; }
		virtual DirectoryNode_v* as_directory() noexcept override
		{ return nullptr; }
	};
	
}
}

#endif