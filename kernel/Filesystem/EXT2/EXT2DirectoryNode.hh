#ifndef INCLUDED_FS_EXT2DIRECTORYNODE_HH
#define INCLUDED_FS_EXT2DIRECTORYNODE_HH

#include "EXT2Node.hh"
#include <kernel/Filesystem/DirectoryNode.hh>
#include <Utils/vector>

namespace Kernel::FS
{
	
	class EXT2DirectoryNode : public EXT2Node, public DirectoryNode
	{
		public:
		
		protected:
		Utils::vector<Node*> children;
		bool has_read : 1;
		bool has_pending : 1;
		
		
		private:
		void init(DirectoryNode* parent);
		bool initial_read();
		
		public:
		EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name);
		EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, detail::EXT2::dirent_t*);
		
		virtual void addChild(Node*) override;
		virtual size_t size() const noexcept override;
		virtual Node* at(size_t index) const override;
		
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		virtual DirEnt* readDir(uint32_t) override;
		virtual Node* findDir(const char* name) override;
	};
	
}

#endif