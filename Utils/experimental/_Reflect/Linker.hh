#ifndef INCLUDED_EXPERIMENTAL_REFLECT_LINKER_HH
#define INCLUDED_EXPERIMENTAL_REFLECT_LINKER_HH

#include <Common.h>
#include "TypeModel.hh"
#include "TypeSystem.hh"
#include <Utils/unordered_map>
#include <Utils/list>

namespace Utils { namespace experimental {
namespace reflect
{
	class Linker
	{
		public:
		
		
		protected:
		
		
		public:
		TypeSystem& system;
		
		Linker(TypeSystem& ts) : system(ts)
		{}
		
	};
}
}
}

#endif