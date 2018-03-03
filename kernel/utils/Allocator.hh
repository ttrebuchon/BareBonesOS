#ifndef INCLUDED_ALLOCATOR_HH
#define INCLUDED_ALLOCATOR_HH

#include "BoolTypes.hh"
#include "ptrdiff.hh"

namespace Utils
{
    template <class T>
    class Allocator;
    
    template <>
    class Allocator<void>
    {
    	public:
    	typedef void               value_type;
        typedef void*              pointer;
        typedef const void*        const_pointer;
    	
    	template <class U>
        struct rebind
        {
            typedef Allocator<U> other;
        };
    };
    
    
    template <class T>
    class Allocator
    {
    	public:
        typedef T               value_type;
        typedef T*              pointer;
        typedef const T*        const_pointer;
        typedef T&              reference;
        typedef const T&        const_reference;
        typedef decltype(sizeof(0))          size_type;
        typedef ptrdiff_t       difference_type;
        typedef true_type       propagate_on_container_move_assignment;
        typedef true_type       is_always_equal;
        

        template <class U>
        struct rebind
        {
            typedef Allocator<U> other;
        };
        
        
        Allocator() noexcept {}
        Allocator(const Allocator&) noexcept {}
        template <class U>
        Allocator(const Allocator<U>&) noexcept {}
        
        ~Allocator() = default;
        
        
        
        
        pointer address(reference x) const noexcept
        {
        	return &x;
        }
        
        const_pointer address(const_reference x) const noexcept
        {
        	return &x;
        }
        
        pointer allocate(size_type n, Allocator<void>::const_pointer hint = 0)
        {
        	return (pointer)operator new[](sizeof(T)*n);
        }
        
        void deallocate(pointer p, size_type n)
        {
        	operator delete[](p);
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
    
    template <class T>
    bool operator==(const Allocator<T>& a1, const Allocator<T>& a2)
    {
    	return true;
    }
    
    template <class T>
    bool operator!=(const Allocator<T>& a1, const Allocator<T>& a2)
    {
    	return false;
    }
    
    
    
    
}
#endif