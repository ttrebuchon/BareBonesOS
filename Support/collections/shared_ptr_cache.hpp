#ifndef INCLUDED_SUPPORT_COLLECTIONS_SHARED_PTR_CACHE_HPP
#define INCLUDED_SUPPORT_COLLECTIONS_SHARED_PTR_CACHE_HPP

#include "shared_ptr_cache.hh"

namespace Support::Collections
{
	template <class T, template <class...> class Ptr, class Alloc>
	shared_ptr_cache<T, Ptr, Alloc>::shared_ptr_cache() : shared_ptr_cache(default_capacity)
	{
		
	}
	
	template <class T, template <class...> class Ptr, class Alloc>
	shared_ptr_cache<T, Ptr, Alloc>::shared_ptr_cache(const allocator_type& a) : shared_ptr_cache(default_capacity, a)
	{
		
	}
	
	template <class T, template <class...> class Ptr, class Alloc>
	shared_ptr_cache<T, Ptr, Alloc>::shared_ptr_cache(size_type cap) : shared_ptr_cache(cap, allocator_type())
	{
		
	}
	
	template <class T, template <class...> class Ptr, class Alloc>
	shared_ptr_cache<T, Ptr, Alloc>::shared_ptr_cache(size_type cap, const allocator_type& a) : store(cap, a), _index(0)
	{
		assert(cap > 0);
		if (cap == 0)
		{
			// TODO: Error out
		}
		for (size_type i = 0; i < cap; ++i)
		{
			store[i] = nullptr;
		}
	}
	
	
	template <class T, template <class...> class Ptr, class Alloc>
	void shared_ptr_cache<T, Ptr, Alloc>::resize(size_type sz)
	{
		assert(sz > 0);
		if (sz == 0)
		{
			// TODO: Error out
		}
		size_type old_sz = store.size();
		
		store.resize(sz);
		if (sz < old_sz)
		{
			_index %= sz;
		}
	}
	
	template <class T, template <class...> class Ptr, class Alloc>
	auto shared_ptr_cache<T, Ptr, Alloc>::size() const noexcept -> size_type
	{
		return store.size();
	}
	
	
	template <class T, template <class...> class Ptr, class Alloc>
	void shared_ptr_cache<T, Ptr, Alloc>::cache(const value_type ptr)
	{
		assert(store.size() > 0);
		assert(_index < store.size());
		store[_index++] = ptr;
		_index %= store.size();
	}
	
	template <class T, template <class...> class Ptr, class Alloc>
	bool shared_ptr_cache<T, Ptr, Alloc>::decache(const value_type ptr)
	{
		for (size_type i = 0; i < store.size(); ++i)
		{
			if (store[i] == ptr)
			{
				store[i] = nullptr;
				return true;
			}
		}
		
		return false;
	}
	
	template <class T, template <class...> class Ptr, class Alloc>
	void shared_ptr_cache<T, Ptr, Alloc>::reset()
	{
		for (size_type i = 0; i < store.size(); ++i)
		{
			store[i] = nullptr;
		}
		_index = 0;
	}
	
	
}

#endif