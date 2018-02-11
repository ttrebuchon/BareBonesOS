#include "Locale.hh"

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
}