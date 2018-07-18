#ifndef INCLUDED_FILESYSTEM_BLOCKDEVICENODE_HH
#define INCLUDED_FILESYSTEM_BLOCKDEVICENODE_HH

#include <Common.h>
#include "Node.hh"
#include <Utils/mutex>

namespace Drivers
{
	class Disk;
}


namespace Kernel { namespace FS {
	
	
	
	class BlockDeviceNode : public virtual Node
	{
		protected:
		Drivers::Disk* _device;
		
		BlockDeviceNode(Drivers::Disk* device, const NodeType);
		BlockDeviceNode(const NodeType);
		BlockDeviceNode(Drivers::Disk* device, const Utils::string& name, const NodeType);
		BlockDeviceNode(const Utils::string& name, const NodeType);
		
		public:
		BlockDeviceNode(Drivers::Disk* disk);
		BlockDeviceNode();
		BlockDeviceNode(Drivers::Disk* device, const Utils::string& name);
		BlockDeviceNode(const Utils::string& name);
		virtual ~BlockDeviceNode() = default;
		
		
		virtual Drivers::Disk* device() noexcept;
		virtual const Drivers::Disk* device() const noexcept;
		virtual Drivers::Disk* mount(Drivers::Disk*) noexcept;
		virtual bool has_device() const noexcept;
		
		
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		virtual DirEnt* readDir(uint32_t) override;
		virtual Node* findDir(const char* name) override;
		
		
		virtual const FileNode_v* as_file() const noexcept override
		{ return nullptr; }
		virtual FileNode_v* as_file() noexcept override
		{ return nullptr; }
		virtual const DirectoryNode_v* as_directory() const noexcept override
		{ return nullptr; }
		virtual DirectoryNode_v* as_directory() noexcept override
		{ return nullptr; }
		
		virtual const BlockDeviceNode* as_block_device() const noexcept
		{ return this; }
		virtual BlockDeviceNode* as_block_device() noexcept
		{ return this; }
	};
	
}
}

#endif