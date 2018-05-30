#ifdef __cpp_rtti

#include "Type.hh"

namespace Utils { namespace experimental {
namespace reflect
{
	map<type_index, const Type*> Type::cache;
}
}
}

#endif