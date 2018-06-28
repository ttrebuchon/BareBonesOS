#ifndef INCLUDED_BITS_ADD_REFERENCE_HH
#define INCLUDED_BITS_ADD_REFERENCE_HH

namespace Utils
{
	template <class T>
	struct add_reference
	{
		typedef T& type;
	};
}

#endif