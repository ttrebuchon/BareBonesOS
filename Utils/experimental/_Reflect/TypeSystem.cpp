#include "TypeSystem.hh"

namespace Utils { namespace experimental {
namespace reflect
{
	
	bool TypeSystem::link()
	{
		if (!is_satisfied())
		{
			return false;
		}
		
		for (auto& model : models)
		{
			if (!model.second->link(*this))
			{
				return false;
			}
		}
		
		return true;
	}
}
}
}