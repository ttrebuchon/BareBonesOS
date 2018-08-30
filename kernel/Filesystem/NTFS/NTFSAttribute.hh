#ifndef INCLUDED_FS_NTFSATTRIBUTE_HH
#define INCLUDED_FS_NTFSATTRIBUTE_HH

#include "types.h"
#include <Utils/shared_ptr>
#include <Utils/weak_ptr>
#include <Utils/vector>

namespace Kernel::FS
{
	namespace detail::NTFS
	{
		struct list_ent_t;
		struct block_t;
	}
	class NTFSNode;
	
	enum class NTFSAttributeState : uint64_t
	{
		Initialized,
		NonResident,
		FullyMapped,
		Appending,
		CompressClosing,
		DirtyRunlist,
	};
	
	ENUM_OPS(NTFSAttributeState, uint64_t);
	
	class NTFSAttribute
	{
		public:
		typedef uint64_t state_type;
		typedef uint16_t flags_type;
		
		protected:
		typedef typename detail::NTFS::list_ent_t li_t;
		typedef typename detail::NTFS::block_t block_type;
		Utils::weak_ptr<NTFSNode> _node;
		Utils::shared_ptr<ntfs_attr_record_t> _record;
		ntfs_char_t* _name;
		uint32_t name_len;
		NTFSAttributeState _state;
		ntfs_attr_type _type;
		Utils::shared_ptr<Utils::vector<li_t>> li;
		
		
		
		int map_runlist(int64_t vcn);
		
		Utils::shared_ptr<uint8_t> read_compressed(const size_t off, size_t* size_left_in_block);
		Utils::shared_ptr<li_t> find_vcn(const size_t vcn);
		
		
		private:
		int64_t runlist_vcn_to_lcn(int64_t vcn) const noexcept;
		
		public:
		NTFSAttribute(Utils::shared_ptr<NTFSNode>, const ntfs_attr_type type, ntfs_char_t* name, uint32_t name_len);
		
		
		Utils::shared_ptr<NTFSNode> node() const noexcept;
		bool encrypted() const noexcept;
		bool sparse() const noexcept;
		bool compressed() const noexcept;
		bool non_resident() const noexcept;
		flags_type flags() const noexcept;
		
		void init(Utils::shared_ptr<ntfs_attr_record_t>) noexcept;
		
		
		
		
		constexpr const ntfs_char_t* name(uint32_t* len = nullptr) const noexcept __attribute__((__always_inline__))
		{
			if (len)
			{
				*len = name_len;
			}
			
			return _name;
		}
		
		constexpr NTFSAttributeState state() const noexcept __attribute__((__always_inline__))
		{
			return _state;
		}
		
		constexpr ntfs_attr_type type() const noexcept __attribute__((__always_inline__))
		{
			return _type;
		}
		
		Utils::shared_ptr<ntfs_attr_record_t> record() const noexcept __attribute__((__always_inline__))
		{
			return _record;
		}
		
		Utils::shared_ptr<uint8_t> read(const size_t off, size_t* size_left_in_block = nullptr);
		
		
		size_t size() const noexcept;
		
		
		friend class NTFS;
		friend class ntfs_setup;
	};
	
}

#endif