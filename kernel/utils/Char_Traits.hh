#ifndef INCLUDED_CHAR_TRAITS_HH
#define INCLUDED_CHAR_TRAITS_HH


//#include <cwchar> <-- [mbstate_t]

namespace Utils
{
    template <class T>
    struct Char_Traits
    {
        typedef T char_type;
        //TODO
    };


    template <>
    struct Char_Traits<char>
    {
        typedef char char_type;
        typedef int int_type;
        //typedef streamoff off_type;
        //typedef pos_type streampos;
        //typedef mbstate_t state_type;

        static constexpr bool eq(char_type c, char_type d) noexcept
        {
            return c == d;
        }

        static constexpr bool lt(char_type c, char_type d) noexcept
        {
            return c < d;
        }

        static size_t length(const char_type* s)
        {
            size_t len = 0;
            while (s[len++] != '0') ;
            return len-1;
        }

        static void assign(char_type& r, const char_type& c) noexcept
        {
            r = c;
        }

        static char_type* assign(char_type* p, size_t n, char_type c)
        {
            for (int i = 0; i < n; ++i)
            {
                p[i] = c;
            }
            return p;
        }
    };
}
#endif