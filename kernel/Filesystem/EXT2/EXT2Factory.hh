#ifndef INCLUDED_FS_EXT2FACTORY_HH
#define INCLUDED_FS_EXT2FACTORY_HH

#include <kernel/Filesystem/NodeFactory.hh>
#include <Utils/shared_ptr>

namespace Kernel::FS
{
	class EXT2;
	class EXT2DirectoryNode;
	class EXT2FileNode;
	class EXT2SymLinkNode;
	
	namespace detail::EXT2
	{
		struct inode_t;
		struct block_t;
		class block_group_t;
		struct dirent_t;
	}
	
	class EXT2Factory : public Factory
	{
		public:
		typedef EXT2DirectoryNode directory_type;
		typedef EXT2FileNode file_type;
		typedef EXT2SymLinkNode link_type;
		
		protected:
		typedef detail::EXT2::inode_t inode_type;
		typedef detail::EXT2::block_t block_t;
		typedef detail::EXT2::block_group_t block_group_t;
		
		EXT2* ext2;
		
		
		Utils::shared_ptr<detail::EXT2::dirent_t> add_child_inode(directory_type* parent, const size_t child, const Utils::string& name) const noexcept;
		
		
		public:
		EXT2Factory(EXT2*);
		
		
		virtual DirectoryNode_v* create_directory(DirectoryNode_v* parent, const Utils::string& name) noexcept override;
		virtual FileNode_v* create_file(DirectoryNode_v* parent, const Utils::string& name) noexcept override;
		virtual LinkNode* create_link(DirectoryNode_v* parent, const Utils::string&, Node* target) noexcept override;
	};
}

#endif