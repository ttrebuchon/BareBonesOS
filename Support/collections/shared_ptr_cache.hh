#ifndef INCLUDED_SUPPORT_COLLECTIONS_SHARED_PTR_CACHE_HH
#define INCLUDED_SUPPORT_COLLECTIONS_SHARED_PTR_CACHE_HH

#include <Utils/allocator.hh>
#include <Utils/vector>


namespace Support::Collections
{
	
	template <class T, template <class...> class Ptr, class Alloc = Utils::allocator<Ptr<T>>>
	class shared_ptr_cache
	{
		public:
		
		typedef Ptr<T> value_type;
		typedef Alloc allocator_type;
		typedef Utils::vector<value_type, allocator_type> container_type;
		typedef typename container_type::size_type size_type;
		
		constexpr static size_type default_capacity = 5;
		
		protected:
		container_type store;
		size_type _index;
		
		public:
		shared_ptr_cache();
		shared_ptr_cache(const allocator_type&);
		shared_ptr_cache(size_type);
		shared_ptr_cache(size_type, const allocator_type&);
		
		
		void resize(size_type);
		size_type size() const noexcept;
		void cache(const value_type);
		bool decache(const value_type);
		void reset();
	};
	
}


#endif