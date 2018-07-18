#ifndef INCLUDED_PATH_HH
#define INCLUDED_PATH_HH

#include <Utils/string>
#include <Utils/vector>

namespace Kernel { namespace FS
{
	class Path
	{
		public:
		typedef char char_type;
		typedef Utils::char_traits<char> traits_type;
		typedef Utils::string string_type;
		typedef const char* cstring_type;
		
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
	
	
	template <class Alloc = Utils::allocator<typename Path::char_type>>
	class Path_t
	{
		public:
		typedef Alloc allocator_type;
		typedef typename Path::char_type char_type;
		typedef typename Path::traits_type traits_type;
		typedef Utils::basic_string<char_type, traits_type, allocator_type> string_type;
		typedef typename Path::cstring_type cstring_type;
		typedef typename Path::off_type off_type;
		
		typedef typename string_type::size_type size_type;
		
		protected:
		
		
		
		template <class T>
		using alloc_rebind = typename allocator_type::template rebind<T>::other;
		
		
		allocator_type _alloc;
		Utils::vector<string_type, alloc_rebind<string_type>> _parts;
		mutable string_type _path;
		mutable bool _changed;
		
		
		public:
		
		Path_t(const string_type&);
		Path_t(const cstring_type&);
		Path_t(const string_type& base, const string_type& item);
		Path_t(const cstring_type& base, const cstring_type& item);
		Path_t();
		
		Path_t(const string_type&, const allocator_type&);
		Path_t(const cstring_type&, const allocator_type&);
		Path_t(const string_type& base, const string_type& item, const allocator_type&);
		Path_t(const cstring_type& base, const cstring_type& item, const allocator_type&);
		Path_t(const allocator_type&);
		
		
		size_type length() const noexcept;
		const string_type& str() const noexcept;
		const cstring_type c_str() const noexcept;
		
		const Path_t& append(const string_type&);
		const Path_t& append(const cstring_type&);
		
		size_type parts_length() const noexcept;
		const string_type& part(const size_type) const;
	};
	
}
}

#include "Path.hpp"

#endif