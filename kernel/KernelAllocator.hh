#ifndef INCLUDED_KERNELALLOCATOR_HH
#define INCLUDED_KERNELALLOCATOR_HH

#include <Common.h>
#include "utils/BoolTypes.hh"
#include "utils/ptrdiff.hh"

EXTERN_C void* kmalloc(uint32_t size, int align, addr_t* phys);

namespace Kernel
{

    template <class T>
    class KernelAllocator;
    
    template <>
    class KernelAllocator<void>
    {
    	public:
    	typedef void               value_type;
        typedef void*              pointer;
        typedef const void*        const_pointer;
    	
    	template <class U>
        struct rebind
        {
            typedef KernelAllocator<U> other;
        };
    };



    template <class T>
    class KernelAllocator
    {
        public:
        typedef T               value_type;
        typedef T*              pointer;
        typedef const T*        const_pointer;
        typedef T&              reference;
        typedef const T&        const_reference;
        typedef size_t          size_type;
        typedef Utils::ptrdiff_t       difference_type;
        typedef Utils::true_type       propagate_on_container_move_assignment;
        typedef Utils::true_type       is_always_equal;
        

        template <class U>
        struct rebind
        {
            typedef KernelAllocator<U> other;
        };
        
        
        KernelAllocator() noexcept {}
        KernelAllocator(const KernelAllocator&) noexcept {}
        template <class U>
        KernelAllocator(const KernelAllocator<U>&) noexcept {}
        
        ~KernelAllocator() = default;
        
        
        
        
        pointer address(reference x) const noexcept
        {
        	return &x;
        }
        
        const_pointer address(const_reference x) const noexcept
        {
        	return &x;
        }
        
        pointer allocate(size_type n, KernelAllocator<void>::const_pointer hint = 0)
        {
        	return (pointer)kmalloc(sizeof(T)*n, true, 0x0);
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
}


#endif