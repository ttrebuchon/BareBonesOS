#ifndef INCLUDED_FS_EXT2NODE_HH
#define INCLUDED_FS_EXT2NODE_HH

#include "EXT2.hh"
#include <Utils/shared_ptr>
#include <Utils/list>

namespace Kernel::FS
{
	
	namespace detail::EXT2
	{
		struct block_t;
		class block_group_t;
	}
	
	class EXT2Node
	{
		public:
		
		protected:
		EXT2* fs;
		Utils::shared_ptr<detail::EXT2::inode_t> node;
		Utils::string inode_name;
		size_t inode_index;
		
		EXT2Node(EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index);
		EXT2Node(EXT2* fs, detail::EXT2::dirent_t*);
		
		
		uint16_t dec_hard_links();
		uint16_t inc_hard_links();
		
		Utils::list<uint32_t> get_file_block_indexes(size_t start, size_t count, bool ignore_zeros = false) const;
		
		// Returns number of blocks freed
		uint32_t release_reserved_blocks();
		
		static uint64_t write_blocks_to_node(EXT2*, detail::EXT2::inode_t*, uint64_t, uint64_t, const uint8_t* buf);
		uint64_t write_blocks(uint64_t, uint64_t, const uint8_t* buf);
		static Utils::list<Utils::shared_ptr<detail::EXT2::block_t>> get_file_blocks(EXT2*, const detail::EXT2::inode_t*, size_t start, size_t count, bool ignore_zeros = false);
		static Utils::list<uint32_t> get_file_block_indexes(EXT2*, const detail::EXT2::inode_t*, size_t start, size_t count, bool ignore_zeros = false);
		static bool add_block_to_node(EXT2*, detail::EXT2::inode_t*, size_t block_index, size_t* placement_index = nullptr) noexcept;
		
		public:
		
		/*virtual ~EXT2Node()
		{
			TRACE(".");
		}*/
		
		uint64_t read(uint64_t, uint64_t, void*);
		uint64_t write(uint64_t, uint64_t, const void*);
		void open();
		void close();
		uint64_t node_size() const noexcept;
		bool add_block(size_t block_index, size_t* placement_index = nullptr) noexcept;
		
		
		Utils::shared_ptr<detail::EXT2::block_group_t> group() const noexcept __attribute__((__pure__));
		Utils::shared_ptr<detail::EXT2::block_t> get_block(const unsigned int index, bool write) noexcept;
		size_t block_count() const noexcept;
		size_t unused_space() const noexcept;
		uint16_t hard_links() const noexcept;
		
		size_t expand_to(const size_t);
		constexpr size_t get_ext2_inode() const
		{ return inode_index; }
		
		
		friend class EXT2Factory;
		friend class EXT2;
	};
	
}

#endif