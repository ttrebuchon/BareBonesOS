#include "NTFSCharDeviceNode.hh"
#include "NTFS.hh"
#include <kernel/Filesystem/Devices/DeviceTarget.hh>

namespace Kernel::FS
{
	
	
	NTFSCharDeviceNode::NTFSCharDeviceNode(NTFS* ntfs, ptr<ntfs_mft_record_t> mrec, ptr<ntfs_stdinfo_t> info) : NTFSNode(ntfs, mrec, info), CharDeviceNode()
	{
		if (mrec)
		{
			this->_name = get_name();
		}
	}
	
	
	void NTFSCharDeviceNode::open()
	{
		if (!_device)
		{
			if (!intx)
			{
				auto ctx = new detail::NTFS::attr_search_t(volume(), this_ptr(), nullptr);
				assert(ctx);
				
				if (ctx->lookup_attribute(NTFS_ATTR_TYPE_DATA, nullptr, 0, true, 0, nullptr, 0))
				{
					auto attr = ctx->attr;
					assert(attr);
					if (!attr->non_resident)
					{
						if (attr->value_len >= sizeof(ntfs_intx_file_t))
						{
							intx = Utils::shared_ptr<ntfs_intx_file_t>(attr, (ntfs_intx_file_t*)((uint8_t*)attr.get() + attr->value_off));
						}
					}
					else
					{
						assert(NOT_IMPLEMENTED);
					}
				}
				
				delete ctx;
			}
			if (intx)
			{
				auto dev_id = (dev_t)intx->dev;
				auto target = DeviceTarget::find_target(volume(), DeviceTargetType::Char, dev_id);
				_device = target;
			}
			else
			{
				assert(NOT_IMPLEMENTED);
			}
		}
	}
	
	void NTFSCharDeviceNode::close()
	{
		intx = nullptr;
		CharDeviceNode::close();
	}
	
	DirectoryNode_v* NTFSCharDeviceNode::get_parent() const noexcept
	{
		auto ntfs_n = this->open_parent();
		if (ntfs_n)
		{
			auto n = ntfs_n->get_inode();
			if (n)
			{
				return n->as_directory();
			}
		}
		return nullptr;
	}
	
	Filesystem* NTFSCharDeviceNode::get_filesystem() const noexcept
	{
		return volume();
	}
}