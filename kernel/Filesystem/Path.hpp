#ifndef INCLUDED_PATH_HPP
#define INCLUDED_PATH_HPP

#include "Path.hh"

namespace Kernel { namespace FS
{
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const string_type& path) : Path_t(path, allocator_type())
	{
		
	}
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const cstring_type& cpath) : Path_t(cpath, allocator_type())
	{
		
	}
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const string_type& base, const string_type& item) : Path_t(base, item, allocator_type())
	{
		
	}
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const cstring_type& base, const cstring_type& item) : Path_t(base, item, allocator_type())
	{
		
	}
	
	template <class Alloc>
	Path_t<Alloc>::Path_t() : Path_t(allocator_type())
	{
		
	}
	
	
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const string_type& path, const allocator_type& alloc) : _alloc(alloc), _parts(alloc), _path(alloc), _changed(true)
	{
		size_t count;
		auto segs = Path::Split(path, count);
		if (!segs)
		{
			return;
		}
		
		for (size_t i = 0; i < count; ++i)
		{
			_parts.push_back(string_type(segs[i], alloc));
		}
		
		delete[] segs;
	}
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const cstring_type& cpath, const allocator_type& alloc) : Path_t(string_type(cpath, alloc), alloc)
	{
		
	}
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const string_type& base, const string_type& item, const allocator_type& alloc) : Path_t(Path::Combine(base, item), alloc)
	{
		
	}
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const cstring_type& base, const cstring_type& item, const allocator_type& alloc) : Path_t(Path::Combine(string_type(base, alloc), string_type(item, alloc)), alloc)
	{
		
	}
	
	
	template <class Alloc>
	Path_t<Alloc>::Path_t(const allocator_type& alloc) : Path_t(string_type(alloc), alloc)
	{
		
	}
	
	
	
	template <class Alloc>
	auto Path_t<Alloc>::length() const noexcept -> size_type
	{
		return str().length();
	}
	
	template <class Alloc>
	auto Path_t<Alloc>::str() const noexcept -> const string_type&
	{
		if (_changed)
		{
			_path = Path::Combine(_parts.data(), _parts.size());
			_changed = false;
		}
		return _path;
	}
	
	template <class Alloc>
	auto Path_t<Alloc>::c_str() const noexcept -> const cstring_type
	{
		return str().c_str();
	}
	
	
	
	
	
	
	template <class Alloc>
	auto Path_t<Alloc>::parts_length() const noexcept -> size_type
	{
		return _parts.size();
	}
	
	template <class Alloc>
	auto Path_t<Alloc>::part(const size_type i) const -> const string_type&
	{
		return _parts.at(i);
	}
	
}
}


#endif