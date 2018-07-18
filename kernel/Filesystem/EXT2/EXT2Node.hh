#ifndef INCLUDED_FS_EXT2NODE_HH
#define INCLUDED_FS_EXT2NODE_HH

#include "EXT2.hh"
#include <Utils/shared_ptr>

namespace Kernel::FS
{
	
	class EXT2Node
	{
		public:
		
		protected:
		EXT2* fs;
		Utils::shared_ptr<detail::EXT2::inode_t> node;
		Utils::string inode_name;
		
		EXT2Node(EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name);
		EXT2Node(EXT2* fs, detail::EXT2::dirent_t*);
		
		uint64_t node_size() const noexcept;
		
		public:
		
		uint64_t read(uint64_t, uint64_t, uint8_t*);
		uint64_t write(uint64_t, uint64_t, const uint8_t*);
		void open();
		void close();
	};
	
}

#endif