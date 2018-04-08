#include "Path.hh"

namespace Kernel { namespace Filesystem
{
	
	bool Path::IsAbsolute(const string_type& path) noexcept
	{
		if (path.empty())
		{
			return false;
		}
		
		if (path[0] == '/')
		{
			return true;
		}
		
		
		// Any special cases go here
		
		return false;
	}
	
	auto Path::MakeRelative(const string_type& _path, const string_type& _base) -> string_type
	{
		if (_base.empty())
		{
			// TODO: Error-out
			ASSERT(false);
		}
		
		bool pAbs = IsAbsolute(_path);
		bool bAbs = IsAbsolute(_base);
		
		if (pAbs != bAbs)
		{
			// TODO: Error-out
			ASSERT(false);
		}
		
		string_type path = _path;
		Simplify(path);
		string_type base = _base;
		Simplify(base);
		
		
		
		size_t pCount;
		string_type* pSegs = Split(path, pCount);
		size_t bCount;
		string_type* bSegs = Split(base, pCount);
		string_type result;
		
		ASSERT(pCount >= bCount);
		size_t i = 0;
		while (i < bCount)
		{
			ASSERT(pSegs[i] == bSegs[i]);
			++i;
		}
		result = Combine(pSegs, pCount - i);
		
		delete[] pSegs;
		delete[] bSegs;
		
		return result;
	}
	
	
}
}