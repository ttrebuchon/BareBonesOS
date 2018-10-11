#ifndef INCLUDED_FS_EXT2SYMLINKNODE_HH
#define INCLUDED_FS_EXT2SYMLINKNODE_HH

#include "EXT2Node.hh"
#include <kernel/Filesystem/LinkNode.hh>
#include <kernel/Filesystem/node_ptr.hh>

namespace Kernel::FS
{
	
	
	
	
	/*class EXT2DirectorySymLinkNode : public virtual EXT2Node, public virtual DirectoryNode_v
	{
		public:
		
		protected:
		//DirectoryNode* _target;
		
		virtual Node* get_target() const noexcept = 0;
		
		
		private:
		void init(DirectoryNode_v* parent);
		
		public:
		EXT2DirectorySymLinkNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name);
		EXT2DirectorySymLinkNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t*);
		
		virtual void addChild(Node*) override;
		virtual size_t size() const noexcept override;
		virtual Node* at(size_t index) const override;
		
		virtual uint32_t read(uint32_t, uint32_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		virtual DirEnt* readDir(uint32_t) override;
		virtual Node* findDir(const char* name) override;
		
		
		DirectoryNode_v* target() const noexcept __attribute__((__pure__));
	};
	
	
	
	
	class EXT2FileSymLinkNode : public virtual EXT2Node, public virtual FileNode_v
	{
		public:
		
		protected:
		//Node* _target;
		
		virtual Node* get_target() const noexcept = 0;
		
		
		private:
		void init(DirectoryNode_v* parent);
		
		public:
		EXT2FileSymLinkNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name);
		EXT2FileSymLinkNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t*);
		
		virtual size_t size() const noexcept override;
		
		FileNode_v* target() const noexcept __attribute__((__pure__));
	};
	
	
	
	
	
	
	class EXT2SymLinkNode : public virtual EXT2DirectorySymLinkNode, public virtual EXT2FileSymLinkNode
	{
		public:
		
		protected:
		mutable Node* _target;
		
		virtual Node* get_target() const noexcept override;
		
		//virtual void init_type() noexcept override;
		
		public:
		EXT2SymLinkNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name);
		EXT2SymLinkNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t*);
		
		virtual NodeType type() const noexcept override;
		
		
		virtual uint64_t read(uint64_t, uint64_t, uint8_t*) override;
		virtual uint64_t write(uint64_t, uint64_t, const uint8_t*) override;
		virtual void open() override;
		virtual void close() override;
		virtual DirEnt* readDir(uint32_t) override;
		virtual Node* findDir(const char* name) override;
		
		
		virtual const FileNode_v* as_file() const noexcept override
		{
			auto t = target();
			return t ? t->as_file() : nullptr;
		}
		
		virtual FileNode_v* as_file() noexcept override
		{
			auto t = target();
			return t ? t->as_file() : nullptr;
		}
		
		virtual const DirectoryNode_v* as_directory() const noexcept override
		{
			auto t = target();
			return t ? t->as_directory() : nullptr;
		}
		
		virtual DirectoryNode_v* as_directory() noexcept override
		{
			auto t = target();
			return t ? t->as_directory() : nullptr;
		}
		
		
		const Utils::string target_path() const;
		Node* target() const noexcept __attribute__((__pure__));
		
	};*/
	
	
	class EXT2SymLinkNode : public EXT2Node, public LinkNode
	{
		public:
		
		protected:
		Path_t _path;
		
		//virtual void init_type() noexcept override;
		
		private:
		void init(DirectoryNode_v* parent) noexcept;
		Utils::string read_path() const;
		
		public:
		EXT2SymLinkNode(DirectoryNode_v* parent, EXT2* fs, Utils::shared_ptr<detail::EXT2::inode_t> node, const Utils::string& name, const size_t inode_index);
		EXT2SymLinkNode(DirectoryNode_v* parent, EXT2* fs, detail::EXT2::dirent_t*);
		
		virtual node_ptr<> target() const noexcept override;
		
		const Path_t& target_path() const noexcept;
		virtual EXT2* get_filesystem() const noexcept override;
	};
	
}

#endif