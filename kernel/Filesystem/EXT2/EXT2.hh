#ifndef INCLUDED_FS_EXT2_HH
#define INCLUDED_FS_EXT2_HH

#include <kernel/Filesystem/Filesystem.hh>
#include <kernel/Security/UGO.h>
#include <Utils/map>
#include <Utils/vector>
#include <Utils/shared_ptr>
#include <Utils/weak_ptr>
#include <Support/collections/shared_ptr_cache.hpp>
#include <Utils/BitSet.hh>
#include <Utils/shared_mutex>
#include "EXT2Factory.hh"


namespace Kernel::FS
{
	
	
	class EXT2;
	
	namespace detail::EXT2
	{
		
		struct superblock_t
		{
			uint32_t nodes;
			uint32_t blocks;
			uint32_t su_rsv_blocks;
			uint32_t unallocated_blocks;
			uint32_t unallocated_nodes;
			uint32_t superblock_block_num;
			
			/// block_size == (1024 << block_size_modifier)
			uint32_t block_size_modifier;
			
			/// fragment_size == (1024 << fragment_size_modifier)
			uint32_t fragment_size_modifier;
			
			uint32_t blocks_per_group;
			uint32_t fragments_per_group;
			uint32_t nodes_per_group;
			uint32_t last_mount_time; /// (POSIX)
			uint32_t last_write_time; /// (POSIX)
			
			/// Number of mounts since the 
			/// last consistency check
			uint16_t mounts_since_chk;
			
			/// Number of mounts allowed 
			/// before a consistency check
			/// is required
			uint16_t mounts_before_chk;
			
			
			uint16_t signature;
			uint16_t state;
			uint16_t error_handling;
			uint16_t minor_ver;
			uint32_t last_chk_time; /// (POSIX)
			
			/// Time allowed between forced
			/// consistency checks (POSIX)
			uint32_t chk_interval;
			
			uint32_t OS_ID;
			uint32_t major_ver;
			uint16_t usr_rsv_block_ID;
			uint16_t group_rsv_block_ID;
			
			
		};
		
		
		struct alignas(1024) superblock_ext_t : public superblock_t
		{
			uint32_t first_non_rsv_node;
			uint16_t node_struct_size;
			uint16_t superblock_block_group;
			uint32_t optional_features;
			uint32_t required_features;
			uint32_t rw_required_features;
			uint8_t FS_ID[16];
			uint8_t volume_name[16];
			uint8_t last_mount_path[64];
			uint32_t compression_alg;
			uint8_t file_prealloc_blocks;
			uint8_t dir_prealloc_blocks;
			uint16_t ___unused1;
			uint8_t journal_ID[16];
			uint32_t journal_node;
			uint32_t journal_dev;
			uint32_t orphan_nodes_head;
			
			uint8_t ___unused2[788];
		};
		
		struct superblock_constants
		{
			constexpr static auto ext_major_ver = 1;
			constexpr static uint32_t first_non_rsv_node = 11;
			constexpr static uint16_t node_struct_size = 128;
			constexpr static uint16_t ext2_signature = 0xEF53;
			constexpr static uint32_t offset = 1024;
			
			struct optional_features
			{
				typedef uint32_t type;
				
				constexpr static type preallocate_directories = 0x1;
				constexpr static type AFS_server_inodes = 0x2;
				constexpr static type has_journal = 0x4;
				constexpr static type inodes_ext_attrs = 0x8;
				constexpr static type FS_can_resize = 0x10;
				constexpr static type dirs_use_hash_index = 0x20;
			};
			
			struct required_features
			{
				typedef uint32_t type;
				
				constexpr static type compression = 0x1;
				constexpr static type dir_entries_type_field = 0x2;
				constexpr static type journal_needs_replay = 0x4;
				constexpr static type journal_device = 0x8;
			};
			
			/// Required for read/write but
			/// not to mount as read-only
			struct rw_required_features
			{
				typedef uint32_t type;
				
				constexpr static type sparse_superblocks_and_group_desc_tables = 0x1;
				constexpr static type file_size_64 = 0x2;
				constexpr static type binary_tree_dirs = 0x4;
			};
			
		};
		
		#define EXT2_STATE_CLEAN 0x1
		#define EXT2_STATE_ERROR 0x2
		
		
		
		static_assert(sizeof(superblock_ext_t) == 1024);
		
		
		struct block_group_desc_t
		{
			uint32_t block_usage_map;
			uint32_t inode_usage_map;
			uint32_t inode_table;
			uint16_t unallocated_blocks;
			uint16_t unallocated_inodes;
			uint16_t directories;
			uint8_t ___unused[14];
		};
		
		static_assert(sizeof(block_group_desc_t) == 32);
		
		
		/*enum class NType : uint8_t
		{
			FIFO = 0x1,
			CHAR_DEV = 0x2,
			DIR = 0x4,
			BLOCK_DEV = 0x6,
			FILE = 0x8,
			SYMLINK = 0xA,
			SOCKET = 0xC,
		};*/
		
		struct EXT2_Types_Perms
		{
			union
			{
				union
				{
					UGO_t perms;
					struct
					{
						uint16_t ____ignore : 12;
						uint16_t type : 4;
					};
				};
				uint16_t raw;
			};
		};
		
		
		
		static_assert(sizeof(EXT2_Types_Perms) == 2);
		
		
		
		
		struct inode_t
		{
			union
			{
				union
				{
					UGO_t perms;
					struct
					{
						uint16_t _____ignore : 12;
						uint16_t type : 4;
					};
				};
				struct
				{
					uint16_t raw_perms : 12;
					uint16_t _____ignore2 : 4;
				};
			};
			//EXT2_Types_Perms type_perms;
			uint16_t user_ID;
			uint32_t size_low;
			uint32_t access_time;
			uint32_t create_time;
			uint32_t modify_time;
			uint32_t delete_time;
			uint16_t group_ID;
			uint16_t hard_links;
			uint32_t sector_count;
			uint32_t flags;
			uint32_t OS_1;
			union
			{
				struct
				{
					uint32_t direct[12];
					uint32_t indirect_1;
					uint32_t indirect_2;
					uint32_t indirect_3;
				};
				char data[60];
			};
			uint32_t gen_num;
			uint32_t ext_attr;
			union
			{
				uint32_t size_high;
				uint32_t directory_ACL;
			};
			uint32_t fragment;
			uint32_t OS_2[3];
			
			unsigned char extra[];
		};
		
		
		/*
		#define EXT2_INODE_TYPE_FIFO 0x1000
		#define EXT2_INODE_TYPE_CHAR_DEV 0x2000
		#define EXT2_INODE_TYPE_DIR 0x4000
		#define EXT2_INODE_TYPE_BLOCK_DEV 0x6000
		#define EXT2_INODE_TYPE_FILE 0x8000
		#define EXT2_INODE_TYPE_SYMLINK 0xA000
		#define EXT2_INODE_TYPE_SOCK 0xC000
		*/
		
		#define EXT2_INODE_TYPE_FIFO 0x1
		#define EXT2_INODE_TYPE_CHAR_DEV 0x2
		#define EXT2_INODE_TYPE_DIR 0x4
		#define EXT2_INODE_TYPE_BLOCK_DEV 0x6
		#define EXT2_INODE_TYPE_FILE 0x8
		#define EXT2_INODE_TYPE_SYMLINK 0xA
		#define EXT2_INODE_TYPE_SOCK 0xC
		
		
		#define EXT2_INODE_STICKY_BIT 0x200
		#define EXT2_INODE_PERMS_SETGID 0x400
		#define EXT2_INODE_PERMS_SETUID 0x800
		
		enum class dirent_type_t : uint8_t
		{
			unknown = 0x0,
			regular = 0x1,
			directory = 0x2,
			char_dev = 0x3,
			block_dev = 0x4,
			FIFO = 0x5,
			socket = 0x6,
			symlink = 0x7
		};
		
		struct dirent_t
		{
			uint32_t inode;
			uint16_t entry_size;
			uint8_t name_len_lsb;
			union
			{
				dirent_type_t type;
				uint8_t name_len_msb;
			};
			unsigned char name[];
		};
		
		dirent_t* next_dirent(dirent_t* array, size_t array_size, dirent_t* current, bool allow_empty = false);
		dirent_t* allocate_dirent(dirent_t* array, size_t array_size, size_t name_len, bool type_field);
		
		
		class block_group_t;
		
		
		struct block_t
		{
			block_group_t* group;
			uint16_t index : 15;
			volatile bool modified : 1;
			//size_t len;
			uint8_t data[];
			
			block_t();
			~block_t();
			
			void operator delete(void*);
		} __attribute__((__packed__));
		
		
		class block_group_t
		{
			public:
			typedef Utils::shared_timed_mutex mutex_type;
			
			protected:
			typedef Utils::shared_lock<mutex_type> read_lock;
			typedef Utils::unique_lock<mutex_type> write_lock;
			
			const uint32_t group_index;
			mutable mutex_type mut;
			mutable Utils::mutex cache_m;
			FS::EXT2* fs;
			volatile block_group_desc_t* gd;
			Utils::shared_ptr<block_t> block_usage_blk;
			Utils::Bitset_Ptr<uint8_t> block_usage;
			Utils::shared_ptr<block_t> node_usage_blk;
			Utils::Bitset_Ptr<uint8_t> node_usage;
			Utils::vector<bool> modified_map;
			uint32_t blocks_per_group;
			uint32_t nodes_per_group;
			mutable Utils::map<uint32_t, Utils::weak_ptr<block_t>> cached_blocks;
			Utils::map<uint32_t, Node*> nodes;
			mutable Support::Collections::shared_ptr_cache<block_t, Utils::shared_ptr> block_ptr_cache;
			
			
			Utils::shared_ptr<block_t> get_block_internal(const size_t) noexcept;
			Utils::shared_ptr<block_t> get_block_internal_no_lock(const size_t, read_lock&) noexcept;
			Utils::shared_ptr<block_t> get_block_internal_no_lock(size_t, write_lock&, bool no_unlock = false) noexcept;
			Utils::shared_ptr<inode_t> get_node_internal(const size_t) noexcept;
			Utils::shared_ptr<inode_t> get_node_internal_no_lock(const size_t, read_lock&) noexcept;
			Utils::shared_ptr<inode_t> get_node_internal_no_lock(size_t, write_lock&, bool no_unlock = false) noexcept;
			
			size_t inode_block_index(const size_t) const noexcept;
			
			
			
			private:
			volatile bool initted;
			void init();
			void init() const;
			size_t free_block_no_lock() const;
			size_t free_node_no_lock() const;
			
			void cache_block(const Utils::shared_ptr<block_t>&, write_lock&, bool no_unlock = false) noexcept;
			void cache_block(const Utils::shared_ptr<block_t>&, read_lock&, bool no_unlock = false) noexcept;
			Utils::shared_ptr<inode_t> get_node_internal_already_locked(const size_t) noexcept;
			
			public:
			
			constexpr static size_t npos = size_t(-1);
			
			block_group_t(FS::EXT2&, block_group_desc_t&, const uint32_t group);
			~block_group_t();
			
			
			bool node_is_free(const size_t i) const;
			bool block_is_free(const size_t) const;
			
			size_t blocks_size() const noexcept;
			size_t inodes_size() const noexcept;
			
			size_t free_node() const;
			size_t free_block() const;
			
			size_t free_node_count() const noexcept;
			size_t free_block_count() const noexcept;
			
			uint16_t directories() const noexcept;
			
			Utils::shared_ptr<block_t> reserve_block(size_t& index);
			bool release_block(const size_t index);
			Utils::shared_ptr<inode_t> reserve_node(size_t& index);
			bool release_node(const size_t index);
			
			bool block_is_modified(const size_t) const;
			bool node_is_modified(const size_t) const;
			
			
			void write_back() noexcept;
			void commit_block(block_t*) noexcept;
			
			Utils::shared_ptr<block_t> get_block(const size_t index, bool write = false);
			
			Utils::shared_ptr<inode_t> allocate_directory(size_t& index) noexcept;
			Utils::shared_ptr<inode_t> allocate_file(size_t& index) noexcept;
			
			bool mark_node_modified(const size_t);
			bool mark_block_modified(const size_t);
		};
	}
	
	
	class DirectoryNode;
	
	class EXT2 : public Filesystem
	{
		public:
		typedef uint32_t inode_index_type;
		typedef uint32_t block_index_type;
		typedef uint32_t group_index_type;
		
		protected:
		typedef inode_index_type inode_index_t;
		typedef block_index_type block_index_t;
		typedef group_index_type group_index_t;
		typedef detail::EXT2::inode_t inode_type;
		typedef detail::EXT2::block_t block_t;
		typedef detail::EXT2::block_group_t block_group_t;
		
		EXT2Factory _factory;
		Drivers::Disk* disk;
		detail::EXT2::superblock_ext_t sb;
		detail::EXT2::block_group_desc_t* bg_table;
		group_index_t block_group_count;
		mutable Utils::map<block_index_t, Utils::weak_ptr<block_t>> cached_blocks;
		size_t max_node_blocks;
		Utils::map<inode_index_t, Node*> nodes;
		mutable Support::Collections::shared_ptr_cache<block_t, Utils::shared_ptr> block_ptr_cache;
		Utils::vector<Utils::shared_ptr<block_group_t>> groups;
		
		
		group_index_t calc_inode_group(inode_index_t) const;
		block_index_t calc_inode_group_block(inode_index_t) const;
		Utils::shared_ptr<inode_type> get_inode(inode_index_t) noexcept;
		inode_index_t inodes_per_block() const noexcept;
		Utils::shared_ptr<block_t> get_block(block_index_t);
		Utils::shared_ptr<const block_t> get_block(block_index_t) const;
		uint8_t* read_inode(const inode_type*, size_t first_blk, size_t count);
		uint8_t* read_inode(Utils::shared_ptr<const inode_type>, size_t first_blk, size_t count);
		bool read_indirect(const uint32_t*, const size_t count, uint8_t* target) const;
		
		Node* parse_node(DirectoryNode* parent, detail::EXT2::dirent_t*);
		
		bool extended_superblock() const noexcept;
		bool has_journal() const noexcept;
		size_t read_block(const size_t index, uint8_t* buffer) const noexcept;
		size_t write_block(const size_t index, const uint8_t* buffer) noexcept;
		Utils::shared_ptr<inode_type> reserve_inode(size_t& index) noexcept;
		Utils::shared_ptr<block_t> reserve_block(size_t& index) noexcept;
		bool release_inode(const size_t index) noexcept;
		bool release_block(const size_t index) noexcept;
		
		private:
		Utils::shared_ptr<inode_type> __get_inode(inode_index_t) const noexcept;
		size_t lba_from_block(block_index_t) const noexcept;
		uint64_t inode_size(const inode_type*) const noexcept;
		
		public:
		EXT2(Drivers::Disk&, int partition = -1) noexcept;
		
		virtual ~EXT2();
		
		size_t inode_size() const noexcept;
		size_t block_size() const noexcept;
		size_t group_size() const noexcept;
		size_t fragment_size() const noexcept;
		
		group_index_type group_count() const noexcept
		{ return block_group_count; }
		uint32_t block_count() const noexcept
		{ return sb.blocks; }
		uint32_t free_blocks() const noexcept;
		
		Utils::shared_ptr<const detail::EXT2::inode_t> get_inode(inode_index_t) const noexcept;
		
		
		Utils::shared_ptr<block_group_t> get_group(size_t i)
		{
			assert(i < block_group_count);
			return groups.at(i);
		}
		
		virtual EXT2Factory& factory() noexcept override
		{
			return _factory;
		}
		
		size_t dirent_name_len(const detail::EXT2::dirent_t*) const noexcept;
		
		
		static bool Format(Drivers::Disk* part);
		
		
		friend class EXT2Node;
		friend class EXT2FileNode;
		friend class EXT2DirectoryNode;
		friend class detail::EXT2::block_group_t;
		friend class EXT2Factory;
	};
	
	
}


#endif