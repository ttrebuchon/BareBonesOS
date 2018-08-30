#ifndef INCLUDED_NTFSCHARDEVICENODE_HH
#define INCLUDED_NTFSCHARDEVICENODE_HH

#include "NTFSNode.hh"
#include <kernel/Filesystem/CharDeviceNode.hh>


namespace Kernel::FS
{
	typedef struct ___ntfs_intx_file_t ntfs_intx_file_t;
	
	
	class NTFSCharDeviceNode : public NTFSNode, public CharDeviceNode
	{
		public:
		
		protected:
		Utils::shared_ptr<ntfs_intx_file_t> intx;
		
		public:
		
		NTFSCharDeviceNode(NTFS*, ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		
		virtual void open() override;
		virtual void close() override;
		virtual DirectoryNode_v* get_parent() const noexcept override;
		
		/*virtual DeviceTarget* device() noexcept override;
		virtual const DeviceTarget* device() const noexcept override;*/
		
		virtual Filesystem* get_filesystem() const noexcept override;
		
		virtual Node* get_inode() noexcept
		{ return this; }
	};
	
}

#endif