#ifndef INCLUDED_MEMORY_HEAPS_BUDDYHEAP_HH
#define INCLUDED_MEMORY_HEAPS_BUDDYHEAP_HH

#include <kernel/Memory/Heap.hh>
#include <Utils/Allocator.hh>
#include <Utils/mutex>

namespace Kernel::Memory
{
	namespace buddy_heap_detail
	{
		
		struct Block
		{
			void* address;
			
			// Block Size = pow(2, order)
			uint8_t order : 7;
			
			// hidden = 1: All children full
			uint8_t hidden : 1;
			
			
			Block(void* addr, uint8_t order) : address(addr), order(order), hidden(0)
			{
				assert(this->order == order);
			}
			
		};
		
		
		
		struct tree_node
		{
			private:
			void* _mid;
			
			public:
			tree_node *parent, *left, *right;
			uint8_t order : 7;
			uint8_t full : 1;
			
			tree_node(void* start, uint8_t order) noexcept : _mid(nullptr), order(order), full(0), parent(nullptr), left(nullptr), right(nullptr)
			{
				assert(this->order == order);
				assert(order < 128);
				assert(order > 0);
				
				_mid = (void*)((addr_t)start + ((addr_t)1 << (order-1)));
			}
			
			
			__attribute__((__always_inline__))
			void* mid() const noexcept
			{
				return _mid;
			}
			
			
			
			void* start() const noexcept
			{
				return (void*)((addr_t)_mid - ((size_t)1 << (order-1)));
			}
			
			void* end() const noexcept
			{
				return (void*)((uintptr_t)start() + size());
			}
			
			__attribute__((__always_inline__))
			bool is_leaf() const noexcept
			{
				return (left == nullptr) && (right == nullptr);
			}
			
			__attribute__((__always_inline__))
			tree_node* next(void* address) noexcept
			{
				if (address < mid())
				{
					return left;
				}
				else
				{
					return right;
				}
			}
			
			__attribute__((__always_inline__))
			size_t size() const noexcept
			{
				return ((size_t)1 << order);
			}
			
			bool update_full() noexcept
			{
				assert(!is_leaf());
				if (is_leaf())
				{
					return false;
				}
				
				bool nFull = false;
				if (left && left->full)
				{
					if (right && right->full)
					{
						nFull = true;
					}
				}
				if (nFull != full)
				{
					full = nFull;
					return true;
				}
				else
				{
					return false;
				}
			}
		} __attribute__((__packed__));
		
		
		template <class Alloc = Utils::allocator<tree_node>>
		class buddy_tree
		{
			public:
			typedef tree_node node_type;
			typedef Alloc allocator_type;
			
			protected:
			typedef typename allocator_type::template rebind<node_type>::other n_allocator_type;
			
			n_allocator_type alloc;
			node_type* _root;
			
			
			void clear_tree();
			void split_node(node_type*);
			
			public:
			node_type* const& root = _root;
			
			buddy_tree(void* start, uint8_t order, const n_allocator_type&);
			
			~buddy_tree();
			
			uint8_t order_for(size_t sz) const noexcept;
			
			node_type* find_leaf_node(void* address) noexcept;
			const node_type* find_leaf_node(void* address) const noexcept;
			
			void release_node(node_type*);
			node_type* allocate_node(uint8_t order);
		};
		
	}
	
	
	template <class Alloc = Utils::allocator<void>, class Mutex = Utils::mutex>
	class BuddyHeap : public Heap
	{
		public:
		typedef Alloc allocator_type;
		typedef Mutex mutex_type;
		
		protected:
		typedef buddy_heap_detail::buddy_tree<allocator_type> tree_type;
		typedef typename tree_type::node_type tree_node_type;
		
		tree_type tree;
		mutable mutex_type tree_m;
		uint8_t highest_order;
		uint8_t lowest_order;
		
		constexpr static float default_efficiency = 0.75;
		
		public:
		
		BuddyHeap(void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz = 0, float efficiency = default_efficiency);
		BuddyHeap(const allocator_type&, void* mem, size_t len, bool kernel_mem, bool default_read_only, size_t pg_sz = 0, float efficiency = default_efficiency);
		
		virtual ~BuddyHeap();
		
		
		virtual void* alloc(size_t size, size_t alignment = 0) override;
		virtual void free(void*) override;
		virtual size_t allocated_size(void*) const noexcept override;
		void* allocation_info(void* address, size_t* size = nullptr) const noexcept;
		
		bool full() const noexcept
		{
			return tree.root->full;
		}
		
		size_t smallest_alloc() const noexcept
		{
			return ((size_t)1) << lowest_order;
		}
		
		constexpr size_t max_node_count() const noexcept
		{
			return (1 << (highest_order - lowest_order + 1)) - 1;
		}
		
		constexpr size_t max_meta_size() const noexcept
		{
			return sizeof(tree_node_type)*max_node_count();
		}
		
		
		constexpr static size_t pre_calc_max_node_count(void* mem, size_t len, float efficiency = default_efficiency) noexcept;
		constexpr static size_t pre_calc_max_meta_size(void* mem, size_t len, float efficiency = default_efficiency) noexcept;
		
		
		constexpr static void calculate_bounds(void* mem, size_t len, uint8_t* low, uint8_t* high, float efficiency = default_efficiency) noexcept;
	};
	
}

#include "BuddyHeap.hpp"

#endif