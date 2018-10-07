#include "NTFSNode.hh"
#include "NTFS.hh"
#include <Std/errno.h>
#include "NTFSAttribute.hh"
#include <Utils/algorithm>


namespace Kernel::FS
{
	NTFSNode::NTFSNode(NTFS* ntfs, ptr<ntfs_mft_record_t> mrec, ptr<ntfs_stdinfo_t> info) :
				ntfs(ntfs),
				mft_no(0),
				mft_rec(mrec),
				state(0),
				flags((ntfs_file_attr_flags)0),
				_info(info),
				_data(nullptr)
	{
		if (mft_rec)
		{
			mft_no = mft_rec->mft_record_num;
			state |= 1;
		}
	}
	
	
	uint64_t NTFSNode::n_read(uint64_t start, uint64_t len, void* buf)
	{
		if (!len)
		{
			return 0;
		}
		assert(buf);
		
		if (!_data)
		{
			if (unlikely(!init_data()))
			{
				//tset_error(EIO, "Couldn't find NTFS inode data.");
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			#ifdef DEBUG
			else
			{
				assert(_data);
			}
			#endif
		}
		
		size_t left;
		auto buf_end = (void*)((uint8_t*)buf + len);
		do
		{
			auto blk = _data->read(start, &left);
			if (!blk || !left)
			{
				break;
			}
			
			auto sect = Utils::min<size_t>((size_t)left, (size_t)((uintptr_t)buf_end - (uintptr_t)buf));
			assert(sect);
			
			memcpy(buf, blk.get(), sect);
			buf = (uint8_t*)buf + sect;
			start += sect;
		}
		while (buf < buf_end);
		
		return len - ((uintptr_t)buf_end - (uintptr_t)buf);
	}
	
	uint64_t NTFSNode::n_write(uint64_t, uint64_t, const void*)
	{
		if (!_data)
		{
			if (unlikely(!init_data()))
			{
				//tset_error(EIO, "Couldn't find NTFS inode data.");
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			#ifdef DEBUG
			else
			{
				assert(_data);
			}
			#endif
		}
		
		assert(NOT_IMPLEMENTED);
	}
	
	size_t NTFSNode::n_size() const noexcept
	{
		if (!_data)
		{
			if (unlikely(!init_data()))
			{
				//tset_error(EIO, "Couldn't find NTFS inode data.");
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			#ifdef DEBUG
			else
			{
				assert(_data);
			}
			#endif
		}
		
		return _data->size();
		
		/*if (unlikely(_data_attr->non_resident))
		{
			assert(NOT_IMPLEMENTED);
		}
		else
		{
			return _data_attr->value_len;
		}*/
	}
	
	bool NTFSNode::set_weak(Utils::shared_ptr<NTFSNode> ptr)
	{
		if (ptr)
		{
			__weak_self = ptr;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	bool NTFSNode::init_data() noexcept
	{
		detail::NTFS::attr_search_t ctx(volume(), this_ptr(), nullptr);
		auto attr = ctx.open_attribute(NTFS_ATTR_TYPE_DATA);
		if (!attr)
		{
			return false;
		}
		
		this->_data = attr;
		
		return true;
	}
	
	Utils::string NTFSNode::get_name()
	{
		auto fname = get_name_raw();
		if (!fname)
		{
			return "";
		}
		
		assert(fname->filename_len > 0);
		
		auto cstr = detail::NTFS::ntfs_str_to_char((ntfs_char_t*)fname->filename, fname->filename_len);
		if (!cstr)
		{
			return "";
		}
		else
		{
			Utils::string str(cstr);
			delete[] cstr;
			assert(str.length() == fname->filename_len);
			return str;
		}
	}
	
	Utils::basic_string<ntfs_char_t> NTFSNode::get_uni_name()
	{
		auto fname = get_name_raw();
		if (!fname)
		{
			return Utils::basic_string<ntfs_char_t>();
		}
		
		
		assert(NOT_IMPLEMENTED);
	}
	
	Utils::shared_ptr<ntfs_filename_attr_t> NTFSNode::get_name_raw()
	{
		assert(mft_rec);
		auto ctx = Utils::make_shared<detail::NTFS::attr_search_t>(ntfs, nullptr, mft_rec);
		assert(ctx);
		
		if (ctx->lookup_attribute(NTFS_ATTR_TYPE_FILE_NAME, nullptr, 0, true, 0, nullptr, 0))
		{
			auto attr = ctx->attr;
			assert(attr);
			assert(!(attr->flags & NTFS_ATTR_COMPRESSED));
			assert(!(attr->flags & NTFS_ATTR_ENCRYPTED));
			if (!attr->non_resident)
			{
				assert(attr->value_len >= sizeof(ntfs_filename_attr_t));
				auto fname = (ntfs_filename_attr_t*)((uint8_t*)attr.get() + attr->value_off);
				return Utils::shared_ptr<ntfs_filename_attr_t>(attr, fname);
			}
			else
			{
				assert(NOT_IMPLEMENTED);
			}
		}
		else
		{
			return nullptr;
		}
	}
	
	uint64_t NTFSNode::get_parent_mft() const noexcept
	{
		if (has_parent_mft)
		{
			return cached_parent_mft;
		}
		
		auto cthis = const_cast<NTFSNode*>(this);
		auto fname = cthis->get_name_raw();
		if (!fname)
		{
			return 0;
		}
		
		cached_parent_mft = fname->parent_dir;
		has_parent_mft = true;
		
		return fname->parent_dir;
	}
	
	Utils::shared_ptr<NTFSNode> NTFSNode::open_parent() const
	{
		auto v = volume();
		if (unlikely(!v))
		{
			return nullptr;
		}
		
		return v->inode_open(get_parent_mft());
	}
}