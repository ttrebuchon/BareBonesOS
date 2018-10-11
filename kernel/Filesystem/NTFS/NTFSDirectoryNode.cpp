#include "NTFSDirectoryNode.hh"
#include "NTFS.hh"
#include "NTFSAttribute.hh"
#include <Std/errno.h>

namespace Kernel::FS
{
	
	namespace detail::NTFS
	{
		ntfs_char_t NTFS_INDEX_I30[5] = { '$', 'I', '3', '0', '\0' };
	}
	
	NTFSDirectoryNode::NTFSDirectoryNode(NTFS* ntfs, ptr<ntfs_mft_record_t> mrec, ptr<ntfs_stdinfo_t> info) :
			NTFSNode(ntfs, mrec, info),
			children(0),
			has_read(false),
			has_pending(false)
	{
		if (mft_rec)
		{
			this->_name = this->get_name();
		}
	}
	
	
	
	bool NTFSDirectoryNode::init()
	{
		assert(children.size() == 0);
		assert(!has_read);
		assert(!has_pending);
		
		if (has_read)
		{
			return false;
		}
		
		
		assert(this_ptr());
		assert(mft_rec->flags & NTFS_MFT_REC_IS_DIR);
		
		
		using namespace detail::NTFS;
		Utils::list<uint64_t> mrefs;
		Utils::shared_ptr<NTFSAttribute> bmp_attr = nullptr;
		Utils::shared_ptr<uint8_t> bmp = nullptr;
		Utils::shared_ptr<ntfs_attr_record_t> root_attr = nullptr;
		size_t left;
		int bmp_pos;
		Utils::shared_ptr<ntfs_index_block_t> iblock = nullptr;
		int64_t isize;
		size_t clsz;
		size_t index_vcn_size;
		uint32_t indx_blk;
		ntfs_index_root_t* iroot = nullptr;
		uint8_t* index_end;
		ntfs_index_entry_t* ient = nullptr;
		
		auto ctx = Utils::make_shared<attr_search_t>(volume(), this_ptr(), mft_rec);
		
		auto iattr = ctx->open_attribute(NTFS_ATTR_TYPE_INDEX_ALLOC, NTFS_INDEX_I30, 4);
		if (!iattr)
		{
			if (errno != ENOENT)
			{
				TRACE_VAL(errno); 
				//goto err;
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			else
			{
				assert(this->mft_rec);
				assert(mft_rec->flags & NTFS_MFT_REC_IS_DIR);
				isize = 0;
			}
		}
		else
		{
			isize = iattr->record()->data_size;
		}
		
		
		
		ctx->reset();
		if (!ctx->lookup_attribute(NTFS_ATTR_TYPE_INDEX_ROOT, NTFS_INDEX_I30, 4, false, 0, nullptr, 0))
		{
			tset_error(EIO, "NTFS dir root index attr missing.");
			goto err;
		}
		
		root_attr = ctx->attr;
		assert(root_attr);
		iroot = (ntfs_index_root_t*)((uint8_t*)root_attr.get() + root_attr->value_off);
		
		indx_blk = iroot->index_block_size;
		if (indx_blk < 512 || (indx_blk & (indx_blk-1)))
		{
			tset_error(EIO, "NTFS index block size is invalid.");
			goto err;
		}
		
		clsz = ntfs->cluster_size();
		
		if (clsz <= indx_blk)
		{
			index_vcn_size = clsz;
		}
		else
		{
			index_vcn_size = 512;
		}
		
		index_end = (uint8_t*)&iroot->index + iroot->index.index_len;
		
		ient = (ntfs_index_entry_t*)((uint8_t*)&iroot->index + iroot->index.entries_off);
		
		assert((uint8_t*)ient < index_end);
		assert(ient->length > 16);
		
		do
		{
			
			if ((uint8_t*)ient + sizeof(ntfs_index_entry_head_t) > index_end)
			{
				tset_error(EIO, "Index entry cannot fit entry header.");
				TRACE_VAL(index_end - (uint8_t*)ient);
				TRACE_VAL(sizeof(ntfs_index_entry_head_t));
				goto err;
			}
			
			
			if ((uint8_t*)ient + ient->key_length > index_end)
			{
				tset_error(EIO, "NTFS index entry key length is too long.");
				goto err;
			}
			
			if (ient->flags & NTFS_INDEX_ENTRY_END)
			{
				break;
			}
			
			
			
			if (!ient->length)
			{
				tset_error(EIO, "NTFS index cannot contain entry with length '0'.");
				goto err;
			}
			
			if (!(ient->flags & NTFS_INDEX_ENTRY_NODE))
			{
				mrefs.push_back(mft_ref(ient->indexed_file));
			}
			else
			{
				TRACE_VAL_HX(ient->flags);
				assert(ient->flags & NTFS_INDEX_ENTRY_NODE);
				
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			
			ient = (ntfs_index_entry_t*)((uint8_t*)ient + ient->length);
		}
		while (true);
		
		
		
		
		if (!iattr)
		{
			goto done;
		}
		
		
		ctx->reset();
		bmp_attr = ctx->open_attribute(NTFS_ATTR_TYPE_BITMAP, NTFS_INDEX_I30, 4);
		if (!bmp_attr)
		{
			//tset_error(EIO, "Failed to open NTFS bitmap attr $I30.");
			goto err;
		}
		
		assert(bmp_attr->record());
		bmp = bmp_attr->read(0, &left);
		assert(bmp);
		if (!bmp)
		{
			goto err;
		}
		assert(left > 0);
		
		if (left > clsz)
		{
			// TODO: Read bitmap and index
			// entried from multiple chunks
			assert(NOT_IMPLEMENTED);
		}
		
		bmp_pos = 0;
		
		while (bmp_pos < left)
		{
			while (!(bmp[bmp_pos / 8] & (1 << (bmp_pos & 0x7))) && bmp_pos < left)
			{
				++bmp_pos;
				//iattr_pos = bmp_pos * indx_blk;
			}
			
			if (bmp_pos >= left)
			{
				break;
			}
			
			size_t ileft;
			auto iblock_raw = iattr->read(bmp_pos*indx_blk, &ileft);
			assert(iblock_raw);
			assert(ileft > 0);
			
			iblock = Utils::shared_ptr<ntfs_index_block_t>(iblock_raw, (ntfs_index_block_t*)iblock_raw.get());
			
			assert(iblock->magic == NTFS_INDEX);
			
			if (iblock->index.allocated_size + 0x18 != indx_blk)
			{
				tset_error(EIO, "Invalid index block.");
				TRACE_VAL(iblock->index.allocated_size);
				TRACE_VAL(indx_blk);
				TRACE_VAL_HX(iblock->index.flags);
				goto err;
			}
			
			
			index_end = (uint8_t*)&iblock->index + iblock->index.index_len;
			
			ient = (ntfs_index_entry_t*)((uint8_t*)&iblock->index + iblock->index.entries_off);
			assert(ient->___reserved2 == 0);
			
			
			
			
			assert(ient->length > 0);
			assert(ient->key_length > 0);
			
			
			for (;; ient = (ntfs_index_entry_t*)((uint8_t*)ient + ient->length))
			{
				if ((uint8_t*)ient + sizeof(ntfs_index_entry_head_t) > index_end)
				{
					tset_error(EIO, "NTFS index entry is out of bounds.");
					goto err;
				}
				
				
				if (ient->flags & NTFS_INDEX_ENTRY_END)
				{
					break;
				}
				
				assert(ient->length > 0);
				if (!ient->length)
				{
					tset_error(EIO, "NTFS index entry should not be able to have a zero length.");
					goto err;
				}
				
				auto mval = mft_ref(ient->indexed_file);
				auto& fname = ient->key.filename;
				
				if (ient->flags & NTFS_INDEX_ENTRY_NODE)
				{
					assert(NOT_IMPLEMENTED);
				}
				
				mrefs.push_back(mval);
				
			}
			++bmp_pos;
		}
		
		for (auto it = mrefs.begin(); it != mrefs.end(); ++it)
		{
			auto it2 = it;
			++it2;
			for (; it2 != mrefs.end(); ++it2)
			{
				if (*it == *it2)
				{
					TRACE_VAL(*it);
					assert(NOT_IMPLEMENTED);
				}
			}
		}
		
		done:
		
		for (const auto& mref : mrefs)
		{
			auto nnode = ntfs->inode_open(mref);
			if (!nnode)
			{
				TRACE_VAL(mref);
				assert(NOT_IMPLEMENTED);
			}
			
			
			auto raw_inode = nnode->get_inode();
			if (!raw_inode)
			{
				TRACE_VAL(mref);
				assert(NOT_IMPLEMENTED);
			}
			
			
			
			children.push_back(node_ptr<>(Utils::shared_ptr<Node>(nnode, raw_inode)));
		}
		
		
		
		
		has_read = true;
		return true;
		
		err:
		
		assert(NOT_IMPLEMENTED);
		return false;
	}
	
	
	
	
	
	
	
	
	
	
	uint64_t NTFSDirectoryNode::read(uint64_t start, uint64_t len, void* buf)
	{
		return n_read(start, len, buf);
	}
	
	uint64_t NTFSDirectoryNode::write(uint64_t start, uint64_t len, const void* buf)
	{
		return n_write(start, len, buf);
	}
	
	void NTFSDirectoryNode::open()
	{
		TRACE("NOT YET IMPLEMENTED");
	}
	
	void NTFSDirectoryNode::close()
	{
		TRACE("NOT YET IMPLEMENTED");
	}
	
		
	node_ptr<> NTFSDirectoryNode::add(const node_ptr<>&)
	{
		if (!has_read)
		{
			init();
		}
		assert(NOT_IMPLEMENTED);
	}
	
	size_t NTFSDirectoryNode::size() const noexcept
	{
		if (!has_read)
		{
			init_const();
		}
		return children.size();
	}
	
	node_ptr<> NTFSDirectoryNode::at(size_t index) const
	{
		if (!has_read)
		{
			init_const();
		}
		if (likely(index < children.size()))
		{
			return children.at(index);
		}
		else
		{
			return nullptr;
		}
	}
	
	DirectoryNode_v* NTFSDirectoryNode::get_parent() const noexcept
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
	
	Filesystem* NTFSDirectoryNode::get_filesystem() const noexcept
	{
		return volume();
	}
	
}