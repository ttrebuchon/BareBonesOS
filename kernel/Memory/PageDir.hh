#ifndef INCLUDED_PAGEDIR_HH
#define INCLUDED_PAGEDIR_HH

#include <Common.h>

namespace Kernel { namespace Memory {

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

        void alloc_frame(bool is_kernel, bool is_writeable);
        void free_frame();
    };

    static_assert(sizeof(Page) == 4);

    struct PageTable
    {
        Page pages[1024];

        PageTable* clone(uint32_t* phys) const;
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
    };
    
    static_assert(sizeof(PageTable*) == 8);
    static_assert(sizeof(PageDir) == 1024*sizeof(PageDir_Entry) + 1024*sizeof(PageTable*));
}
}
#endif