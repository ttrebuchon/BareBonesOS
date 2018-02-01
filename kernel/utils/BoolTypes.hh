#ifndef INCLUDED_BOOLTYPES_HH
#define INCLUDED_BOOLTYPES_HH

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
	
	
	
}
#endif