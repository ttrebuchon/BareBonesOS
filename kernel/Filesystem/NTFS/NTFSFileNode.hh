#ifndef INCLUDED_NTFSFILENODE_HH
#define INCLUDED_NTFSFILENODE_HH

#include "NTFSNode.hh"
#include <kernel/Filesystem/FileNode.hh>


namespace Kernel::FS
{
	
	class NTFSFileNode : public NTFSNode, public FileNode
	{
		public:
		
		protected:
		
		public:
		
		NTFSFileNode(NTFS*, ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		
		virtual uint64_t read(uint64_t start, uint64_t len, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		
		virtual size_t size() const noexcept override;
		virtual DirectoryNode_v* get_parent() const noexcept override;
		
		virtual Filesystem* get_filesystem() const noexcept override;
		virtual const NTFSFileNode* as_file() const noexcept override
		{ return this; }
		virtual NTFSFileNode* as_file() noexcept override
		{ return this; }
		
		virtual Node* get_inode() noexcept
		{ return this; }
	};
	
}

#endif