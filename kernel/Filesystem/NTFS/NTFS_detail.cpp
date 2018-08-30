#include "NTFS.hh"
#include "NTFSNode.hh"
#include <Utils/algorithm>
#include "NTFSAttribute.hh"
#include <Std/errno.h>
#include "MFT.hh"
#include "NTFSDirectoryNode.hh"
#include "NTFSRootDirectoryNode.hh"
#include "NTFSAttrDefNode.hh"
#include "NTFSBadClusNode.hh"
#include "NTFSBitmapNode.hh"
#include "NTFSBootNode.hh"
#include "NTFSExtendNode.hh"
#include "NTFSSecureNode.hh"
#include "NTFSLogFileNode.hh"
#include "NTFSUpCaseNode.hh"
#include "NTFSVolumeNode.hh"
#include "NTFSFileNode.hh"
#include "NTFSCharDeviceNode.hh"

#define NTFS_MAX_COMPRESSION_CLU_SZ 4096

namespace Kernel::FS::detail::NTFS
{
	
	block_t::block_t(const Utils::shared_ptr<block_t>& base, uint8_t* data, size_t indx, bool modified) noexcept : _modified(false), index(indx), len(0), base(base), data(data)
	{
		assert(base);
		assert(data - base->data < base->len);
		assert(indx > base->index);
		
		len = base->len - (data - base->data);
		
		this->modified(modified);
	}
	
	block_t::block_t(size_t indx, size_t len, bool modified) noexcept : _modified(modified), index(indx), len(len), base(nullptr), data(nullptr)
	{
		//TRACE_VAL(indx);
		
		assert(len > 0);
		this->data = (uint8_t*)(operator new(len, (std::align_val_t)4096));
	}
	
	block_t::~block_t()
	{
		//TRACE_VAL(index);
		assert(this->len > 0);
		assert(data);
		
		if (!base)
		{
			if (_modified)
			{
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			else
			{
				delete[] data;
			}
		}
	}
	
	auto block_t::make_block(size_t indx, size_t len, bool modified) noexcept -> pointer_type
	{
		if (!len)
		{
			return nullptr;
		}
		
		return pointer_type(new block_t(indx, len, modified));
	}
	
	auto block_t::attach_block(const Utils::shared_ptr<block_t>& base, uint8_t* data, size_t indx, bool modified) noexcept -> pointer_type
	{
		if (!base || !data)
		{
			return nullptr;
		}
		
		if (unlikely(data == base->data))
		{
			if (unlikely(indx == base->index))
			{
				base->modified(modified);
				return base;
			}
			else
			{
				return nullptr;
			}
		}
		else if (base->base)
		{
			return attach_block(base->base, data, indx, modified);
		}
		else if (unlikely(data < base->data))
		{
			return nullptr;
		}
		else if (unlikely(base->index > indx))
		{
			return nullptr;
		}
		else if (unlikely(data - base->data >= base->len))
		{
			return nullptr;
		}
		
		return pointer_type(new block_t(base, data, indx, modified));
		
	}
	
	
	
	
	uint16_t mft_seq_num(const uint64_t m) noexcept
	{
		return (uint16_t)((m >> 48) & 0xFFFF);
	}
	
	
	attr_search_t::attr_search_t(const FS::NTFS* ntfs, Utils::shared_ptr<NTFSNode> node, Utils::shared_ptr<ntfs_mft_record_t> rec) noexcept
	{
		this->ntfs = ntfs;
		// attrib.c - Ln 3444
		if (!rec)
		{
			assert(node);
			rec = node->mft_rec;
		}
		
		if (!rec)
		{
			// TODO: Error out
			assert(NOT_IMPLEMENTED);
			return;
		}
		
		this->mrec = rec;
		attr = Utils::shared_ptr<ntfs_attr_record_t>(rec, (ntfs_attr_record_t*)((uint8_t*)rec.get() + rec->attrs_off));
		is_first = true;
		inode = node;
		ent = nullptr;
		base_inode = nullptr;
		base_mrec = nullptr;
		base_attr = nullptr; 
	}
	
	
	
	Utils::shared_ptr<NTFSAttribute> attr_search_t::open_attribute(const ntfs_attr_type type, ntfs_char_t* name, uint32_t name_len)
	{
		// attrib.c - Ln 398
		ntfs_char_t* new_name = nullptr;
		Utils::shared_ptr<ntfs_attr_record_t> a = nullptr;
		Utils::shared_ptr<NTFSAttribute> na = nullptr;
		uint16_t cs;
		bool b;
		
		if (!lookup_attribute(type, name, name_len, false, 0, nullptr, 0))
		{
			goto fail;
		}
		
		a = attr;
		if (!name)
		{
			if (a->name_len)
			{
				name = ntfs_uni_cstr_clone((ntfs_char_t*)((uint8_t*)a.get() + a->name_off), a->name_len);
				if (!name)
				{
					goto fail;
				}
				new_name = name;
				name_len = a->name_len;
				
				TRACE_VAL("Found Name: ");
				TRACE_VAL(new_name);
			}
			else
			{
				name_len = 0;
			}
		}
		
		
		na = Utils::make_shared<NTFSAttribute>(inode, type, name, name_len);
		assert(na);
		
		if (type == NTFS_ATTR_TYPE_ATTR_LIST)
		{
			a->flags = (ntfs_attr_flags)0;
		}
		
		if ((type == NTFS_ATTR_TYPE_DATA) && (a->non_resident ? !a->initialized_size : !a->value_len))
		{
			a->flags &= ~NTFS_ATTR_COMPRESSION_MASK;
			
			if (inode->flags & NTFS_ATTR_FILE_COMPRESSED)
			{
				if ((ntfs->major_ver() >= 3)
					&& ntfs->has_state(NTFSState::Compression)
					&& (ntfs->cluster_size() <= NTFS_MAX_COMPRESSION_CLU_SZ))
				{
					a->flags |= NTFS_ATTR_COMPRESSED;
				}
					
			}
		}
		
		
		cs = a->flags & (NTFS_ATTR_COMPRESSED | NTFS_ATTR_SPARSE);
		
		b = (a->flags & NTFS_ATTR_SPARSE) && !na->sparse();
		b |= (!(a->flags & NTFS_ATTR_ENCRYPTED) != !na->encrypted());
		
		b &= (na->type() == NTFS_ATTR_TYPE_DATA);
		b &= (na->name() == nullptr);
		
		if (b)
		{
			TRACE("inode has invalid attr flags.");
			set_error(EIO, "NTFS inode has invalid attr flags.");
			goto fail;
		}
		
		if (a->non_resident)
		{
			if ((a->flags & NTFS_ATTR_COMPRESSION_MASK) && !a->compression_unit)
			{
				TRACE("inode with compression doesn't have a compression unit.");
				set_error(EIO, "inode with compression doesn't have a compression unit.");
				goto fail;
			}
			
			na->init(a);
		}
		else
		{
			na->init(a);
			//assert(NOT_IMPLEMENTED);
		}
		
		return na;
		
		
		
		
		fail:
		if (new_name)
		{
			delete[] new_name;
			new_name = nullptr;
		}
		
		return nullptr;
	}
	
	static int ntfs_names_full_collate(const ntfs_char_t* name1, const uint32_t name1_len, ntfs_char_t* name2, const uint32_t name2_len, bool ignore_case, const ntfs_char_t* upcase, const uint32_t upcase_len)
	{
		assert(name1);
		assert(name2);
		assert(upcase);
		ntfs_char_t c1, c2;
		
		auto len = Utils::min(name1_len, name2_len);
		
		if (unlikely(len == 0))
		{
			if (name1_len < name2_len)
			{
				return -1;
			}
			else if (name1_len > name2_len)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		
		if (ignore_case)
		{
			do
			{
				c1 = *name1;
				++name1;
				c2 = *name2;
				++name2;
				if (c1 < upcase_len)
				{
					c1 = upcase[c1];
				}
				
				if (c2 < upcase_len)
				{
					c2 = upcase[c2];
				}
			}
			while ((c1 == c2) && --len);
			
			if (c1 < c2)
			{
				return -1;
			}
			else if (c1 > c2)
			{
				return 1;
			}
			else if (name1_len < name2_len)
			{
				return -1;
			}
			else if (name1_len > name2_len)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			while (--len && (*name1 == *name2))
			{
				++name1;
				++name2;
			}
			
			ntfs_char_t o1, o2;
			
			c1 = o1 = *name1;
			c2 = o2 = *name2;
			
			if (c1 < upcase_len)
			{
				c1 = upcase[c1];
			}
			
			if (c2 < upcase_len)
			{
				c2 = upcase[c2];
			}
			
			if ((c1 == c2) && len)
			{
				do
				{
					
				}
				while ((c1 == c2) && --len);
			}
			
			if (c1 < c2)
			{
				return -1;
			}
			else if (c1 > c2)
			{
				return 1;
			}
			else if (name1_len < name2_len)
			{
				return -1;
			}
			else if (name1_len > name2_len)
			{
				return 1;
			}
			else if (o1 < o2)
			{
				return -1;
			}
			else if (o1 > o2)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		
		//assert(NOT_IMPLEMENTED);
	}
	
	bool attr_search_t::lookup_attribute(const ntfs_attr_type type, ntfs_char_t* name, uint32_t name_len, bool ignore_case, const size_t cluster_no, const uint8_t* val, const uint32_t val_len) noexcept
	{
		uint32_t upcase_len;
		auto upcase = ntfs->upcase(&upcase_len);
		if (!mrec || !attr || (name && (!inode || !ntfs || !upcase || !upcase_len)))
		{
			tset_error(EINVAL, "Invalid arguments to lookup_attribute()");
			return false;
		}
		
		auto base_ni = base_inode;
		if (!base_ni)
		{
			base_ni = inode;
		}
		
		if (!base_ni || ((base_ni->flags & NTFS_ATTR_TYPE_ATTR_LIST) == (uint32_t)0) || type == NTFS_ATTR_TYPE_ATTR_LIST)
		{
			return find_attribute(type, name, name_len, ignore_case, val, val_len);
		}
		else
		{
			return find_external_attribute(type, name, name_len, ignore_case, cluster_no, val, val_len);
		}
	}
	
	bool attr_search_t::find_attribute(const ntfs_attr_type type, ntfs_char_t* name, uint32_t name_len, bool ignore_case, const uint8_t* val, const uint32_t val_len) noexcept
	{
		// attrib.c - Ln 3372
		// It calls:
		// attrib.c - Ln 2761
		
		
		uint32_t upcase_len;
		const ntfs_char_t* upcase = ntfs->upcase(&upcase_len);
		Utils::shared_ptr<ntfs_attr_record_t> a = nullptr;
		
		assert(attr);
		
		if (is_first)
		{
			a = attr;
			is_first = false;
		}
		else
		{
			a = Utils::shared_ptr<ntfs_attr_record_t>(attr, (ntfs_attr_record_t*)((uint8_t*)attr.get() + attr->length));
		}
		
		for (;; a = Utils::shared_ptr<ntfs_attr_record_t>(a, (ntfs_attr_record_t*)((uint8_t*)a.get() + a->length)))
		{
			assert(a);
			
			if ((uint8_t*)a.get() - (uint8_t*)mrec.get() > mrec->bytes_allocated)
			{
				break;
			}
			
			attr = a;
			
			if (((type != NTFS_ATTR_TYPE_UNUSED) && ((uint32_t)a->type > (uint32_t)type)) || a->type == NTFS_ATTR_TYPE_END)
			{
				tset_error(ENOENT, "No attribute found.");
				return false;
			}
			
			if (!a->length)
			{
				TRACE_VAL((void*)a.get());
				TRACE("Breaking due to \"(!a->length)\"");
				break;
			}
			
			if (type == NTFS_ATTR_TYPE_UNUSED)
			{
				return true;
			}
			
			if (a->type != type)
			{
				continue;
			}
			
			if (name == nullptr)
			{
				if (a->name_len)
				{
					tset_error(ENOENT, "The found attribute was named.");
					return false;
				}
			}
			else
			{
				int rc;
				if ((rc = ntfs_names_full_collate(name, name_len, (ntfs_char_t*)((uint8_t*)a.get() + a->name_off), a->name_len, ignore_case, upcase, upcase_len)))
				{
					if (rc < 0)
					{
						tset_error(ENOENT, "The attr name collates before the supplied name.");
						return false;
					}
					
					continue;
				}
			}
			
			
			
			if (!val)
			{
				return true;
			}
			else
			{
				int rc = memcmp(val, (uint8_t*)a.get() + a->value_off, Utils::min(val_len, a->value_len));
				
				if (!rc)
				{
					uint32_t avall;
					avall = a->value_len;
					if (val_len == avall)
					{
						return true;
					}
					
					if (val_len < avall)
					{
						return false;
					}
				}
				else if (rc < 0)
				{
					return false;
				}
			}
		}
		
		set_error(EIO, "Corrupt inode.");
		return false;
	}
	
	bool attr_search_t::find_external_attribute(const ntfs_attr_type, ntfs_char_t* name, uint32_t name_len, bool ignore_case, const size_t cluster_no, const uint8_t* val, const uint32_t val_len) noexcept
	{
		assert(NOT_IMPLEMENTED);
	}
	
	void attr_search_t::reset() noexcept
	{
		if (!base_inode)
		{
			is_first = true;
			attr = Utils::shared_ptr<ntfs_attr_record_t>(mrec, (ntfs_attr_record_t*)((uint8_t*)mrec.get() + mrec->attrs_off));
			ent = nullptr;
			return;
		}
		
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	
	
	
	
	ntfs_char_t* ntfs_uni_cstr_clone(const ntfs_char_t* cstr, const uint32_t len) noexcept
	{
		auto nstr = new ntfs_char_t[len];
		memcpy(nstr, cstr, len*sizeof(ntfs_char_t));
		return nstr;
	}
	
	static char ntfs_char_to_char(const ntfs_char_t nc) noexcept
	{
		return 0;
	}
	
	char* ntfs_str_to_char(const ntfs_char_t* vname, uint32_t length) noexcept
	{
		assert(length);
		assert(vname);
		char* cstr = new char[length+1];
		size_t clen = length;
		uint32_t j, k;
		wchar_t nc;
		char c;
		
		for (size_t i = j = 0; i < length; ++i)
		{
			if (j + MB_CUR_MAX > clen)
			{
				assert(64 >= MB_CUR_MAX);
				clen += 64;
				auto tmp = new char[clen+1];
				strncpy(tmp, cstr, j);
				delete[] cstr;
				cstr = tmp;
			}
			nc = (wchar_t)vname[i];
			if (!nc)
			{
				break;
			}
			
			k = wctomb(cstr + j, nc);
			if (k == -1)
			{
				goto error;
			}
			else
			{
				assert(k > 0);
				j += k;
			}
		}
		
		if (j != clen)
		{
			auto tmp = new char[j+1];
			strncpy(tmp, cstr, j);
			delete[] cstr;
			cstr = tmp;
			clen = j;
		}
		
		cstr[clen] = '\0';
		
		return cstr;
		
		
		error:
		
		if (cstr)
		{
			delete[] cstr;
			cstr = nullptr;
		}
		return nullptr;
	}
	
	typedef Utils::shared_ptr<Utils::vector<list_ent_t>> li_ptr;
	
	static li_ptr merge_attr_li(li_ptr& lhs, li_ptr& rhs)
	{
		// runlist.c - Ln 508
		
		
		
		
		assert(NOT_IMPLEMENTED);
	}
	
}


namespace Kernel::FS
{
	using namespace detail::NTFS;
	typedef Utils::shared_ptr<Utils::vector<list_ent_t>> li_ptr;
	
	auto NTFS::mapping_pairs_decomp(const Utils::shared_ptr<const ntfs_attr_record_t>& attr, Utils::shared_ptr<Utils::vector<li_t>> old) const noexcept -> Utils::shared_ptr<Utils::vector<li_t>>
	{
		// runlist.c - Ln 1013
		
		// runlist.c - Ln 784
		
		vcn_t vcn;
		lcn_t lcn;
		int64_t delta_cn;
		Utils::vector<li_t> l;
		li_t* li = nullptr;
		const uint8_t* buf = nullptr;
		const uint8_t* buf_end = nullptr;
		int lsz;
		uint16_t lpos;
		uint8_t itb;
		
		
		if (!attr || !attr->non_resident || attr->lowest_vcn < 0)
		{
			tset_error(EINVAL, "Invalid attr.");
			return nullptr;
		}
		
		vcn = attr->lowest_vcn;
		lcn = 0;
		buf = (const uint8_t*)attr.get() + attr->mapping_pairs_offset;
		buf_end = (const uint8_t*)attr.get() + attr->length;
		if (buf < (const uint8_t*)attr.get() || buf > buf_end)
		{
			tset_error(EIO, "NTFS corrupt attribute.");
			return nullptr;
		}
		
		
		lpos = 0;
		lsz = 0x1000;
		
		l.resize(1);
		li = &l[0];
		
		if (vcn)
		{
			li->vcn = (vcn_t)0;
			li->lcn = (lcn_t)NTFS_LCN_LI_NOT_MAPPED;
			li->length = vcn;
			++lpos;
		}
		
		while (buf < buf_end && *buf)
		{
			// runlist.c - Ln 830
			
			/*if ((int)(lpos + 3) > l.size())
			{
				
			}*/
			
			while (l.size() <= lpos)
			{
				assert(lpos - l.size() <= 1);
				l.push_back(li_t());
			}
			
			l[lpos].vcn = vcn;
			
			
			
			itb = *buf & 0xF;
			if (itb)
			{
				if (itb + buf > buf_end)
				{
					tset_error(EIO, "NTFS attribute has inconsistent length.");
					return nullptr;
				}
				
				for (delta_cn = (int8_t)buf[itb--]; itb; itb--)
				{
					delta_cn = (delta_cn << 8) + buf[itb];
				}
			}
			else
			{
				tset_error(EIO, "Missing length entry in mapping pairs.");
				return nullptr;
				
				
				delta_cn = ((int64_t) -1);
			}
			
			
			if (delta_cn < 0)
			{
				tset_error(EIO, "invalid length in mapped pairs array");
				return nullptr;
			}
			
			
			l[lpos].length = delta_cn;
			
			vcn += delta_cn;
			
			if ((*buf & 0xF0) == 0)
			{
				l[lpos].lcn = (lcn_t)NTFS_LCN_HOLE;
			}
			else
			{
				uint8_t itb2 = *buf & 0xF;
				itb = itb2 + ((*buf >> 4) & 0xF);
				if (buf + itb > buf_end)
				{
					tset_error(EIO, "Attribute mapping pair length is too long.");
					return nullptr;
				}
				
				for (delta_cn = (int8_t)buf[itb--]; itb > itb2; itb--)
				{
					delta_cn = (delta_cn << 8) + buf[itb];
				}
				
				lcn += delta_cn;
				
				
				
				
				if (lcn < (lcn_t)-1)
				{
					tset_error(EIO, "Invalid negative LCN in mapping pairs.");
					return nullptr;
				}
				
				l[lpos].lcn = lcn;
			}
			
			++lpos;
			buf += (*buf & 0xF) + ((*buf >> 4) & 0xF) + 1;
		}
		
		if (buf >= buf_end)
		{
			tset_error(EIO, "buf should not be past buf_end.");
			return nullptr;
		}
		
		delta_cn = attr->highest_vcn;
		if (delta_cn && (vcn - 1) != delta_cn)
		{
			tset_error(EIO, "Corrupt attr mapping array.");
			return nullptr;
		}
		
		if (attr->lowest_vcn == 0)
		{
			vcn_t ccount = ((attr->allocated_size + cluster_size() - 1) / cluster_size());
			if (ccount > vcn)
			{
				while (l.size() <= lpos)
				{
					l.push_back(li_t());
				}
				l[lpos].vcn = vcn;
				vcn += l[lpos].length = ccount - vcn;
				l[lpos].lcn = (lcn_t)NTFS_LCN_LI_NOT_MAPPED;
				++lpos;
			}
			else if (vcn > ccount)
			{
				tset_error(EIO, "Corrupt attribute, incorrect number of clusters");
				return nullptr;
			}
			while (l.size() <= lpos)
			{
				l.push_back(li_t());
			}
			l[lpos].lcn = (lcn_t)NTFS_LCN_ENOENT;
		}
		else
		{
			l[lpos].lcn = (lcn_t)NTFS_LCN_LI_NOT_MAPPED;
		}
		
		l[lpos].vcn = vcn;
		l[lpos].length = 0;
		
		auto new_list = Utils::make_shared<Utils::vector<li_t>>(Utils::move(l));
		assert(l.size() == 0);
		
		if (!old)
		{
			return new_list;
		}
		
		auto merged = merge_attr_li(old, new_list);
		old = merged;
		return old;
		
		
		
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	Utils::shared_ptr<NTFSNode> NTFS::inode_open(uint64_t mref) noexcept
	{
		using namespace detail::NTFS;
		mref = mft_ref(mref);
		
		auto node = nodes[mref];
		
		if (node)
		{
			return node;
		}
		
		// inode.c - Ln 174
		auto rec = this->get_mft_record(mref, true);
		if (!rec)
		{
			//set_error(EIO, "Couldn't get MFT record for inode.");
			return nullptr;
		}
		
		
		auto ctx = Utils::make_shared<attr_search_t>(this, nullptr, rec);
		assert(ctx);
		if (!ctx)
		{
			return nullptr;
		}
		
		if (!ctx->lookup_attribute(NTFS_ATTR_TYPE_STD_INFO, nullptr, 0, false, 0, nullptr, 0))
		{
			return nullptr;
		}
		
		auto stdinfo = Utils::shared_ptr<ntfs_stdinfo_t>(ctx->attr, (ntfs_stdinfo_t*)((uint8_t*)ctx->attr.get() + ctx->attr->value_off));
		
		
		/*if ((stdinfo->file_attrs & NTFS_ATTR_FILE_VALID_SET_FLAGS) != stdinfo->file_attrs)
		{
			TRACE_VAL_HX(stdinfo->file_attrs & ~(NTFS_ATTR_FILE_VALID_SET_FLAGS));
			tset_error(EIO, "Invalid standard info flags.");
			return nullptr;
		}*/
		
		switch (mref)
		{
			case NTFS_FILE_EXTEND:
			TRACE("Is $Extend");
			node = Utils::shared_ptr<NTFSNode>(new NTFSExtendNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_SECURE:
			TRACE("Is $Secure");
			node = Utils::shared_ptr<NTFSNode>(new NTFSSecureNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_LOG_FILE:
			TRACE("Is $LogFile");
			node = Utils::shared_ptr<NTFSNode>(new NTFSLogFileNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_MFT_MIRROR:
			TRACE("Is MFT mirror");
			node = Utils::make_shared<MFTNode>(this, rec, stdinfo);
			break;
			
			case NTFS_FILE_VOLUME:
			TRACE("Is $VOLUME");
			node = Utils::shared_ptr<NTFSNode>(new NTFSVolumeNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_UPCASE:
			TRACE("Is $UPCASE");
			node = Utils::shared_ptr<NTFSNode>(new NTFSUpCaseNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_ROOT:
			if (stdinfo->file_attrs & NTFS_ATTR_FILE_ARCHIVE)
			{
				TRACE("Is root archive/dir");
				node = Utils::shared_ptr<NTFSRootDirectoryNode>(new NTFSRootDirectoryNode(this, rec, stdinfo));
			}
			break;
			
			case NTFS_FILE_ATTR_DEF:
			TRACE("Is $AttrDef");
			node = Utils::shared_ptr<NTFSAttrDefNode>(new NTFSAttrDefNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_BAD_CLUSTERS:
			TRACE("Is $BadClus");
			node = Utils::shared_ptr<NTFSBadClusNode>(new NTFSBadClusNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_BITMAP:
			TRACE("Is $Bitmap");
			node = Utils::shared_ptr<NTFSNode>(new NTFSBitmapNode(this, rec, stdinfo));
			break;
			
			case NTFS_FILE_BOOT:
			TRACE("Is $Boot");
			node = Utils::shared_ptr<NTFSNode>(new NTFSBootNode(this, rec, stdinfo));
			break;
		}
		
		// This shouldn't be possible
		// this flag is reserved
		assert(!(stdinfo->file_attrs & NTFS_ATTR_FILE_DIR));
		
		
		if (node)
		{ }
		else if (stdinfo->file_attrs & NTFS_ATTR_FILE_NORMAL)
		{
			TRACE("Is normal file");
		}
		else if (rec && (rec->flags & NTFS_MFT_REC_IS_DIR))
		{
			// A directory
			TRACE("Is directory");
			node = Utils::make_shared<NTFSDirectoryNode>(this, rec, stdinfo);
		}
		else
		{
			ctx->reset();
			/*auto ctx = this->attr_search(nullptr, rec);
			assert(ctx);*/
			
			enum
			{
				UNKNOWN = 0x0,
				REG_FILE = 0x1,
				CHAR_DEV = 0x2,
				BLOCK_DEV = 0x3,
				LINK = 0x4
			} ftype = UNKNOWN;
			
			if (ctx->lookup_attribute(NTFS_ATTR_TYPE_DATA, nullptr, 0, false, 0, nullptr, 0))
			{
				auto attr = ctx->attr;
				assert(attr);
				
				
				
				if (!attr->non_resident)
				{
					if (attr->value_len < sizeof(ntfs_intx_file_t))
					{
						ftype = REG_FILE;
						goto found_type;
					}
					assert(attr->value_len >= sizeof(ntfs_intx_file_t));
					auto f = (ntfs_intx_file_t*)((uint8_t*)attr.get() + attr->value_off);
					
					#ifdef DEBUG
					{
						auto ft = f->magic;
						bool valid = false;
						valid |= (ft == NTFS_INTX_SYMLINK_TYPE);
						valid |= (ft == NTFS_INTX_CHAR_DEV_TYPE);
						valid |= (ft == NTFS_INTX_BLOCK_DEV_TYPE);
						
						assert(valid);
					}
					#endif
					
					switch (f->magic)
					{
						case NTFS_INTX_CHAR_DEV_TYPE:
						ftype = CHAR_DEV;
						break;
						
						case NTFS_INTX_BLOCK_DEV_TYPE:
						ftype = BLOCK_DEV;
						break;
						
						case NTFS_INTX_SYMLINK_TYPE:
						ftype = LINK;
						break;
					}
				}
				else
				{
					assert(NOT_IMPLEMENTED);
				}
			}
			else
			{
				assert(NOT_IMPLEMENTED);
			}
			
			found_type:
			
			switch (ftype)
			{
				case REG_FILE:
				TRACE("Is a regular file");
				node = Utils::make_shared<NTFSFileNode>(this, rec, stdinfo);
				break;
				
				case CHAR_DEV:
				TRACE("Is character device");
				node = Utils::make_shared<NTFSCharDeviceNode>(this, rec, stdinfo);
				break;
				
				case BLOCK_DEV:
				TRACE("Is block device");
				assert(NOT_IMPLEMENTED);
				break;
				
				case LINK:
				TRACE("Is symbolic link");
				assert(NOT_IMPLEMENTED);
				break;
			}
			
			//delete ctx;
		}
		
		if (node)
		{
			node->set_weak(node);
			assert(!nodes[mref]);
			nodes[mref] = node;
			return node;
		}
		
		
		TRACE_VAL(mref);
		
		
		assert(NOT_IMPLEMENTED);
	}
}