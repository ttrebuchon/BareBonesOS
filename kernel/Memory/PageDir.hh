#ifndef INCLUDED_PAGEDIR_HH
#define INCLUDED_PAGEDIR_HH

#include <Common.h>
#include <Utils/map>
#include "Allocators/cross_processor_allocator.hh"

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif


#define PAGE_SIZE 0x1000

extern "C" void flush_tlb_page(addr_t addr);



namespace Kernel { namespace Memory {
	
	class PageRegion;
	template <class T>
	class cross_proc_allocator;
	
	
	class Page
	{
		public:
		uint32_t present : 1;
		uint32_t rw : 1;
		uint32_t user : 1;
		uint32_t reserved : 2;
		uint32_t accessed : 1;
		uint32_t dirty : 1;
		uint32_t reserved2 : 2;
		uint32_t avail : 3;
		uint32_t frame : 20;

		Page();

		void alloc_frame(bool is_kernel, bool is_writeable);
		void free_frame();
		
		bool map(addr_t phys, bool writeable, bool kernel, bool overwrite = true);
	};

	static_assert(sizeof(Page) == 4);

	struct PageTable
	{
		Page pages[1024];

		PageTable* clone(addr_t* phys) const;
	};

	static_assert(sizeof(PageTable) == 4*1024);

	class PageDir_Entry
	{
		public:
		uint32_t present : 1;
		uint32_t rw : 1;
		uint32_t user : 1;
		uint32_t w_through : 1;
		uint32_t cache : 1;
		uint32_t access : 1;
		uint32_t reserved : 1;
		uint32_t page_size : 1;
		uint32_t global : 1;
		uint32_t avail : 3;
		uint32_t frame : 20;

	} __attribute__((packed));


	static_assert(sizeof(PageDir_Entry) == 4);

	class PageDir
	{
		public:
		PageDir_Entry tables[1024];
		PageTable* ref_tables[1024];

		Page* getPage(uint32_t addr, bool create);
		PageDir* clone() const;
		void flush() const;
		
		bool map(addr_t virt, size_t len, addr_t phys, bool writeable, bool kernel, bool overwrite = true);
	};
	
	#if !defined(TESTING) && !defined(FREE_TESTING)
	static_assert(sizeof(PageTable*) == 4);
	#endif
	static_assert(sizeof(PageDir) == 1024*sizeof(PageDir_Entry) + 1024*sizeof(PageTable*));
	
	
	
	class PageDirectory
	{
		protected:
		struct alignas(uint32_t) _Page
		{
			uint32_t present : 1;
			uint32_t rw : 1;
			uint32_t user : 1;
			uint32_t reserved : 2;
			uint32_t accessed : 1;
			uint32_t dirty : 1;
			uint32_t reserved2 : 2;
			uint32_t avail : 3;
			
			// void*
			uint32_t frame : 20;
		};
		
		static_assert(sizeof (_Page) == sizeof (uint32_t));
		static_assert(alignof (_Page) == alignof (uint32_t));
		
		
		struct alignas(1024*4) _Pages
		{
			_Page pages[1024];
			
		}__attribute__((packed));
		
		static_assert(sizeof(_Pages) == sizeof(_Page)*1024);
		static_assert(sizeof(_Pages) == 1024*4);
		
		
		struct alignas(uint32_t) _Table
		{
			// 1 = Is present
			// 0 = Is not present
			uint32_t present : 1;
			
			// 1 = Read/Write
			// 0 = Read
			uint32_t rw : 1;
			
			// 1 = Unrestricted
			// 0 = Supervisor only
			uint32_t user : 1;
			
			// 1 = Write-through caching
			// 0 = Write-back caching
			uint32_t w_through : 1;
			
			// 1 = Disabled
			// 0 = Enabled
			uint32_t cacheDisabled : 1;
			
			// Set by CPU when read/written
			// from/to
			uint32_t accessed : 1;
			
			// Reserved: Always 0
			uint32_t reserved : 1;
			
			// 1 = 4MiB
			// 0 = 4KiB
			uint32_t pageSize : 1;
			
			// Ignored
			uint32_t global : 1;
			
			// Available for any use
			uint32_t avail : 3;
			
			// _Page[]
			// aka _Page*
			uint32_t frame : 20;
			
		}__attribute__((packed));
		
		static_assert(sizeof (_Table) == sizeof (uint32_t));
		static_assert(alignof (_Table) == alignof (uint32_t));
		
		struct _Dir
		{
			_Table tables[1024];
		};
		
		static_assert(sizeof (_Dir) == 1024*sizeof (_Table));
		
		public:
		class Table;
		
		
		class Page
		{
			protected:
			_Page* page;
			//Table* _table;
			uint16_t page_index;
			
			Page();
			Page(_Page&, uint16_t pg_indx);
			
			public:
			
			Page(const Page&) = delete;
			Page(Page&&) = delete;
			
			_Page& operator*() noexcept;
			_Page* operator->() noexcept;
			const _Page& operator*() const noexcept;
			const _Page* operator->() const noexcept;
			Page& operator=(const Page&) = delete;
			Page& operator=(Page&&) = delete;
			Page& operator=(const void* const addr) noexcept;
			
			void* frame() const noexcept;
			void frame(const void* const) noexcept;
			
			void read_write(const bool nValue) noexcept;
			bool read_write() const noexcept;

			void supervisor(const bool nValue) noexcept;
			bool supervisor() const noexcept;
			
			bool present() const noexcept;
			void present(bool) noexcept;

			void flush() const noexcept;

			bool allocate(bool, bool) noexcept;
			bool deallocate() noexcept;
			
			Table* table() noexcept;
			const Table* table() const noexcept
			{ return (Table*)((addr_t)this - (offsetof(Table, pages) + sizeof(Page)*page_index)); }
			
			uint16_t index() const noexcept
			{
				return page_index;
			}
			
			void* virtual_address() const noexcept;
			
			
			friend class Table;
			friend class PageDirectory;
		};
		
		
		
		class Table
		{
			protected:
			_Table* table;
			Page pages[1024];
			_Pages* _pages;
			void* _pages_phys;
			size_t present_count;
			PageDirectory* _dir;
			uint16_t table_index;
			cross_proc_allocator<_Pages> alloc;
			
			
			
			
			Table(_Table&, PageDirectory&, uint16_t) noexcept;
			
			template <class Alloc>
			Table(_Table&, const Alloc&, PageDirectory&, uint16_t);
			
			public:
			
			Table(const Table&) = delete;
			Table(Table&&) = delete;
			
			Page& operator[](size_t index) noexcept;
			Page& at(size_t index) noexcept;
			const Page& at(size_t index) const noexcept;
			Table* clone(_Table&, PageDirectory& other) const noexcept;
			
			_Table* operator->() noexcept;
			const _Table* operator->() const noexcept;
			_Table& operator*() noexcept;
			const _Table& operator*() const noexcept;
			Table& operator=(const Table&) = delete;
			Table& operator=(Table&&) = delete;
			
			
			uint16_t index() const noexcept
			{
				return table_index;
			}
			
			__attribute__((__always_inline__))
			constexpr size_t page_count() const noexcept
			{
				return 1024;
			}
			
			friend class PageDirectory;
		};
		
		
		protected:
		_Dir* dir;
		Table* tables[1024];
		void* dir_phys;
		cross_proc_allocator<Table> tbl_alloc;
		
		
		public:
		
		PageDirectory(const PageDirectory&) = delete;
		PageDirectory(PageDirectory&&) = delete;
		
		template <class Alloc>
		PageDirectory(const Alloc&, size_t = 0);
		PageDirectory();
		
		
		Page* at(const void* const, bool create = false) noexcept;
		const Page* at(const void* const) const noexcept;

		Page* at(const addr_t, bool create = false) noexcept;
		const Page* at(const addr_t) const noexcept;
		
		Table* table(const size_t, bool create = false) noexcept;
		Table* table(const void* const, bool create = false) noexcept;
		const Table* table(const size_t) const noexcept;
		const Table* table(const void* const) const noexcept;
		void* getPhysicalAddress(const void* const) const noexcept;
		void* getPhysicalAddress(const volatile void* const) const noexcept;
		__attribute__((always_inline))
		void* physical(const void* const p) const noexcept
		{ return getPhysicalAddress(p); }
		__attribute__((always_inline))
		void* physical(const volatile void* const p) const noexcept
		{ return getPhysicalAddress(p); }
		void switch_to() noexcept;
		bool flush() const noexcept;
		const void* thisPhysical() const noexcept
		{
			return dir_phys;
		}

		bool map(const void* virt, const void* phys, size_t len, bool writeable, bool kernel_only, bool overwrite = false) noexcept;
		bool map(addr_t virt, addr_t phys, size_t len, bool writeable, bool kernel_only, bool overwrite = false) noexcept;
		bool map(const void* virt, size_t len, bool writeable, bool kernel_only) noexcept;
		bool map(addr_t virt, size_t len, bool writeable, bool kernel_only) noexcept;
		
		PageDirectory* clone(PageDirectory* linkWith = nullptr) const noexcept;
		PageDirectory* clone(PageDirectory** linkWith, size_t count) const noexcept;
		
		
		Page& operator[](const void*) noexcept;
		PageDirectory& operator=(const PageDirectory&) = delete;
		PageDirectory& operator=(PageDirectory&&) = delete;
		
		__attribute__((__always_inline__))
		constexpr size_t table_count() const noexcept
		{
			return 1024;
		}
		
		
		__attribute__((always_inline))
		constexpr static size_t GetTableIndex(const void* const p) noexcept
		{
			return (addr_t)p >> 22;
			return (((addr_t)p)/PAGE_SIZE)/1024;
		}
		
		__attribute__((always_inline))
		constexpr static size_t GetPageIndex(const void* const p) noexcept
		{
			return (addr_t)p >> 12 & 0x03FF;
			return (((addr_t)p)/PAGE_SIZE) % 1024;
		}
		
		
		static PageDirectory* Current;
		constexpr static size_t Regions_Max = 1;
		
		static PageRegion* Regions[Regions_Max];
		//static Utils::map<uint16_t, PageRegion*, Utils::less<uint16_t>, cross_proc_allocator<Utils::pair<const uint16_t, PageRegion*>>> Regions;
	};
}
}
#endif
#include "PageDir.hpp"