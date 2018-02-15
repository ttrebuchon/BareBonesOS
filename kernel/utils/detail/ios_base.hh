#ifndef INCLUDED_IOS_BASE_HH
#define INCLUDED_IOS_BASE_HH

#include <Common.h>
#include <kernel/utils/Types.hh>
#include <kernel/utils/Locale.hh>

namespace Utils {
	
	#define BITFLAGS_OR(X, TYPE) \
	constexpr X operator|(const X m) const \
	{ \
		TYPE c = (*reinterpret_cast<const TYPE *>(this) | *reinterpret_cast<const TYPE *>(&m)); \
		return *reinterpret_cast<X*>(&c); \
	}
	
	#define BITFLAGS_AND(X, TYPE) \
	constexpr X operator&(const X m) const \
	{ \
		TYPE c = (*reinterpret_cast<const TYPE *>(this) & *reinterpret_cast<const TYPE *>(&m)); \
		return *reinterpret_cast<X*>(&c); \
	}
	
	
	
	
	
	
	
	
	
	class ios_base
	{
		public:
		/*struct alignas(4) fmtflags
		{
			bool boolalpha : 1;
			bool dec : 1;
			bool fixed : 1;
			bool hex : 1;
			bool internal : 1;
			bool left : 1;
			bool oct : 1;
			bool right : 1;
			bool scientific : 1;
			bool showbase : 1;
			bool showpoint : 1;
			bool showpos : 1;
			bool skipws : 1;
			bool unitbuf : 1;
			bool uppercase : 1;
			
			BITFLAGS_OR(fmtflags, int)
			BITFLAGS_AND(fmtflags, int)
		};
		static_assert(sizeof(fmtflags) == sizeof(int));
		constexpr static fmtflags boolalpha = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		constexpr static fmtflags dec = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
		constexpr static fmtflags fixed = {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0};
		constexpr static fmtflags hex = {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0};
		constexpr static fmtflags internal = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0};
		constexpr static fmtflags left = {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0};
		constexpr static fmtflags oct = {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0};
		constexpr static fmtflags right = {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0};
		constexpr static fmtflags scientific = {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0};
		constexpr static fmtflags showbase = {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0};
		constexpr static fmtflags showpoint = {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0};
		constexpr static fmtflags showpos = {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0};
		constexpr static fmtflags skipws = {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0};
		constexpr static fmtflags unitbuf = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0};
		constexpr static fmtflags uppercase = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
		constexpr static fmtflags adjustfield = {0,0,0,0,1,1,1,0,0,0,0,0,0,0,0};
		constexpr static fmtflags basefield = {0,1,0,1,0,0,1,0,0,0,0,0,0,0,0};
		constexpr static fmtflags floatfield = {0,0,1,0,0,0,0,0,1,0,0,0,0,0,0};*/
		
		
		typedef unsigned int fmtflags;
		constexpr static fmtflags boolalpha = 0x1;
		constexpr static fmtflags dec = 0x2;
		constexpr static fmtflags fixed = 0x4;
		constexpr static fmtflags hex = 0x8;
		constexpr static fmtflags internal = 0x10;
		constexpr static fmtflags left = 0x20;
		constexpr static fmtflags oct = 0x40;
		constexpr static fmtflags right = 0x80;
		constexpr static fmtflags scientific = 0x100;
		constexpr static fmtflags showbase = 0x200;
		constexpr static fmtflags showpoint = 0x400;
		constexpr static fmtflags showpos = 0x800;
		constexpr static fmtflags skipws = 0x1000;
		constexpr static fmtflags unitbuf = 0x2000;
		constexpr static fmtflags uppercase = 0x4000;
		constexpr static fmtflags adjustfield = left | right | internal;
		constexpr static fmtflags basefield = dec | oct | hex;
		constexpr static fmtflags floatfield = fixed | scientific;
		
		
		
		
		
		/*struct iostate
		{
			bool badbit : 1;
			bool efbit : 1;
			bool failbit : 1;
			
			BITFLAGS_OR(iostate, char)
			BITFLAGS_AND(iostate, char)
		};
		static_assert(sizeof(iostate) == sizeof(char));
		constexpr static iostate badbit = {1,0,0};
		constexpr static iostate eofbit = {0,1,0};
		constexpr static iostate failbit = {0,0,1};
		constexpr static iostate goodbit = {0,0,0};*/
		
		typedef char iostate;
		constexpr static iostate badbit = 0x1;
		constexpr static iostate eofbit = 0x2;
		constexpr static iostate failbit = 0x4;
		constexpr static iostate goodbit = 0x0;
		
		
		struct openmode
		{
			bool app : 1;
			bool ate : 1;
			bool binary : 1;
			bool in : 1;
			bool out : 1;
			bool trunc : 1;
			
			BITFLAGS_OR(openmode, char)
			BITFLAGS_AND(openmode, char)
		};
		static_assert(sizeof(openmode) == sizeof(char));
		constexpr static openmode app = {1,0,0,0,0,0};
		constexpr static openmode ate = {0,1,0,0,0,0};
		constexpr static openmode binary = {0,0,1,0,0,0};
		constexpr static openmode in = {0,0,0,1,0,0};
		constexpr static openmode out = {0,0,0,0,1,0};
		constexpr static openmode trunc = {0,0,0,0,0,1};
		
		struct seekdir
		{
			bool beg : 1;
			bool cur : 1;
			bool end : 1;
			BITFLAGS_OR(seekdir, char)
			BITFLAGS_AND(seekdir, char)
		};
		static_assert(sizeof(seekdir) == sizeof(char));
		
		constexpr static seekdir beg = {1, 0, 0};
		constexpr static seekdir cur = {0, 1, 0};
		constexpr static seekdir end = {0, 0, 1};
		
		
		private:
		static int index;
		long* iarray;
		void** parray;
		
		fmtflags _flags;
		locale _loc;
		
		protected:
		ios_base();
		
		public:
		enum event_type
		{
			
			
		};
		
		typedef void (*event_callback)(event_type, ios_base&, int);
		
		ios_base(const ios_base&) = delete;
		virtual ~ios_base() = default;
		
		
		
		
		fmtflags flags() const;
		fmtflags flags(fmtflags);
		fmtflags setf(fmtflags);
		fmtflags setf(fmtflags, fmtflags mask);
		void unsetf(fmtflags);
		streamsize precision() const;
		streamsize precision(streamsize prec);
		streamsize width();
		streamsize width(streamsize w);
		
		locale imbue(const locale&);
		locale getloc() const;
	};
	
	#undef BITFLAGS_OR
	#undef BITFLAGS_AND
	
	
	
	
}

#endif