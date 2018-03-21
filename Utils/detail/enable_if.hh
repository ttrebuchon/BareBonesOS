#ifndef INCLUDED_ENABLE_IF_HH
#define INCLUDED_ENABLE_IF_HH

namespace Utils
{
	// enable_if
	template <bool b, class T>
	struct enable_if
	{};
	
	template <class T>
	struct enable_if<true, T>
	{
		typedef T type;
	};
	
	template <bool b, class T>
	using enable_if_t = typename enable_if<b, T>::type;
}

#endif