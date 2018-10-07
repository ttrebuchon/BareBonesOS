#ifndef INCLUDED_FS_EXT2DIRECTORYNODE_HH
#define INCLUDED_FS_EXT2DIRECTORYNODE_HH

#include "EXT2Node.hh"
#include <kernel/Filesystem/DirectoryNode.hh>
#include <Utils/vector>
#include <kernel/Filesystem/node_ptr.hh>

namespace Kernel::FS
{
	
	class EXT2DirectoryNode : public EXT2Node, public DirectoryNode
	{
		public:
		
		protected:
		Utils::vector<Node*> children;
		Utils::vector<node_ptr<>> children2;
		bool has_read : 1;
		bool has_pending : 1;
		
		
		private:
		void init(DirectoryNode* parent);
		bool initial_read();
		
		public:
		EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index);
		EXT2DirectoryNode(DirectoryNode* parent, EXT2* fs, detail::EXT2::dirent_t*);
		
		virtual Node* add(Node*) override;
		virtual bool remove(Node*) override
		{ assert(NOT_IMPLEMENTED); }
		virtual size_t size() const noexcept override;
		virtual Node* at(size_t index) const override;
		
		virtual uint64_t read(uint64_t, uint64_t, void*) override;
		virtual uint64_t write(uint64_t, uint64_t, const void*) override;
		virtual void open() override;
		virtual void close() override;
		virtual DirEnt* readDir(uint32_t) override;
		virtual Node* findDir(const char* name) override;
		virtual EXT2* get_filesystem() const noexcept override;
		
		
		
		
		virtual node_ptr<> add2(const node_ptr<>&);
		virtual bool remove2(const node_ptr<>&)
		{ assert(NOT_IMPLEMENTED); }
		virtual node_ptr<> at2(size_t index) const;
		virtual node_ptr<> findDir2(const char* name);
		
		
		friend class EXT2Factory;
	};
	
}

#endif