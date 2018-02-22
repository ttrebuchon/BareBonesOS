#include "Locale"
#include "Locale.hpp"
#ifdef TESTING
#include <assert.h>
#endif

namespace Utils {
	
	size_t locale::id::counter = 0;
	locale locale::_global(locale::classic());
	
	
	const locale& locale::classic()
	{
		static Internal* imp = nullptr;
		if (!imp)
		{
			imp = new Internal;
			imp->addFacet(new num_put<char>);
			imp->rcount = 100;
		}
		
		static locale _classic(imp);
		
		return _classic;
	}
	
	locale::locale(Internal* i) : imp(i)
	{
		
	}
	
	
	
	
	
	locale::locale() : locale(_global)
	{
		
	}
	
	locale::locale(const locale& loc) : imp(loc.imp)
	{
		++imp->rcount;
	}
	
	
	
	
	
	locale::~locale()
	{
		--imp->rcount;
		if (imp->rcount == 0)
		{
			delete imp;
		}
	}
	
	
	
	
	const locale& locale::operator=(const locale& loc)
	{
		if (this->imp)
		{
			if (--imp->rcount == 0)
			{
				delete imp;
			}
		}
		this->imp = loc.imp;
		if (imp)
		{
			++imp->rcount;
		}
		
		return *this;
	}
}