#include "NTFS.hh"
#include "NTFSFactory.hh"
#include <drivers/Disk/Disk.hh>
#include "MFT.hh"
#include <Utils/mutex>
#include <Std/errno.h>
#include "NTFSAttribute.hh"
#include "NTFSDirectoryNode.hh"


namespace Kernel::FS
{
	
	NTFS::NTFS(Drivers::Disk& disk) : Filesystem(), disk(&disk), _factory(nullptr)
	{
		
	}
	
	NTFS::~NTFS()
	{
		assert(_factory);
		if (_factory)
		{
			delete _factory;
			_factory = nullptr;
		}
	}
	
	
	Factory& NTFS::factory() noexcept
	{
		assert(likely(_factory != nullptr));
		if (unlikely(!_factory))
		{
			// TODO
		}
		
		return *_factory;
	}
	
	
	
	
	
	
	
	
	size_t NTFS::MFT_mirror_size() const noexcept
	{
		if (cluster_size() <= 4*mft_rec_size)
		{
			return 4*mft_rec_size;
		}
		else
		{
			return (cluster_size() / mft_rec_size)*mft_rec_size;
		}
	}
	
	
	auto NTFS::get_block(size_t index, bool writing) -> ptr<block_type>
	{
		ptr<block_type> blk = nullptr;
		if (writing)
		{
			write_lock lock(mut);
			blk = get_block_internal(index, lock);
			if (blk)
			{
				// TODO: Set modified
				blk->modified(true);
			}
		}
		else
		{
			read_lock lock(mut);
			blk = get_block_internal(index, lock);
		}
		return blk;
	}
	
	
	static const ntfs_record_type mft_types[8] = { NTFS_FILE, NTFS_INDEX, NTFS_HOLE, NTFS_RSTR, NTFS_RCRD, NTFS_CHKD, NTFS_BAD, NTFS_EMPTY };
	
	auto NTFS::get_mft_record(size_t index, bool writing) -> ptr<ntfs_mft_record_t>
	{
		assert((((uint64_t)index) & NTFS_MREF_MASK) == (uint64_t)index);
		auto mft_num = (uint64_t)((index) & 0x0000FFFFFFFFFFFFULL);
		assert(mft_num == index);
		
		ptr<block_type> blk = nullptr;
		size_t mft_off = 0;
		ptr<ntfs_mft_record_t> record = nullptr;
		
		if (likely(index != NTFS_FILE_MFT && MFT_data != nullptr))
		{
			if (index + 1 > (MFT_data->record()->initialized_size / mft_rec_size))
			{
				tset_error(ESPIPE, "Can't read unallocated MFT records.");
				return nullptr;
			}
			
			size_t attr_off = (mft_rec_size*index)/cluster_size();
			
			size_t left;
			auto raw = this->MFT_data->read(index*mft_rec_size, &left);
			
			if (!raw)
			{
				//set_error(EIO, "Failed to read from MFT DATA attribute.");
				return nullptr;
			}
			
			if (left < mft_rec_size)
			{
				tset_error(EIO, "MFT record lies across clusters.");
				TRACE_VAL(left);
				return nullptr;
			}
			
			record = ptr<ntfs_mft_record_t>(raw, (ntfs_mft_record_t*)raw.get());
			
			
			
			
			
			/*blk = this->MFT_data->read_cluster(attr_off);
			
			if (!blk)
			{
				set_error(EIO, "Failed to read record from MFT.");
				return nullptr;
			}
			
			
			mft_off = index - ((attr_off*cluster_size())/mft_rec_size);*/
		}
		else
		{
			size_t cluster = MFT_cluster_index() + (index*mft_rec_size)/cluster_size();
			blk = get_block(cluster, writing);
			assert(blk);
			if (!blk)
			{
				return nullptr;
			}
			mft_off = index - (((cluster - MFT_cluster_index())*cluster_size())/mft_rec_size);
			auto raw_record = (ntfs_mft_record_t*)blk->data + mft_off;
			
			record = ptr<ntfs_mft_record_t>(blk, raw_record);
		}
		
		assert(record->reserved == 0);
		
		
		using namespace detail::NTFS;
		if (mft_seq_num(index) && mft_seq_num(index) != record->seq_num)
		{
			tset_error(EIO, "MFT record has wrong sequence number.");
			return nullptr;
		}
		
		if (!check_mft_record(index, record))
		{
			return nullptr;
		}
		
		
		for (auto i = 0; i < 9; ++i)
		{
			if (unlikely(i == 8))
			{
				tset_error(EIO, "MFT record has invalid type_magic.");
				return nullptr;
			}
			else if (record->type_magic == mft_types[i])
			{
				break;
			}
		}
		
		return record;//ptr<ntfs_mft_record_t>(blk, record);
		
		assert(NOT_IMPLEMENTED);
	}
	
	bool NTFS::check_mft_record(size_t index, ptr<ntfs_mft_record_t> rec) const
	{
		assert(rec);
		if (!rec)
		{
			return false;
		}
		
		if (rec->bytes_allocated != mft_rec_size)
		{
			tset_error(EIO, "MFT record has incorrect bytes_allocated.");
			TRACE_VAL(rec->bytes_allocated);
			return false;
		}
		
		if (rec->attrs_off > mft_rec_size)
		{
			tset_error(EIO, "MFT record is corrupt.");
			return false;
		}
		
		
		return true;
	}
	
	
	
	
	
	
	
	
	detail::NTFS::attr_search_t* NTFS::attr_search(ptr<NTFSNode> node, ptr<ntfs_mft_record_t> record) const
	{
		assert(node || record);
		
		if (!node && !record)
		{
			return nullptr;
		}
		
		return new detail::NTFS::attr_search_t(this, node, record);
	}
	
	Utils::shared_ptr<NTFSAttribute> NTFS::open_attribute(Utils::shared_ptr<NTFSNode> ni, const ntfs_attr_type type, ntfs_char_t* name, uint32_t name_len) const
	{
		auto ctx = attr_search(ni, nullptr);
		assert(ctx);
		if (!ctx)
		{
			return nullptr;
		}
		auto a = ctx->open_attribute(type, name, name_len);
		delete ctx;
		return a;
	}
	
	
	
	
	
	
	
	auto NTFS::get_block_internal(size_t index, read_lock& lock) noexcept -> ptr<block_type>
	{
		assert(lock.owns_lock());
		bool start_owns_lock = lock.owns_lock();
		if (!start_owns_lock)
		{
			lock.lock();
		}
		
		ptr<block_type> bptr = nullptr;
		
		if (blocks.count(index))
		{
			bptr = blocks.at(index).lock();
			if (bptr)
			{
				cache_block(bptr, lock);
				if (!start_owns_lock)
				{
					lock.unlock();
				}
				return bptr;
			}
		}
		
		{
			lock.unlock();
			write_lock wlock(mut);
			bptr = get_block_internal(index, wlock);
		}
		if (start_owns_lock)
		{
			lock.lock();
		}
		return bptr;
	}
	
	auto NTFS::get_block_internal(size_t index, write_lock& lock, bool no_unlock) noexcept -> ptr<block_type>
	{
		assert(lock.owns_lock());
		bool start_owns_lock = lock.owns_lock();
		if (!start_owns_lock)
		{
			lock.lock();
		}
		
		ptr<block_type> bptr = nullptr;
		
		if (blocks.count(index))
		{
			bptr = blocks.at(index).lock();
			if (bptr)
			{
				cache_block(bptr, lock, no_unlock);
				if (!start_owns_lock)
				{
					lock.unlock();
				}
				return bptr;
			}
		}
		
		
		
		bptr = block_type::make_block(index, cluster_size(), false);
		
		uint8_t* data = bptr->data;
		
		auto read = disk->read(index*cluster_size(), cluster_size(), data);
		if (read != cluster_size())
		{
			TRACE("Invalid cluster!");
			assert(NOT_IMPLEMENTED);
			return nullptr;
		}
		
		blocks[index] = bptr;
		cache_block(bptr, lock, no_unlock);
		if (!start_owns_lock)
		{
			lock.unlock();
		}
		return bptr;
	}
	
	void NTFS::cache_block(ptr<block_type> block, read_lock& lock, bool no_unlock) const
	{
		if (no_unlock)
		{
			Utils::lock_guard<Utils::mutex> calock(cache_m);
			block_ptr_cache.cache(block);
		}
		else
		{
			lock.unlock();
			cache_m.lock();
			block_ptr_cache.cache(block);
			cache_m.unlock();
			lock.lock();
		}
	}
	
	void NTFS::cache_block(ptr<block_type> block, write_lock& lock, bool no_unlock) const
	{
		if (no_unlock)
		{
			Utils::lock_guard<Utils::mutex> calock(cache_m);
			block_ptr_cache.cache(block);
		}
		else
		{
			lock.unlock();
			cache_m.lock();
			block_ptr_cache.cache(block);
			cache_m.unlock();
			lock.lock();
		}
	}
	
	const ntfs_char_t* NTFS::upcase(uint32_t* len) const noexcept
	{
		if (len)
		{
			*len = upcase_len;
		}
		return _upcase;
	}
	
	const ntfs_char_t* NTFS::lowcase(uint32_t* len) const noexcept
	{
		if (len)
		{
			*len = lowcase_len;
		}
		return _lowcase;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	class ntfs_setup
	{
		private:
		static bool check_first_mft_record(NTFS*, Utils::shared_ptr<ntfs_mft_record_t>) noexcept;
		public:
		typedef typename detail::NTFS::list_ent_t li_t;
		typedef Utils::shared_ptr<Utils::vector<li_t>> li_ptr;
		
		
		static bool load_mft(NTFS*) noexcept;
		static bool load_mft_mirr(NTFS*) noexcept;
		static bool load_volume_file(NTFS*) noexcept;
		static bool setup_root(NTFS*) noexcept;
		static bool setup_chars(NTFS*) noexcept;
		
	};
	
	
	
	
	NTFS* NTFS::Open(Drivers::Disk& disk) noexcept
	{
		auto ntfs = new NTFS(disk);
		ntfs->_factory = new NTFSFactory(*ntfs);
		size_t mft_lcn;
		size_t mft_zn_sz;
		
		auto read_sz = disk.read(0, sizeof(boot), (uint8_t*)&ntfs->boot);
		assert(read_sz == sizeof(boot));
		if (read_sz != sizeof(boot))
		{
			// TODO
			goto fail;
		}
		
		// 0x202020205346544eULL
		
		
		if (strncmp((const char*)ntfs->boot.OEM_ID, "NTFS    ", 8) != 0)
		{
			set_error(EIO, "Not an NTFS volume");
			goto fail;
		}
		
		ntfs->clu_sz = ntfs->boot.bytes_per_sec*ntfs->boot.sec_per_cluster;
		
		
		
		
		ntfs->cluster_count = ntfs->boot.sec_count / ntfs->boot.sec_per_cluster;
		
		
		if (unlikely(ntfs->boot.clusters_per_mft_record == 0))
		{
			goto fail;
		}
		else if (ntfs->boot.clusters_per_mft_record < 0)
		{
			ntfs->mft_rec_size = 1 << -(ntfs->boot.clusters_per_mft_record);
		}
		else
		{
			ntfs->mft_rec_size = size_t(ntfs->boot.clusters_per_mft_record)*ntfs->cluster_size();
		}
		
		assert(ntfs->cluster_size() >= ntfs->mft_rec_size);
		
		
		if (unlikely(ntfs->boot.clusters_per_index_record == 0))
		{
			goto fail;
		}
		else if (ntfs->boot.clusters_per_index_record < 0)
		{
			ntfs->index_rec_size = 1 << -(ntfs->boot.clusters_per_index_record);
		}
		else
		{
			ntfs->index_rec_size = size_t(ntfs->boot.clusters_per_index_record)*ntfs->cluster_size();
		}
		
		mft_zn_sz = ntfs->cluster_count >> 3;
		ntfs->full_zones = 0;
		
		ntfs->mft_zn_st = ntfs->mft_zn_pos = ntfs->MFT_cluster_index();
		
		mft_lcn = (8192 + 2 * ntfs->cluster_size() - 1) / ntfs->cluster_size();
		if (mft_lcn * ntfs->cluster_size() < 16 * 1024)
		{
			mft_lcn = (16 * 1024 + ntfs->cluster_size() - 1) / ntfs->cluster_size();
		}
		
		if (ntfs->mft_zn_st <= mft_lcn)
		{
			ntfs->mft_zn_st = 0;
		}
		
		ntfs->mft_zn_end = mft_zn_sz + ntfs->MFT_cluster_index();
		
		
		while (ntfs->mft_zn_end >= ntfs->cluster_count)
		{
			mft_zn_sz >>= 1;
			ntfs->mft_zn_end = ntfs->MFT_cluster_index() + mft_zn_sz;
		}
		
		
		ntfs->d1_zn_pos = ntfs->mft_zn_end;
		ntfs->d2_zn_pos = 0;
		ntfs->mft_data_pos = 24;
		
		if (!ntfs_setup::load_mft(ntfs))
		{
			goto fail;
		}
		
		if (!ntfs_setup::load_mft_mirr(ntfs))
		{
			goto fail;
		}
		
		if (!ntfs_setup::setup_chars(ntfs))
		{
			goto fail;
		}
		
		if (!ntfs_setup::load_volume_file(ntfs))
		{
			goto fail;
		}
		
		if (!ntfs_setup::setup_root(ntfs))
		{
			goto fail;
		}
		
		TRACE("NTFS setup is complete");
		
		return ntfs;
		
		
		
		
		
		
		fail:
		if (ntfs)
		{
			delete ntfs;
		}
		return nullptr;
	}
	
	
	
	bool ntfs_setup::load_mft(NTFS* ntfs) noexcept
	{
		assert(ntfs);
		
		auto mft = Utils::shared_ptr<MFTNode>(new MFTNode(ntfs));
		ntfs->MFT = mft;
		mft->ntfs = ntfs;
		ntfs->nodes[NTFS_FILE_MFT] = mft;
		
		auto mb = ntfs->get_mft_record(NTFS_FILE_MFT);
		if (!mb)
		{
			return false;
		}
		
		mft->init_mft(mb, nullptr);
		
		//mft->mft_rec = mb;
		
		// Check MFT record
		if (!check_first_mft_record(ntfs, mb))
		{
			set_error(EINVAL, "NTFS MFT is invalid");
			return false;
		}
		
		
		// Volume.c - Ln ~276
		auto ctx = Utils::wrap_shared(ntfs->attr_search(ntfs->MFT, nullptr));
		if (!ctx)
		{
			return false;
		}
		
		
		if (ctx->lookup_attribute(NTFS_ATTR_TYPE_ATTR_LIST, nullptr, 0, false, 0, nullptr, 0))
		{
			auto attr = ctx->ent;
			assert(attr);
		}
		
		
		assert(ctx->inode == mft);
		
		auto mft_na = ctx->open_attribute(NTFS_ATTR_TYPE_DATA, nullptr, 0);
		
		if (!mft_na)
		{
			set_error(kerrno, "Failed to open NTFS $MFT/$DATA attribute");
			return false;
		}
		ntfs->MFT_data = mft_na;
		
		
		ctx->reset();
		int64_t last_vcn, next_vcn, highest_vcn;
		last_vcn = mft_na->record()->allocated_size / ntfs->cluster_size();
		highest_vcn = next_vcn = 0;
		Utils::shared_ptr<ntfs_attr_record_t> a = nullptr;
		
		while (ctx->lookup_attribute(NTFS_ATTR_TYPE_DATA, nullptr, 0, false, next_vcn, nullptr, 0))
		{
			li_ptr nli = nullptr;
			a = ctx->attr;
			
			if (!a->non_resident)
			{
				TRACE("$MFT must be non-resident");
				set_error(EIO, "$MFT must be non-resident");
				return false;
			}
			
			if ((bool)(a->flags & NTFS_ATTR_COMPRESSION_MASK) || (bool)(a->flags & NTFS_ATTR_ENCRYPTED))
			{
				TRACE("$MFT cannot be encrypted or compressed");
				set_error(EIO, "$MFT cannot be encrypted or compressed");
				return false;
			}
			
			// volume.c - Ln 353
			nli = ntfs->mapping_pairs_decomp(a, ntfs->MFT_data->li);
			if (!nli)
			{
				return false;
			}
			ntfs->MFT_data->li = nli;
			highest_vcn = a->highest_vcn;
			next_vcn = highest_vcn + 1;
			
			if (next_vcn <= 0)
			{
				break;
			}
			
			if (next_vcn < a->lowest_vcn)
			{
				set_error(EIO, "$MFT attribute list is corrupt");
				return false;
			}
		}
		
		if (!a)
		{
			set_error(EIO, "$MFT/$DATA attribute not present.");
			return false;
		}
		
		if (highest_vcn && highest_vcn != last_vcn - 1)
		{
			set_error(EIO, "Could not load list for $MFT/$DATA.");
			return false;
		}
		
		ctx = nullptr;
		
		
		// TODO(?): Update size fields in
		// ntfs->MFT ?
		// volume.c - Ln 389
		
		
		
		ntfs->MFT_bmap = ntfs->open_attribute(ntfs->MFT, NTFS_ATTR_TYPE_BITMAP, nullptr, 0);
		
		if (!ntfs->MFT_bmap)
		{
			set_error(EIO, "Could not open $MFT/$BITMAP");
			return false;
		}
		
		
		return true;
	}
	
	bool ntfs_setup::check_first_mft_record(NTFS* ntfs, Utils::shared_ptr<ntfs_mft_record_t> rec) noexcept
	{
		assert(ntfs);
		assert(rec);
		if (!ntfs || !rec)
		{
			return false;
		}
		
		auto mag = rec->type_magic;
		if (mag != NTFS_FILE)
		{
			return false;
		}
		
		if (rec->bytes_allocated != ntfs->mft_rec_size)
		{
			return false;
		}
		
		auto attr = (ntfs_attr_record_t*)((uint8_t*)rec.get() + rec->attrs_off);
		assert((uint8_t*)attr - (uint8_t*)rec.get() < ntfs->mft_rec_size);
		
		return true;
	}
	
	
	bool ntfs_setup::load_mft_mirr(NTFS* ntfs) noexcept
	{
		// volume.c - Ln 431
		
		TRACE("Opening mirror MFT\n\n\n");
		ntfs->MFT_m = ntfs->inode_open(NTFS_FILE_MFT_MIRROR);
		if (!ntfs->MFT_m)
		{
			return false;
		}
		
		TRACE("Opening mirror MFT DATA");
		ntfs->MFT_m_data = ntfs->open_attribute(ntfs->MFT_m, NTFS_ATTR_TYPE_DATA);
		if (!ntfs->MFT_m_data)
		{
			return false;
		}
		
		if (ntfs->MFT_m_data->map_runlist(0) < 0)
		{
			set_error(EIO, "Couldn't map runlist for $MFTMirr/$DATA");
			return false;
		}
		
		return true;
	}
	
	
	bool ntfs_setup::load_volume_file(NTFS* ntfs) noexcept
	{
		assert(ntfs);
		if (!ntfs)
		{
			return false;
		}
		
		
		auto vin = ntfs->inode_open(NTFS_FILE_VOLUME);
		if (!vin)
		{
			return false;
		}
		
		auto ctx = Utils::wrap_shared(ntfs->attr_search(vin, nullptr));
		if (!ctx)
		{
			return false;
		}
		
		if (!ctx->lookup_attribute(NTFS_ATTR_TYPE_VOLUME_INFO, nullptr, 0, false, 0, nullptr, 0))
		{
			return false;
		}
		
		auto vi_attr = ctx->attr;
		assert(vi_attr);
		
		if (vi_attr->non_resident)
		{
			tset_error(EIO, "NTFS $VOLUME/$VOLUME_INFORMATION has to be resident but isn't.");
			return false;
		}
		
		auto vinfo = Utils::shared_ptr<ntfs_volume_info_t>(vi_attr, (ntfs_volume_info_t*)((uint8_t*)vi_attr.get() + vi_attr->value_off));
		
		ntfs->version[0] = vinfo->major_ver;
		ntfs->version[1] = vinfo->minor_ver;
		
		ctx->reset();
		ntfs_char_t* volume_name = nullptr;
		size_t vname_len = 0;
		if (ctx->lookup_attribute(NTFS_ATTR_TYPE_VOLUME_NAME, nullptr, 0, false, 0, nullptr, 0))
		{
			assert(!ctx->attr->non_resident);
			
			volume_name = (ntfs_char_t*)(ctx->attr->value_off + (uint8_t*)ctx->attr.get());
			vname_len = ctx->attr->value_len/sizeof(ntfs_char_t);
		}
		
		char* vname = nullptr;
		if (volume_name && vname_len)
		{
			vname = detail::NTFS::ntfs_str_to_char(volume_name, vname_len);
			if (!vname)
			{
				vname = new char[vname_len+1];
				
				for (int i = 0; i < vname_len; ++i)
				{
					if (volume_name[i] > 0xFF)
					{
						vname[i] = '_';
					}
					else
					{
						vname[i] = (char)volume_name[i];
					}
				}
				
				vname[vname_len] = '\0';
			}
		}
		else
		{
			vname = new char[1];
			*vname = '\0';
			vname_len = 0;
		}
		
		ntfs->_name = Utils::string(vname);
		delete[] vname;
		
		
		return true;
	}
	
	bool ntfs_setup::setup_root(NTFS* ntfs) noexcept
	{
		TRACE("Getting root...");
		auto r = ntfs->inode_open(NTFS_FILE_ROOT);
		assert(r);
		if (!r)
		{
			return false;
		}
		
		
		auto raw_root = 
		#ifdef __cpp_rtti2
		dynamic_cast
		#else
		static_cast
		#endif
		<NTFSDirectoryNode*>(r.get());
		assert(raw_root);
		if (!raw_root)
		{
			return false;
		}
		
		auto root = Utils::shared_ptr<NTFSDirectoryNode>(r, raw_root);
		
		ntfs->_root = node_ptr<>(root);
		
		return true;
	}
	
	bool ntfs_setup::setup_chars(NTFS* ntfs) noexcept
	{
		assert(ntfs);
		
		auto upni = ntfs->inode_open(NTFS_FILE_UPCASE);
		if (!upni)
		{
			return false;
		}
		
		auto upattr = ntfs->open_attribute(upni, NTFS_ATTR_TYPE_DATA, nullptr, 0);
		if (!upattr)
		{
			return false;
		}
		
		auto attr_rec = upattr->record();
		if (!attr_rec)
		{
			return false;
		}
		if (attr_rec->data_size & ~0x1FFFFFFFFULL)
		{
			return false;
		}
		
		assert(attr_rec->data_size % sizeof(ntfs_char_t) == 0);
		
		size_t left;
		auto data = upattr->read(0, &left);
		if (!data)
		{
			return false;
		}
		
		auto tbl = new ntfs_char_t[attr_rec->data_size/sizeof(ntfs_char_t)];
		if (!tbl)
		{
			return false;
		}
		
		if (left >= attr_rec->data_size)
		{
			memcpy(tbl, data.get(), attr_rec->data_size);
		}
		else
		{
			uint8_t* ptr = (uint8_t*)tbl;
			size_t count = left;
			memcpy(ptr, data.get(), left);
			ptr += left;
			do
			{
				data = upattr->read(count, &left);
				assert(data);
				if (!data)
				{
					delete[] tbl;
					return false;
				}
				
				if (attr_rec->data_size - count > left)
				{
					memcpy(ptr, data.get(), left);
					count += left;
					ptr += left;
				}
				else
				{
					memcpy(ptr, data.get(), attr_rec->data_size - count);
					break;
				}
			}
			while (count < attr_rec->data_size);
		}
		
		
		
		
		
		
		
		ntfs->_upcase = tbl;
		ntfs->upcase_len = attr_rec->data_size;
		
		return true;
	}
	
}