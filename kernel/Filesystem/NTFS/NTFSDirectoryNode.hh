#ifndef INCLUDED_NTFSDIRECTORYNODE_HH
#define INCLUDED_NTFSDIRECTORYNODE_HH

#include "NTFSNode.hh"
#include <kernel/Filesystem/DirectoryNode.hh>
#include <Utils/vector>


namespace Kernel::FS
{
	
	class NTFSDirectoryNode : public NTFSNode, public DirectoryNode
	{
		public:
		
		protected:
		Utils::vector<Node*> children;
		bool has_read : 1;
		bool has_pending : 1;
		virtual bool init();
		bool init_const() const
		{
			return const_cast<NTFSDirectoryNode*>(this)->init();
		}
		
		public:
		
		NTFSDirectoryNode(NTFS*, ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		
		virtual uint64_t read(uint64_t start, uint64_t len, void*) override;
		virtual uint64_t write(uint64_t, uint64_t, const void*) override;
		virtual void open() override;
		virtual void close() override;
		
		virtual Node* add(Node*) override;
		virtual bool remove(Node*) override
		{ assert(NOT_IMPLEMENTED); }
		virtual size_t size() const noexcept override;
        virtual Node* at(size_t index) const override;
        virtual DirectoryNode_v* get_parent() const noexcept override;
		
		virtual Filesystem* get_filesystem() const noexcept override;
		virtual const NTFSDirectoryNode* as_directory() const noexcept override
		{ return this; }
		virtual NTFSDirectoryNode* as_directory() noexcept override
		{ return this; }
		
		virtual Node* get_inode() noexcept
		{ return this; }
	};
	
}

#endif