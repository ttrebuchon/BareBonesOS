#ifndef INCLUDED_PATH_HPP
#define INCLUDED_PATH_HPP

#include "Path.hh"

namespace Kernel { namespace FS
{
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const string_type& path) : basic_path(path, allocator_type())
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const char_type* cpath) : basic_path(cpath, allocator_type())
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const string_type& base, const string_type& item) : basic_path(base, item, allocator_type())
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const char_type* base, const char_type* item) : basic_path(base, item, allocator_type())
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path() : basic_path(allocator_type())
	{
		
	}
	
	
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const string_type& path, const allocator_type& alloc) : _alloc(alloc), _parts(alloc), _path(alloc), _changed(true)
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
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const char_type* cpath, const allocator_type& alloc) : basic_path(string_type(cpath, alloc), alloc)
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const string_type& base, const string_type& item, const allocator_type& alloc) : basic_path(Path::Combine(base, item), alloc)
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const char_type* base, const char_type* item, const allocator_type& alloc) : basic_path(Path::Combine(string_type(base, alloc), string_type(item, alloc)), alloc)
	{
		
	}
	
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const allocator_type& alloc) : basic_path(string_type(alloc), alloc)
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const basic_path& other) : _alloc(other._alloc), _parts(other._parts), _path(other._path), _changed(other._changed)
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(basic_path&& other) : _alloc(Utils::move(other._alloc)), _parts(Utils::move(other._parts)), _path(Utils::move(other._path)), _changed(other._changed)
	{
		
	}
	
	
	
	template <class Char_t, class Traits, class Alloc>
	auto basic_path<Char_t, Traits, Alloc>::length() const noexcept -> size_type
	{
		return str().length();
	}
	
	template <class Char_t, class Traits, class Alloc>
	bool basic_path<Char_t, Traits, Alloc>::empty() const noexcept
	{
		if (!_changed)
		{
			return str().empty();
		}
		
		for (auto& s : _parts)
		{
			if (!s.empty())
			{
				return false;
			}
		}
		
		return true;
	}
	
	template <class Char_t, class Traits, class Alloc>
	auto basic_path<Char_t, Traits, Alloc>::str() const noexcept -> const string_type&
	{
		if (_changed)
		{
			_path = Path::Combine(_parts.data(), _parts.size());
			_changed = false;
		}
		return _path;
	}
	
	template <class Char_t, class Traits, class Alloc>
	auto basic_path<Char_t, Traits, Alloc>::c_str() const noexcept -> const char_type*
	{
		return str().c_str();
	}
	
	
	
	
	
	
	template <class Char_t, class Traits, class Alloc>
	auto basic_path<Char_t, Traits, Alloc>::parts_length() const noexcept -> size_type
	{
		return _parts.size();
	}
	
	template <class Char_t, class Traits, class Alloc>
	auto basic_path<Char_t, Traits, Alloc>::part(const size_type i) const -> const string_type&
	{
		return _parts.at(i);
	}
	
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>& basic_path<Char_t, Traits, Alloc>::operator=(basic_path&& other)
	{
		_alloc = Utils::move(other._alloc);
		_parts = Utils::move(other._parts);
		_path = Utils::move(other._path);
		_changed = other._changed;
		
		return *this;
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>& basic_path<Char_t, Traits, Alloc>::append(const string_type& seg)
	{
		if (seg.find(Path::Separator) != string_type::npos)
		{
			size_t count;
			auto segs = Path::Split(seg, count);
			if (!segs)
			{
				assert(count == 0);
			}
			for (size_t i = 0; i < count; ++i)
			{
				_parts.push_back(string_type(segs[i], _alloc));
			}
			
			if (segs)
			{
				delete[] segs;
				_changed = true;
			}
		}
		else if (seg.length() > 0)
		{
			_parts.push_back(string_type(seg, _alloc));
			_changed = true;
		}
		
		return *this;
	}
	
}
}


#endif