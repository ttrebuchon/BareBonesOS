#ifndef INCLUDED_FILESYSTEM_DEVICENODE_HH
#define INCLUDED_FILESYSTEM_DEVICENODE_HH

#include <Common.h>
#include "Node.hh"
#include "Devices/DeviceTarget.hh"




namespace Kernel { namespace FS {
	
	class DeviceTarget;
	
	
	class DeviceNode : public virtual Node
	{
		protected:
		Utils::shared_ptr<DeviceTarget> _device;
		
		
		DeviceNode(const Utils::shared_ptr<DeviceTarget>& device, const Utils::string& name, const NodeType);
		DeviceNode(const Utils::shared_ptr<DeviceTarget>& dev, const NodeType);
		DeviceNode(const Utils::shared_ptr<DeviceTarget>& device, const Utils::string& name);
		DeviceNode(const Utils::shared_ptr<DeviceTarget>& dev);
		
		
		public:
		
		
		virtual ~DeviceNode() = default;
		
		
		virtual Utils::shared_ptr<DeviceTarget> device() noexcept;
		virtual Utils::shared_ptr<const DeviceTarget> device() const noexcept;
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