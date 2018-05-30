#ifndef INCLUDED_EXPERIMENTAL_REFLECT_BASETYPE_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_BASETYPE_HH

#include <Common.h>

namespace Utils { namespace experimental {
namespace reflect
{
	
	class Model;
	
	struct BaseType
	{
		bool linked;
		ptrdiff_t off;
		union
		{
			const Model* model;
			const type_info* info;
		};
		
		BaseType(const Model* m, const ptrdiff_t o) : linked(true), off(o), model(m)
		{
			
		}
		
		BaseType(const type_info* info, const ptrdiff_t o) : linked(false), off(o), info(info)
		{
			
		}
	};
	
}
}
}

#endif