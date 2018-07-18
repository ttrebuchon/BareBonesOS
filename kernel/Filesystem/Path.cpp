#include "Path.hh"

namespace Kernel { namespace FS
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
		string_type* bSegs = Split(base, bCount);
		string_type result;
		
		ASSERT(pCount >= bCount);
		size_t i = 0;
		while (i < bCount)
		{
			ASSERT(pSegs[i] == bSegs[i]);
			++i;
			
		}
		result = Combine(pSegs + i, pCount - i);
		
		
		delete[] pSegs;
		delete[] bSegs;
		
		return result;
	}
	
	auto Path::Combine(const string_type& p1, const string_type& p2, const string_type& ps...) noexcept -> string_type
	{
		return Combine(Combine(p1, p2), ps);
	}

	auto Path::Combine(const string_type& p1, const string_type& p2) noexcept -> string_type
	{
		if (p1.empty())
		{
			return p2;
		}

		if (p2.empty())
		{
			return p1;
		}

		if (p1[p1.length() - 1] == '/')
		{
			if (p2[0] == '/')
			{
				return p1 + p2.substr(1);
			}
			else
			{
				return p1 + p2;
			}
		}
		else if (p2[0] == '/')
		{
			return p1 + p2;
		}
		else
		{
			return p1 + "/" + p2;
		}
	}

	auto Path::Combine(const string_type* ps, const size_t count) noexcept -> string_type
	{
		if (count == 0)
		{
			return string_type();
		}
		
		size_t max_poss = 0;
		for (size_t i = 0; i < count; ++i)
		{
			max_poss += ps[i].length() + 1;
		}
		//max_poss += count - 1;

		string_type accum;
		accum.reserve(max_poss);
		accum += ps[0];
		bool hasSlash = true;
		if (accum.length() > 0)
		{
			hasSlash = accum[accum.length()-1] == '/';
		}
		
		for (size_t i = 1; i < count; ++i)
		{
			if (ps[i].length() > 0)
			{
				if (ps[i][0] == '/')
				{
					if (hasSlash)
					{
						accum += ps[i].substr(1);
					}
					else
					{
						accum += ps[i];
					}
				}
				else if (hasSlash)
				{
					accum += ps[i];
				}
				else
				{
					accum += "/" + ps[i];
				}

				hasSlash = accum[accum.length()-1] == '/';
			}
		}

		return accum;
	}

	auto Path::Split(const string_type& path, size_t& count) noexcept -> string_type*
	{
		if (path.length() == 0)
		{
			count = 0;
			return nullptr;
		}
		
		count = 1;
		for (size_t i = 0; i < path.length() - 1; ++i)
		{
			if (path[i] == '/')
			{
				++count;
			}
		}
		

		string_type* array = new string_type[count];
		size_t n = 0;
		for (size_t i = 0; i < path.length() - 1; ++i)
		{
			assert(n < count);
			if (path[i] == '/')
			{
				++n;
			}
			else
			{
				array[n] += path[i];
			}
		}
		
		if (path[path.length()-1] != '/')
		{
			array[n] += path[path.length()-1];
		}
		
		return array;
	}

	bool Path::CanSimplify(const string_type&)
	{
		// TODO
		return false;
		ASSERT(false);
	}

	bool Path::Simplify(string_type&)
	{
		// TODO
		return false;
		ASSERT(false);
	}
}
}