#ifndef INCLUDED_FS_NTFS_HH
#define INCLUDED_FS_NTFS_HH

#include "types.h"
#include "../Filesystem.hh"
#include <Utils/shared_ptr>
#include <Utils/weak_ptr>
#include <Utils/map>
#include <Utils/list>
#include <Support/collections/shared_ptr_cache.hpp>
#include <Utils/shared_mutex>

#define NTFS_MREF_MASK 0x0000FFFFFFFFFFFFULL

namespace Kernel::FS
{
	
	class NTFS;
	class NTFSFactory;
	class NTFSNode;
	class MFTNode;
	class NTFSAttribute;
	
	#define INL __attribute__((__always_inline__))
	
	
	
	namespace detail::NTFS
	{
		typedef int64_t vcn_t;
		typedef int64_t lcn_t;
		
		struct block_t
		{
			private:
			bool _modified;
			block_t(const Utils::shared_ptr<block_t>&, uint8_t* data, size_t indx, bool modified) noexcept;
			block_t(size_t indx, size_t len, bool modified) noexcept;
			
			public:
			typedef Utils::shared_ptr<block_t> pointer_type;
			size_t index;
			size_t len;
			pointer_type base;
			uint8_t* data;
			
			
			~block_t();
			//void operator delete(void*);
			
			bool modified() const noexcept
			__attribute__((__always_inline__))
			{
				if (base)
				{
					return base->modified();
				}
				else
				{
					return _modified;
				}
			}
			void modified(bool v) noexcept
			{
				_modified |= v;
				if (base && v)
				{
					base->modified(true);
				}
			}
			
			static pointer_type make_block(size_t indx, size_t len, bool modified) noexcept;
			static pointer_type attach_block(const Utils::shared_ptr<block_t>&, uint8_t* data, size_t indx, bool modified) noexcept;
			
			
		};// __attribute__((__packed__));
		
		
		
		
		
		
		
		struct attr_search_t
		{
			const FS::NTFS* ntfs;
			Utils::shared_ptr<ntfs_mft_record_t> mrec;
			Utils::shared_ptr<ntfs_attr_record_t> attr;
			bool is_first;
			Utils::shared_ptr<NTFSNode> inode;
			Utils::shared_ptr<ntfs_attrlist_entry_t> ent;
			Utils::shared_ptr<NTFSNode> base_inode;
			Utils::shared_ptr<ntfs_mft_record_t> base_mrec;
			Utils::shared_ptr<ntfs_attr_record_t> base_attr;
			
			
			
			
			
			attr_search_t(const FS::NTFS*, Utils::shared_ptr<NTFSNode>, Utils::shared_ptr<ntfs_mft_record_t>) noexcept;
			
			
			
			Utils::shared_ptr<NTFSAttribute> open_attribute(const ntfs_attr_type, ntfs_char_t* name = nullptr, uint32_t name_len = 0);
			
			
			
			bool lookup_attribute(const ntfs_attr_type, ntfs_char_t* name, uint32_t name_len, bool ignore_case, const size_t cluster_no, const uint8_t* val, const uint32_t val_len) noexcept;
			bool find_attribute(const ntfs_attr_type, ntfs_char_t* name, uint32_t name_len, bool ignore_case, const uint8_t* val, const uint32_t val_len) noexcept;
			bool find_external_attribute(const ntfs_attr_type, ntfs_char_t* name, uint32_t name_len, bool ignore_case, const size_t cluster_no, const uint8_t* val, const uint32_t val_len) noexcept;
			
			void reset() noexcept;
		};
		
		
		/*struct region_t
		{
			typedef size_t n_type;
			n_type pos;
			n_type size;
		};
		
		
		
		struct region_compare
		{
			typedef typename region_t::n_type n_type;
			
			bool operator()(const region_t& lhs, const region_t& rhs) const noexcept INL
			{
				if (lhs.pos < rhs.pos)
				{
					return (rhs.pos - lhs.pos >= lhs.size);
				}
				else
				{
					return false;
				}
			}
			
			bool operator()(const region_t& lhs, const n_type rhs) const noexcept INL
			{
				if (lhs.pos < rhs)
				{
					return (rhs - lhs.pos > lhs.size);
				}
				else
				{
					return false;
				}
			}
			
			bool operator()(const n_type lhs, const region_t& rhs) const noexcept INL
			{
				return !(*this)(rhs, lhs);
			}
			
			
		};*/
		
		
		
		struct list_ent_t
		{
			Utils::shared_ptr<Utils::shared_ptr<block_t>[]> blocks;
			size_t block_len;
			vcn_t vcn;
			lcn_t lcn;
			size_t length;
			
			
			uint8_t* get_data(size_t* remaining_in_block, bool mark_modified) noexcept;
		};
		
		
		ntfs_char_t* ntfs_uni_cstr_clone(const ntfs_char_t*, const uint32_t len) noexcept;
		char* ntfs_str_to_char(const ntfs_char_t* vname, const uint32_t length) noexcept;
		inline Utils::string ntfs_str_to_str(const ntfs_char_t* vname, const uint32_t length) noexcept INL;
		inline Utils::string ntfs_str_to_str(const ntfs_char_t* vname, const uint32_t length) noexcept
		{
			auto cstr = ntfs_str_to_char(vname, length);
			if (cstr)
			{
				Utils::string str(cstr);
				delete[] cstr;
				return str;
			}
			else
			{
				return "";
			}
		}
		
		
		
		typedef enum
		{
			NTFS_LCN_HOLE = -1,
			NTFS_LCN_LI_NOT_MAPPED = -2,
			NTFS_LCN_ENOENT = -3,
			NTFS_LCN_EINVAL = -4,
		}
		lcn_special_val;
		
		
		uint16_t mft_seq_num(const uint64_t) noexcept;
		
		constexpr uint64_t mft_ref(const auto in) noexcept
		{
			return (uint64_t)((in) & NTFS_MREF_MASK);
		}
	}
	
	
	
	
	enum class NTFSState : uint64_t
	{
		Compression,
		
	};
	
	ENUM_OPS(NTFSState, uint64_t);
	
	class NTFS : public Filesystem
	{
		public:
		// The pointer type for raw NTFS
		// data structures
		template <class T>
		using ptr = Utils::shared_ptr<T>;
		
		// The weak pointer type for raw
		// NTFS data structures
		template <class T>
		using wptr = Utils::weak_ptr<T>;
		
		typedef Utils::shared_timed_mutex mutex_type;
		
		
		
		
		protected:
		typedef typename detail::NTFS::block_t block_type;
		typedef Utils::shared_lock<mutex_type> read_lock;
		typedef Utils::unique_lock<mutex_type> write_lock;
		typedef typename detail::NTFS::list_ent_t li_t;
		typedef Utils::shared_ptr<Utils::vector<li_t>> li_ptr;
		typedef Utils::shared_ptr<const Utils::vector<li_t>> li_const_ptr;
		
		// Synchronization Members
		mutable mutex_type mut;
		mutable Utils::mutex cache_m;
		
		
		
		
		Drivers::Disk* disk;
		NTFSFactory* _factory;
		
		// Data
		ntfs_boot_t boot;
		size_t clu_sz;
		size_t mft_rec_size;
		size_t index_rec_size;
		size_t cluster_count;
		size_t mft_zn_st;
		size_t mft_zn_end;
		size_t mft_zn_pos;
		size_t full_zones;
		size_t d1_zn_pos;
		size_t d2_zn_pos;
		size_t mft_data_pos;
		uint8_t version[2];
		Utils::string _name;
		
		
		size_t MFT_mirror_size() const noexcept;
		size_t sector_size() const noexcept INL
		{
			return boot.bytes_per_sec;
		}
		
		size_t MFT_cluster_index() const noexcept INL
		{
			return boot.MFT_cluster_no;
		}
		
		
		
		// Filesystem Members
		Utils::shared_ptr<MFTNode> MFT;
		Utils::shared_ptr<NTFSAttribute> MFT_data;
		Utils::shared_ptr<NTFSAttribute> MFT_bmap;
		Utils::shared_ptr<NTFSNode> MFT_m;
		Utils::shared_ptr<NTFSAttribute> MFT_m_data;
		Utils::shared_ptr<NTFSAttribute> MFT_m_bmap;
		NTFSState state;
		Utils::map<uint64_t, Utils::shared_ptr<NTFSNode>> nodes;
		ntfs_char_t* _upcase;
		uint32_t upcase_len;
		ntfs_char_t* _lowcase;
		uint32_t lowcase_len;
		
		
		// Data Block Infrastructure
		mutable Utils::map<size_t, wptr<block_type>> blocks;
		mutable Support::Collections::shared_ptr_cache<block_type, ptr> block_ptr_cache;
		
		
		ptr<block_type> get_block(size_t index, bool writing = false);
		ptr<ntfs_mft_record_t> get_mft_record(size_t index, bool writing = false);
		
		
		detail::NTFS::attr_search_t* attr_search(ptr<NTFSNode>, ptr<ntfs_mft_record_t>) const;
		Utils::shared_ptr<NTFSAttribute> open_attribute(Utils::shared_ptr<NTFSNode>, const ntfs_attr_type, ntfs_char_t* name = nullptr, uint32_t name_len = 0) const;
		
		
		li_ptr mapping_pairs_decomp(const Utils::shared_ptr<const ntfs_attr_record_t>&, li_ptr old) const noexcept;
		
		
		
		private:
		NTFS(Drivers::Disk&);
		
		
		ptr<block_type> get_block_internal(size_t index, read_lock&) noexcept;
		ptr<block_type> get_block_internal(size_t index, write_lock&, bool no_unlock = false) noexcept;
		void cache_block(ptr<block_type>, read_lock&, bool no_unlock = false) const;
		void cache_block(ptr<block_type>, write_lock&, bool no_unlock = false) const;
		bool check_mft_record(size_t index, ptr<ntfs_mft_record_t>) const;
		
		public:
		
		
		virtual ~NTFS();
		
		
		constexpr uint8_t major_ver() const noexcept INL
		{ return version[0]; }
		constexpr uint8_t minor_ver() const noexcept INL
		{ return version[1]; }
		
		constexpr bool has_state(const NTFSState s) const noexcept INL
		{ return (state & s) > 0; }
		constexpr size_t cluster_size() const noexcept INL
		{ return clu_sz; }
		
		const ntfs_char_t* upcase(uint32_t* len = nullptr) const noexcept;
		const ntfs_char_t* lowcase(uint32_t* len = nullptr) const noexcept;
		Utils::shared_ptr<NTFSNode> inode_open(uint64_t mref) noexcept;
		
		
		
		virtual Factory& factory() noexcept override;
		
		static NTFS* Open(Drivers::Disk&) noexcept;
		
		
		
		friend class ntfs_setup;
		friend class NTFSAttribute;
	};
	
	
	#undef INL
	
}




#endif