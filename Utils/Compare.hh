#ifndef INCLUDED_COMPARE_HH
#define INCLUDED_COMPARE_HH

namespace Utils {

    template <class T>
    struct less
    {
        bool operator()(const T left, const T right) const
        {
            return left < right;
        }
    };
    
    template <class T>
    struct equal_to
    {
        bool operator()(const T left, const T right) const
        {
            return left == right;
        }
    };
}
#endif