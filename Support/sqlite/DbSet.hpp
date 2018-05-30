#if __cplusplus >= 201703L
#ifndef INCLUDED_SUPPORT_SQLITE_DBSET_HPP
#define INCLUDED_SUPPORT_SQLITE_DBSET_HPP

#include "DbSet.hh"

namespace Support { namespace SQLite
{
	
	namespace detail
	{
		template <class TEntity, class Allocator>
		_DbSet<TEntity, void, void, Allocator>::_DbSet(const bool hasDerived, const allocator_type& alloc, size_t bucketHint) :
		key_container(bucketHint, entity_key_selector<entity_type, key_type>(), typename key_container_type::hasher(), Utils::equal_to<key_type>(), alloc, true),
		entity_container(bucketHint, Utils::SelectIdentity(), typename entity_container_type::hasher(), Utils::equal_to<const_value_type>(), alloc),
		staging_container(bucketHint, entity_key_selector<entity_type, key_type>(), typename key_container_type::hasher(), Utils::equal_to<key_type>(), alloc, true),
		owned_objects(alloc),
		alloc(alloc),
		hasDerived(hasDerived)
		{
			
		}
		
		
		
		
	}
	
	
}
}

#endif
#endif