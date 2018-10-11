#ifndef INCLUDED_NTFSROOTDIRECTORYNODE_HH
#define INCLUDED_NTFSROOTDIRECTORYNODE_HH

#include "NTFSDirectoryNode.hh"
#include "MFT.hh"
#include <Utils/shared_ptr>


namespace Kernel::FS
{
	
	class NTFSRootDirectoryNode : public NTFSDirectoryNode
	{
		public:
		
		protected:
		constexpr static size_t sys_node_count = 12;
		Utils::shared_ptr<MFTNode> MFT;
		Utils::shared_ptr<NTFSNode> MFT_m;
		Utils::shared_ptr<NTFSNode> upCase;
		Utils::shared_ptr<NTFSNode> lowCase;
		Utils::shared_ptr<NTFSNode> attrDefs;
		Utils::shared_ptr<NTFSNode> logFile;
		Utils::shared_ptr<NTFSNode> _volume;
		Utils::shared_ptr<NTFSNode> bitmap;
		Utils::shared_ptr<NTFSNode> boot;
		Utils::shared_ptr<NTFSNode> secure;
		Utils::shared_ptr<NTFSNode> extend;
		Utils::shared_ptr<NTFSNode> badClus;
		
		
		NTFSRootDirectoryNode(NTFS*, ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		virtual bool init() override;
		
		public:
		
		
		
		virtual size_t size() const noexcept override;
        virtual node_ptr<> at(size_t index) const override;
        virtual DirectoryNode_v* get_parent() const noexcept override;
        virtual Path_t get_path() const override;
		
		
		virtual const NTFSRootDirectoryNode* as_directory() const noexcept override
		{ return this; }
		virtual NTFSRootDirectoryNode* as_directory() noexcept override
		{ return this; }
		
		friend class NTFS;
		friend class ntfs_setup;
	};
	
}

#endif