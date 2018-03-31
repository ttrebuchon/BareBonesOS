#ifndef INCLUDED_NUM_PUT_HH
#define INCLUDED_NUM_PUT_HH

#include "locale.hh"
#include "ios_base.hh"

namespace Utils
{
	template <class T, class OutputIt>
	class num_put : public locale::facet
	{
		public:
		typedef T char_type;
		typedef OutputIt iter_type;
		
		
		private:
		template <class N>
		static void numeric_str(N v, iter_type&, ios_base& io);
		template <class N>
		static void numeric_str(N v, iter_type&, ios_base& io, typename ios_base::fmtflags base);
		static void bool_str(const bool, iter_type&, ios_base& io);
		static void long_long_str(const long long, iter_type&, ios_base& io);
		static void unsigned_long_long_str(const unsigned long long, iter_type&, ios_base& io);
		static void long_double_str(const long double, iter_type&, ios_base& io);
		static void ptr_str(const void*, iter_type&, ios_base& io);
		//With Flags:
		static void bool_str(const bool, iter_type&, ios_base& io, ios_base::fmtflags);
		static void long_long_str(const long long, iter_type&, ios_base& io, ios_base::fmtflags);
		static void unsigned_long_long_str(const unsigned long long, iter_type&, ios_base& io, ios_base::fmtflags);
		static void long_double_str(const long double, iter_type&, ios_base& io, ios_base::fmtflags);
		static void ptr_str(const void*, iter_type&, ios_base& io, ios_base::fmtflags);
		
		template <class N>
		static N getNumBase(const ios_base& io, ios_base::fmtflags);
		
		inline static ios_base::fmtflags getFlagsBase(const ios_base& io, ios_base::fmtflags flags)
		{
			if ((flags & ios_base::basefield) == 0)
			{
				flags |= io.flags();
			}
			flags &= ios_base::basefield;
			
			if (flags != ios_base::hex && flags != ios_base::oct)
			{
				return ios_base::dec;
			}
			
			return flags;
		}
		
		
		
		
		constexpr static const T* _trueStr = "true";
		constexpr static auto _trueStrLen = 4;
		constexpr static const T* _falseStr = "false";
		constexpr static auto _falseStrLen = 5;
		static const T digits[16];// = { '0', '1', '2', '3', '3', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		constexpr static T pos = '+';
		constexpr static T neg = '-';
		constexpr static const T* _hexPrefix = "0x";
		constexpr static auto _hexPrefixLen = 2;
		constexpr static const T* _octPrefix = "0";
		constexpr static auto _octPrefixLen = 1;
		constexpr static T point = '.';
		
		
		
		protected:
		
		public:
		static locale::id id;
		
		#define PUT_T(X, Y) \
		public: \
		iter_type put(iter_type out, ios_base& str, T fill, X v) const \
		{ \
			return do_put(out, str, fill, v); \
		} \
		\
		protected: \
		virtual iter_type do_put(iter_type out, ios_base& str, T fill, X v) const \
		{ \
			Y##_str(v, out, str); \
			return out; \
		} \
		\
		public: \
		iter_type put(iter_type out, ios_base& str, ios_base::fmtflags fl, T fill, X v) const \
		{ \
			return do_put(out, str, fl, fill, v); \
		} \
		\
		protected: \
		virtual iter_type do_put(iter_type out, ios_base& str, ios_base::fmtflags fl, T fill, X v) const \
		{ \
			Y##_str(v, out, str, fl); \
			return out; \
		}
		
		PUT_T(bool, bool)
		PUT_T(long, long_long)
		PUT_T(long long, long_long)
		PUT_T(unsigned long, unsigned_long_long)
		PUT_T(unsigned long long, unsigned_long_long)
		PUT_T(double, long_double)
		PUT_T(long double, long_double)
		PUT_T(const void*, ptr)
		
	};
	
	
	
	template <class T, class OutputIt>
	locale::id num_put<T, OutputIt>::id;
	
	template <class T, class OutputIt>
	const T num_put<T, OutputIt>::digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	
}
#endif