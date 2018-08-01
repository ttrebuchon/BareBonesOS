#ifndef INCLUDED_FILESYSTEM_LINKNODE_HH
#define INCLUDED_FILESYSTEM_LINKNODE_HH

#include <Common.h>
#include "Node.hh"


namespace Kernel { namespace FS {
	
	
	
	class LinkNode : public virtual Node
	{
		protected:
		
		
		LinkNode(const NodeType);
		
		public:
		LinkNode();
		virtual ~LinkNode() = default;
		
		virtual Node* target() const noexcept = 0;
		
		virtual NodeType type() const noexcept override;
		
		virtual uint64_t read(uint64_t start, uint64_t len, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		
		virtual const FileNode_v* as_file() const noexcept override;
		virtual FileNode_v* as_file() noexcept override;
		
		virtual const DirectoryNode_v* as_directory() const noexcept override;
		virtual DirectoryNode_v* as_directory() noexcept override;
		
		virtual const BlockDeviceNode* as_block_device() const noexcept override;
		virtual BlockDeviceNode* as_block_device() noexcept override;
		
		virtual const LinkNode* as_link() const noexcept
		{ return this; }
		virtual LinkNode* as_link() noexcept
		{ return this; }
		
	};
	
}
}
#endif