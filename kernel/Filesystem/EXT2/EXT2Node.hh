#ifndef INCLUDED_FS_EXT2NODE_HH
#define INCLUDED_FS_EXT2NODE_HH

#include "EXT2.hh"
#include <Utils/shared_ptr>

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
		
		
		
		public:
		
		uint64_t read(uint64_t, uint64_t, uint8_t*);
		uint64_t write(uint64_t, uint64_t, const uint8_t*);
		void open();
		void close();
		uint64_t node_size() const noexcept;
		bool add_block(size_t block_index, size_t* placement_index = nullptr) noexcept;
		
		
		Utils::shared_ptr<detail::EXT2::block_group_t> group() const noexcept __attribute__((__pure__));
		Utils::shared_ptr<detail::EXT2::block_t> get_block(const unsigned int index, bool write) noexcept;
		size_t block_count() const noexcept;
		size_t unused_space() const noexcept;
		
		
		friend class EXT2Factory;
	};
	
}

#endif