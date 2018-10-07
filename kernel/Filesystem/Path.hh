#ifndef INCLUDED_PATH_HH
#define INCLUDED_PATH_HH

#include <Utils/string>
#include <Utils/vector>
#include "Path_Fwd.hh"

namespace Kernel { namespace FS
{
	
	
	
	template <class Char_t, class Traits, class Alloc>
	class basic_path
	{
		public:
		typedef Alloc allocator_type;
		typedef Char_t char_type;
		typedef Traits traits_type;
		typedef Utils::basic_string<char_type, traits_type, allocator_type> string_type;
		typedef char_type* cstring_type;
		typedef const char_type* const_cstring_type;
		typedef typename traits_type::off_type off_type;
		
		typedef typename string_type::size_type size_type;
		
		protected:
		
		
		
		template <class T>
		using alloc_rebind = typename allocator_type::template rebind<T>::other;
		
		
		allocator_type _alloc;
		Utils::vector<string_type, alloc_rebind<string_type>> _parts;
		mutable string_type _path;
		mutable bool _changed;
		bool _is_absolute;
		
		void _M_init_split(const string_type& path);
		string_type _M_combine() const;
		
		
		public:
		constexpr static size_type npos = string_type::npos;
		
		basic_path(const string_type&);
		explicit basic_path(const char_type*);
		basic_path(const string_type& base, const string_type& item);
		basic_path(const char_type* base, const char_type* item);
		basic_path();
		
		basic_path(const string_type&, const allocator_type&);
		basic_path(const char_type*, const allocator_type&);
		basic_path(const string_type& base, const string_type& item, const allocator_type&);
		basic_path(const char_type* base, const char_type* item, const allocator_type&);
		basic_path(const allocator_type&);
		
		basic_path(const basic_path&);
		basic_path(basic_path&&);
		
		
		size_type length() const noexcept;
		bool empty() const noexcept;
		off_type compare(const Path_t&) const;
		
		basic_path subpath(const size_type start, const size_type len = npos) const;
		
		const string_type& str() const noexcept;
		const char_type* c_str() const noexcept;
		constexpr bool is_absolute() const noexcept
		{ return _is_absolute; }
		
		basic_path& append(const string_type&);
		//basic_path& append(const cstring_type&);
		
		size_type parts_length() const noexcept;
		size_type parts_size() const noexcept
		{ return parts_length(); }
		const string_type& part(const size_type) const;
		const string_type& back() const
		{ return _parts.back(); }
		
		constexpr const allocator_type& get_allocator() const noexcept
		{ return _alloc; }
		
		basic_path& operator=(const basic_path&);
		basic_path& operator=(basic_path&&);
	};
	
	
	
	class Path
	{
		public:
		typedef typename Path_t::char_type char_type;
		typedef Utils::char_traits<char_type> traits_type;
		typedef Utils::basic_string<char_type> string_type;
		typedef const char_type* cstring_type;
		
		typedef typename traits_type::off_type off_type;
		
		private:
		
		
		
		public:
		constexpr static const cstring_type Root = "/";
		constexpr static const cstring_type Separator = "/";
		
		static bool IsAbsolute(const string_type&) noexcept;
		
		//static off_type IndexForRelativeToRoot(const string_type&, off_type off = 0) noexcept;
		
		static string_type MakeRelative(const string_type& path, const string_type& base);
		
		static string_type Combine(const string_type&, const string_type&, const string_type&...) noexcept;
		static string_type Combine(const string_type&, const string_type&) noexcept;
		static string_type Combine(const string_type*, const size_t count) noexcept;
		
		static string_type* Split(const string_type& path, size_t& count) noexcept;
		
		static bool CanSimplify(const string_type&);
		static bool Simplify(string_type&);
	};
	
	
}
}

#include "Path.hpp"

#endif