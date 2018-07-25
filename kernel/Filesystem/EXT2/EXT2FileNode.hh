#ifndef INCLUDED_FS_EXT2FILENODE_HH
#define INCLUDED_FS_EXT2FILENODE_HH

#include "EXT2Node.hh"
#include <kernel/Filesystem/FileNode.hh>

namespace Kernel::FS
{
	
	class EXT2FileNode : public EXT2Node, public FileNode
	{
		public:
		
		protected:
		
		private:
		void init(DirectoryNode_v* parent);
		
		public:
		EXT2FileNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index);
		EXT2FileNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t*);
		
		
		virtual size_t size() const noexcept override;
		
		
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		virtual DirEnt* readDir(uint32_t) override;
		virtual Node* findDir(const char* name) override;
		virtual EXT2* get_filesystem() const noexcept override;
	};
	
}

#endif