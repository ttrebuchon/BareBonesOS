#ifndef INCLUDED_ALLOCATOR_HH
#define INCLUDED_ALLOCATOR_HH

namespace Utils
{
    struct true_type
    {
        typedef bool value_type;
        typedef true_type type;

        constexpr operator value_type()
        {
            return true;
        }  
    };

    template <class T>
    struct Allocator
    {
        typedef T               value_type;
        typedef T*              pointer;
        typedef const T*        const_pointer;
        typedef T&              reference;
        typedef const T&        const_reference;
        typedef size_t          size_type;
        typedef ptrdiff_t       difference_type;
        typedef true_type       propagate_on_container_move_assignment;
        typedef true_type       is_always_equal;
        

        template <class U>
        struct rebind
        {
            typedef Allocator<U> other;
        };
    };
}
#endif