#pragma once
#include <map>
#include <typeinfo>
#include <list>
#include <string>

namespace QA
{
	class Memory final
	{
		public:
		
		static void Init();
		static void* Allocate(size_t s);
		static void* AllocateArray(size_t s);
		static void Release(void*);
		static void Release(void*, size_t);
		static void ReleaseArray(void*);
		static void ReleaseArray(void*, size_t);
		
		static void Reset();
		
		
		
		
		private:
		static void* metaPool;
		static void* metaPtr;
		static size_t metaLimit;
		
		
		template <class T>
		class MetaAllocator
		{
			public:
			typedef T               value_type;
			typedef T*              pointer;
			typedef const T*        const_pointer;
			typedef T&              reference;
			typedef const T&        const_reference;
			typedef decltype(sizeof(0)) size_type;
			typedef std::ptrdiff_t       difference_type;
			typedef std::true_type       propagate_on_container_move_assignment;
			typedef std::true_type       is_always_equal;
			
			
			template <class U>
			struct rebind
			{
				typedef MetaAllocator<U> other;
			};
			
			constexpr MetaAllocator() noexcept {}
			constexpr MetaAllocator(const MetaAllocator&) noexcept {}
			template <class U>
			constexpr MetaAllocator(const MetaAllocator<U>&) noexcept {}
			
			~MetaAllocator() = default;
			
			
			
			
			pointer address(reference x) const noexcept
			{
				return &x;
			}
			
			const_pointer address(const_reference x) const noexcept
			{
				return &x;
			}
			
			pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
			{
				auto p = metaPtr;
				metaPtr += sizeof(T)*n;
				return (pointer)p;
			}
			
			void deallocate(pointer p, size_type n)
			{
				
			}
			template <class U, class... Args>
			void construct(U* p, Args&&... args)
			{
				new ((void*)p) U(args...);
			}
			template <class U>
			void destroy(U* u)
			{
				u->~U();
			}
			
		};
		
		public:
		class Allocation
		{
			private:
			void* __addr;
			size_t __size : 7;
			bool __array : 1;
			
			//std::basic_string<char, std::char_traits<char>, MetaAllocator<char>> __tinfo;
			
			public:
			Allocation(void* ptr, size_t s, bool array = false) : __addr(ptr), __size(s), __array(array)//, __tinfo()
			{
				
			}
			
			
			void* addr() const
			{
				return __addr;
			}
			
			size_t size() const
			{
				return __size;
			}
			
			bool array() const
			{
				return __array;
			}
		};
		
		
		private:
		static std::map<void*, Allocation*, std::less<void*>, MetaAllocator<std::pair<void* const, Allocation*>>> allocationMap;
		static std::list<Allocation*, MetaAllocator<Allocation*>> allocations;
		
		static MetaAllocator<Allocation> alloc;
		static bool __initted;
		
		static size_t _total;
		
		public:
		
		static const std::map<void*, Allocation*, std::less<void*>, MetaAllocator<std::pair<void* const, Allocation*>>>& Map;
		static const std::list<Allocation*, MetaAllocator<Allocation*>>& Allocations;
		
		static const size_t& Total;
	};
	
	
}