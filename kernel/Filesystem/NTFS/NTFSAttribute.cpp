#include "NTFSAttribute.hh"
#include <Std/errno.h>
#include "NTFS.hh"
#include "NTFSNode.hh"

namespace Kernel::FS
{
	NTFSAttribute::NTFSAttribute(Utils::shared_ptr<NTFSNode> n, const ntfs_attr_type type, ntfs_char_t* name, uint32_t name_len) :
			_node(n),
			_record(nullptr),
			_name(name),
			name_len(name_len),
			_state(),
			_type(type),
			li(nullptr)
	{
		
	}
	
	Utils::shared_ptr<NTFSNode> NTFSAttribute::node() const noexcept
	{
		return _node.lock();
	}
	
	bool NTFSAttribute::encrypted() const noexcept
	{
		return ((flags() & NTFS_ATTR_ENCRYPTED) != 0);
	}
	
	bool NTFSAttribute::sparse() const noexcept
	{
		return ((flags() & NTFS_ATTR_SPARSE) != 0);
	}
	
	bool NTFSAttribute::compressed() const noexcept
	{
		return ((flags() & NTFS_ATTR_COMPRESSED) != 0);
	}
	
	bool NTFSAttribute::non_resident() const noexcept
	{
		return record()->non_resident != 0;
	}
	
	auto NTFSAttribute::flags() const noexcept -> flags_type
	{
		auto r = record();
		if (likely(r != nullptr))
		{
			return (flags_type)r->flags;
		}
		else
		{
			return 0;
		}
	}
	
	
	
	void NTFSAttribute::init(Utils::shared_ptr<ntfs_attr_record_t> attr) noexcept
	{
		if ((state() & NTFSAttributeState::Initialized) == 0)
		{
			_record = attr;
			
			
			
			
			_state |= NTFSAttributeState::Initialized;
			
			//assert(NOT_IMPLEMENTED);
		}
	}
	
	Utils::shared_ptr<uint8_t> NTFSAttribute::read(const size_t off, size_t* size_left_in_block)
	{
		if (((bool)(record()->flags & NTFS_ATTR_COMPRESSION_MASK)) && ((state() & NTFSAttributeState::NonResident) != 0))
		{
			if ((record()->flags & NTFS_ATTR_COMPRESSION_MASK) == NTFS_ATTR_COMPRESSED)
			{
				return read_compressed(off, size_left_in_block);
			}
			else
			{
				tset_error(EOPNOTSUPP, "Compression mode isn't supported");
				return nullptr;
			}
		}
		
		NTFS* ntfs = nullptr;
		auto n = node();
		assert(n);
		if (n)
		{
			ntfs = n->volume();
			assert(ntfs);
		}
		if (!ntfs)
		{
			assert(NOT_IMPLEMENTED);
		}
		
		auto max_init = record()->initialized_size;
		auto max_read = record()->data_size;
		
		if (encrypted() && non_resident())
		{
			assert(NOT_IMPLEMENTED);
		}
		
		if (!non_resident())
		{
			max_read = record()->value_len;
		}
		
		
		
		if (off >= max_read)
		{
			tset_error(EINVAL, "attr read offset is out of range.");
			return nullptr;
		}
		
		
		if (!non_resident())
		{
			auto ctx = Utils::wrap_shared(ntfs->attr_search(n, nullptr));
			assert(ctx);
			if (!ctx)
			{
				return nullptr;
			}
			size_t _name_len = name_len;
			if (!ctx->lookup_attribute(type(), _name, _name_len, false, 0, nullptr, 0))
			{
				return nullptr;
			}
			
			assert(ctx->attr == record());
			
			auto val = Utils::shared_ptr<uint8_t>(ctx->attr, (uint8_t*)ctx->attr.get() + ctx->attr->value_off);
			
			if (val.get() < (uint8_t*)ctx->attr.get() || (val.get() + ctx->attr->value_len) > (uint8_t*)ctx->mrec.get() + ntfs->mft_rec_size)
			{
				tset_error(EIO, "Value offset was negative or too far");
				return nullptr;
			}
			
			/*if (ctx->attr->value_len < off)
			{
				TRACE("Provided offset was outside of the attr value length.");
				TRACE_VAL(ctx->attr->value_len);
				TRACE_VAL(off);
				set_error(EINVAL, "Provided offset was outside of the attr value length.");
				return nullptr;
			}*/
			
			val = Utils::shared_ptr<uint8_t>(val, val.get() + off);
			
			if (size_left_in_block)
			{
				*size_left_in_block = max_read - off;
				//*size_left_in_block = ((uint8_t*)ctx->mrec.get() + ntfs->mft_rec_size) - val.get();
			}
			
			return val;
		}
		
		if (encrypted() && (off > max_init-2))
		{
			assert(NOT_IMPLEMENTED);
		}
		
		auto vcn = off / ntfs->cluster_size();
		auto rl = this->find_vcn(vcn);
		if (!rl)
		{
			
			assert(NOT_IMPLEMENTED);
		}
		
		auto lcn = rl->lcn;
		assert(lcn);
		
		using namespace detail::NTFS;
		
		if (lcn == NTFS_LCN_LI_NOT_MAPPED)
		{
			assert(NOT_IMPLEMENTED);
		}
		
		auto blk = ntfs->get_block(lcn + vcn - rl->vcn);
		assert(blk);
		
		
		auto roff = off - vcn*ntfs->cluster_size();
		auto raw = blk->data + roff;
		if (size_left_in_block)
		{
			*size_left_in_block = blk->len - roff;
		}
		return Utils::shared_ptr<uint8_t>(blk, raw);
		
		assert(NOT_IMPLEMENTED);
	}
	
	size_t NTFSAttribute::size() const noexcept
	{
		if (non_resident())
		{
			assert(NOT_IMPLEMENTED);
		}
		else
		{
			return record()->value_len;
		}
	}
	
	
	
	
	
	
	
	
	
	int NTFSAttribute::map_runlist(int64_t vcn)
	{
		using namespace detail::NTFS;
		auto n = node();
		NTFS* ntfs = nullptr;
		if (n)
		{
			ntfs = n->volume();
		}
		
		assert(ntfs);
		if (!ntfs)
		{
			tset_error(EINVAL, "Couldn't get NTFS volume in attribute");
			return -1;
		}
		
		assert(record());
		assert(record()->mapping_pairs_offset > 0);
		
		auto lcn = runlist_vcn_to_lcn(vcn);
		if (lcn >= 0 || lcn == NTFS_LCN_HOLE || lcn == NTFS_LCN_ENOENT)
		{
			return 0;
		}
		
		/*{
			auto r = record();
			assert(r);
			auto raw = (uint8_t*)r.get() + r->mapping_pairs_offset;
			auto off = Utils::shared_ptr<uint8_t>(r, raw);
			TRACE_VAL((void*)off.get());
			return -1;
		}*/
		
		auto runl = ntfs->mapping_pairs_decomp(record(), this->li);
		
		if (runl)
		{
			li = runl;
			return 0;
		}
		
		
		return -1;
	}
	
	Utils::shared_ptr<uint8_t> NTFSAttribute::read_compressed(const size_t off, size_t* size_left_in_block)
	{
		assert(NOT_IMPLEMENTED);
	}
	
	auto NTFSAttribute::find_vcn(const size_t vcn) -> Utils::shared_ptr<li_t>
	{
		using namespace detail::NTFS;
		if (!li)
		{
			//assert(NOT_IMPLEMENTED);
			int res = map_runlist(vcn);
			assert(res >= 0);
			if (res < 0)
			{
				return nullptr;
			}
		}
		
		int64_t last_vcn = -1;
		li_t* last_item = nullptr;
		for (auto& item : *this->li)
		{
			if (item.vcn == vcn)
			{
				return Utils::shared_ptr<li_t>(li, &item);
			}
			else if (item.vcn > vcn && last_item && last_item->vcn < vcn && last_item->lcn >= NTFS_LCN_HOLE)
			{
				assert(last_item->length + last_item->vcn > vcn);
				return Utils::shared_ptr<li_t>(li, last_item);
			}
			
			last_item = &item;
		}
		TRACE_VAL(vcn);
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	
	int64_t NTFSAttribute::runlist_vcn_to_lcn(int64_t vcn) const noexcept
	{
		using namespace detail::NTFS;
		if (vcn < 0)
		{
			return (int64_t)NTFS_LCN_EINVAL;
		}
		
		if (!li || li->empty())
		{
			return NTFS_LCN_LI_NOT_MAPPED;
		}
		else if (vcn < li->at(0).vcn)
		{
			return NTFS_LCN_ENOENT;
		}
		
		for (auto i = 0; i < li->size() && li->at(i).length; ++i)
		{
			
		}
		
		assert(NOT_IMPLEMENTED);
	}
}