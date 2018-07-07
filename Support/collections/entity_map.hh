#ifndef INCLUDED_SUPPORT_COLLECTIONS_ENTITY_MAP_HH
#define INCLUDED_SUPPORT_COLLECTIONS_ENTITY_MAP_HH

#include <Common.h>
#include <Utils/map>
#include <Utils/typeinfo>

namespace Support { namespace Collections
{
	
	namespace detail
	{
		class entity_node
		{
			protected:
			
			public:
			
			virtual ~entity_node() = default;
			
			virtual size_t size() const noexcept = 0;
			
			#ifdef __cpp_rtti
			virtual const std::type_info& info() const noexcept = 0;
			#endif
		};
		
		template <class T>
		class typed_entity_node
		{
			protected:
			
			public:
			T item;
			
			virtual ~typed_entity_node() = default;
			
			virtual size_t size() const noexcept override
			{
				return sizeof(T);
			}
			
			#ifdef __cpp_rtti
			virtual const std::type_info& info() const noexcept override
			{
				return typeid(T);
			}
			#endif
		};
	}
	
	template <class Key_t>
	class entity_map
	{
		
		public:
		
		protected:
		//using detail::entity_node;
		
		/*template <class T>
		using typed_entity_node = detail::typed_entity_node<T>;*/
		
		template <class T>
		detail::typed_entity_node<T>* make_node();
		
		public:
		
		
	};
	
}
}

#endif