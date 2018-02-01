#ifndef INCLUDED_PTRDIFF_HH
#define INCLUDED_PTRDIFF_HH

namespace Utils
{
	
	typedef decltype((int*)nullptr - (int*)nullptr) ptrdiff_t;
	
	
	
}
#endif