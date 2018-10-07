#ifndef INCLUDED_NTFSSYSTEMNODE_HH
#define INCLUDED_NTFSSYSTEMNODE_HH

#include "NTFSNode.hh"
#include <kernel/Filesystem/Node.hh>

namespace Kernel::FS
{
	class NTFSSystemNode : public NTFSNode, public Node
	{
		public:
		
		protected:
		NTFSSystemNode(NTFS*, ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		
		public:
		
		
		virtual uint64_t read(uint64_t start, uint64_t len, void*) override;
		virtual uint64_t write(uint64_t, uint64_t, const void*) override;
		virtual void open() override {}
		virtual void close() override {}
		
		
		virtual Filesystem* get_filesystem() const noexcept override;
		
		
		virtual NTFSSystemNode* get_inode() noexcept override
		{ return this; }
		
		
		friend class NTFS;
	};
}

#endif