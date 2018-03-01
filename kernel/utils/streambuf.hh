#ifndef INCLUDED_STREAMBUF_HH
#define INCLUDED_STREAMBUF_HH

#include "Locale.hh"
#include "detail/ios_base.hh"

namespace Utils {
	
	template <class T, class Traits = Char_Traits<T>>
	class basic_streambuf
	{
		public:
		typedef T char_type;
		typedef typename Traits::int_type int_type;
		typedef typename Traits::pos_type pos_type;
		typedef typename Traits::off_type off_type;
		typedef Traits traits_type;
		
		
		
		private:
		char_type *_pstart, *_pptr, *_pend, *_gstart, *_gptr, *_gend;
		locale _loc;
		
		
		protected:
		
		
		basic_streambuf();
		basic_streambuf(const basic_streambuf&);
		basic_streambuf& operator=(const basic_streambuf&);
		void swap(basic_streambuf&);
		
		
		char_type* eback() const;
		char_type* gptr() const;
		char_type* egptr() const;
		void gbump(int);
		void setg(char_type* gbeg, char_type* gnext, char_type* gend);
		
		char_type* pbase() const;
		char_type* pptr() const;
		char_type* epptr() const;
		void pbump(int);
		void setp(char_type* pbeg, char_type* pend);
		
		virtual void imbue(const locale&);
		
		virtual basic_streambuf* setbuf(char_type*, streamsize);
		virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in|ios_base::out);
		virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in|ios_base::out);
		virtual int sync();
		
		virtual streamsize showmanyc();
		virtual streamsize xsgetn(char_type* s, streamsize n);
		virtual int_type underflow();
		virtual int_type uflow();
		
		virtual int_type pbackfail(int_type c = traits_type::eof());
		
		virtual streamsize xsputn(const char_type* s, streamsize n);
		virtual int_type overflow(int_type c = traits_type::eof());
		
		
		public:
		
		virtual ~basic_streambuf() = default;
		
		locale pubimbue(const locale&);
		locale getloc() const;
		
		basic_streambuf* pubsetbuf(char_type* s, streamsize n);
		pos_type pubseekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in|ios_base::out);
		pos_type pubseekpos(pos_type sp, ios_base::openmode which = ios_base::in|ios_base::out);
		int pubsync();
		
		streamsize in_avail();
		int_type snextc();
		int_type sbumpc();
		int_type sgetc();
		streamsize sgetn(char_type* s, streamsize n);
		
		int_type sputbackc(char_type);
		int_type sungetc();
		
		
		int_type sputc(char_type);
		streamsize sputn(const char_type* s, streamsize n);
	};
	
	typedef basic_streambuf<char> streambuf;
	typedef basic_streambuf<wchar_t> wstreambuf;
}
#endif