#ifndef INCLUDED_PATH_HPP
#define INCLUDED_PATH_HPP

#include "Path.hh"

namespace Kernel { namespace FS
{
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const string_type& path) : basic_path(path, path.get_allocator())
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const char_type* cpath) : basic_path(cpath, allocator_type())
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(const string_type& base, const string_type& item) : basic_path(base, item, base.get_allocator())
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
	basic_path<Char_t, Traits, Alloc>::basic_path(const string_type& path, const allocator_type& alloc) : _alloc(alloc), _parts(alloc), _path(alloc), _changed(true), _is_absolute(false)
	{
		_M_init_split(path);
		/*size_t count;
		auto segs = Path::Split(path, count);
		if (!segs)
		{
			return;
		}
		
		for (size_t i = 0; i < count; ++i)
		{
			_parts.push_back(string_type(segs[i], alloc));
		}
		
		delete[] segs;*/
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
	basic_path<Char_t, Traits, Alloc>::basic_path(const basic_path& other) : _alloc(other._alloc), _parts(other._parts), _path(other._path), _changed(other._changed), _is_absolute(other._is_absolute)
	{
		
	}
	
	template <class Char_t, class Traits, class Alloc>
	basic_path<Char_t, Traits, Alloc>::basic_path(basic_path&& other) : _alloc(Utils::move(other._alloc)), _parts(Utils::move(other._parts)), _path(Utils::move(other._path)), _changed(other._changed), _is_absolute(other._is_absolute)
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
		if (is_absolute())
		{
			return false;
		}
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
	auto basic_path<Char_t, Traits, Alloc>::subpath(const size_type start, const size_type len) const -> basic_path
	{
		if (start > parts_size())
		{
			// TODO
			assert(NOT_IMPLEMENTED);
		}
		
		basic_path npath(get_allocator());
		npath._is_absolute = is_absolute();
		if (unlikely(len == 0))
		{
			return npath;
		}
		else if (len == npos)
		{
			for (size_type i = start; i < _parts.size(); ++i)
			{
				npath.append(_parts.at(i));
			}
		}
		else
		{
			if (start + len > parts_size())
			{
				// TODO
				assert(NOT_IMPLEMENTED);
			}
			
			for (size_type i = start; i < start + len; ++i)
			{
				npath.append(_parts.at(i));
			}
		}
		
		return npath;
	}
	
	template <class Char_t, class Traits, class Alloc>
	auto basic_path<Char_t, Traits, Alloc>::str() const noexcept -> const string_type&
	{
		if (_changed)
		{
			_path = _M_combine();
			//_path = Path::Combine(_parts.data(), _parts.size());
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
				append(string_type(segs[i], _alloc));
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
	
	
	
	
	
	
	template <class Char_t, class Traits, class Alloc>
	void basic_path<Char_t, Traits, Alloc>::_M_init_split(const string_type& path)
	{
		size_t count;
		_parts.clear();
		_is_absolute = false;
		
		if (path.length() == 0)
		{
			return;
		}
		
		count = 1;
		size_t i = 0;
		if (path[0] == '/')
		{
			_is_absolute = true;
			i = 1;
		}
		for (; i < path.length() - 1; ++i)
		{
			if (path[i] == '/')
			{
				++count;
			}
		}
		
		
		_parts.reserve(count);
		while (_parts.size() < count)
		{
			_parts.emplace_back(_alloc);
		}

		size_t n = 0;
		if (is_absolute())
		{
			i = 1;
		}
		else
		{
			i = 0;
		}
		for (; i < path.length() - 1; ++i)
		{
			assert(n < count);
			if (path[i] == '/')
			{
				++n;
			}
			else
			{
				_parts[n] += path[i];
			}
		}
		
		if (path[path.length()-1] != '/')
		{
			_parts[n] += path[path.length()-1];
		}
	}
	
	template <class Char_t, class Traits, class Alloc>
	auto basic_path<Char_t, Traits, Alloc>::_M_combine() const -> string_type
	{
		size_t count = _parts.size();
		if (count == 0)
		{
			return string_type(_alloc);
		}
		
		size_t max_poss = 1;
		for (size_t i = 0; i < count; ++i)
		{
			max_poss += _parts[i].length() + 1;
		}
		//max_poss += count - 1;

		string_type accum(_alloc);
		accum.reserve(max_poss);
		if (is_absolute())
		{
			accum += '/';
		}
		accum += _parts[0];
		bool hasSlash = false;
		if (accum.length() > 0)
		{
			hasSlash = accum[accum.length()-1] == '/';
		}
		
		for (size_t i = 1; i < count; ++i)
		{
			if (_parts[i].length() > 0)
			{
				if (_parts[i][0] == '/')
				{
					if (hasSlash)
					{
						accum += _parts[i].substr(1);
					}
					else
					{
						accum += _parts[i];
					}
				}
				else if (hasSlash)
				{
					accum += _parts[i];
				}
				else
				{
					accum += "/";
					accum += _parts[i];
				}

				hasSlash = accum[accum.length()-1] == '/';
			}
		}

		return accum;
	}
	
}
}


#endif