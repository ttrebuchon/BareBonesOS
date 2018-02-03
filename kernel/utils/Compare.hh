#ifndef INCLUDED_COMPARE_HH
#define INCLUDED_COMPARE_HH

namespace Utils {

    template <class T>
    struct Less
    {
        bool operator()(const T left, const T right) const
        {
            return left < right;
        }
    };
}
#endif