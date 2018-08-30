#ifndef INCLUDED_FS_NTFS_NTFSNODE_HH
#define INCLUDED_FS_NTFS_NTFSNODE_HH

#include "types.h"
#include <Utils/shared_ptr>
#include <Utils/weak_ptr>
#include <Utils/string>


namespace Kernel::FS
{
	class NTFS;
	class Node;
	class NTFSAttribute;
	
	namespace detail::NTFS
	{
		struct attr_search_t;
	}
	
	class NTFSNode
	{
		public:
		// The pointer type for raw NTFS
		// data structures
		template <class T>
		using ptr = Utils::shared_ptr<T>;
		
		
		protected:
		NTFS* ntfs;
		uint64_t mft_no;
		ptr<ntfs_mft_record_t> mft_rec;
		uint64_t state;
		ntfs_file_attr_flags flags;
		ptr<ntfs_stdinfo_t> _info;
		mutable Utils::weak_ptr<NTFSNode> __weak_self;
		Utils::shared_ptr<NTFSAttribute> _data;
		mutable bool has_parent_mft;
		mutable uint64_t cached_parent_mft;
		
		
		NTFSNode(NTFS*, ptr<ntfs_mft_record_t>, ptr<ntfs_stdinfo_t>);
		
		
		bool set_weak(Utils::shared_ptr<NTFSNode>);
		
		Utils::shared_ptr<NTFSNode> this_ptr()
		{
			return __weak_self.lock();
		}
		Utils::shared_ptr<const NTFSNode> this_ptr() const
		{
			return __weak_self.lock();
		}
		
		Utils::string get_name();
		Utils::basic_string<ntfs_char_t> get_uni_name();
		
		bool init_data() noexcept;
		bool init_data() const noexcept
		{
			return const_cast<NTFSNode*>(this)->init_data();
		}
		
		private:
		Utils::shared_ptr<ntfs_filename_attr_t> get_name_raw();
		
		public:
		
		
		virtual ~NTFSNode() = default;
		
		/*constexpr ntfs_file_attr_flags flags() const noexcept
		{ return _flags; }*/
		
		constexpr NTFS* volume() const noexcept
		{
			return ntfs;
		}
		
		constexpr uint64_t mft_index() const noexcept
		{
			return mft_no;
		}
		
		
		
		uint64_t n_read(uint64_t start, uint64_t len, uint8_t*);
		uint64_t n_write(uint64_t, uint64_t, const uint8_t*);
		size_t n_size() const noexcept;
		virtual Node* get_inode() noexcept
		{ return nullptr; }
		
		uint64_t get_parent_mft() const noexcept;
		Utils::shared_ptr<NTFSNode> open_parent() const;
		
		friend struct detail::NTFS::attr_search_t;
		friend class ntfs_setup;
		friend class NTFS;
		
	};
}

#endif