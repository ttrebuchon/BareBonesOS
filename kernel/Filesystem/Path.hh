#ifndef INCLUDED_PATH_HH
#define INCLUDED_PATH_HH

#include <Utils/string>

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
		constexpr static const char* Root = "/";
		
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

#endif