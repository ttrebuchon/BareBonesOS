#ifndef INCLUDED_SUPPORT_VECTOR_STREAMBUF_HH
#define INCLUDED_SUPPORT_VECTOR_STREAMBUF_HH

#include <Utils/vector>
#include <Utils/iostream>
#include <Utils/string>

namespace Support
{
	template <class T, class Traits = Utils::Char_Traits<T>, class Alloc = Utils::allocator<T>>
	class basic_vector_streambuf : public Utils::basic_streambuf<T, Traits>
	{
		public:
		using typename Utils::basic_streambuf<T, Traits>::int_type;
		using typename Utils::basic_streambuf<T, Traits>::traits_type;
		
		
		private:
		Utils::vector<T, Alloc> vec;
		bool resize(size_t sz)
		{
			if (vec.capacity() >= sz && sz >= vec.size())
			{
				vec.resize(sz);
				return true;
			}
			
			
			auto gp_off = this->gptr() - this->eback();
			auto ge_off = this->egptr() - this->eback();
			
			auto p_off = this->pptr() - this->pbase();
			auto pe_off = this->epptr() - this->pbase();
			
			vec.resize(sz);
			
			if (gp_off > sz)
			{
				gp_off = sz;
			}
			
			if (p_off > sz)
			{
				p_off = sz;
			}
			
			this->setg(vec.data(), vec.data() + gp_off, vec.data() + vec.size());
			
			this->setp(vec.data(), vec.data() + vec.size());
			this->pbump(p_off);
			return true;
		}
		
		void expand_put()
		{
			if (vec.capacity() > vec.size())
			{
				vec.push_back(0);
				return;
			}
			
			auto gp_off = this->gptr() - this->eback();
			auto ge_off = this->egptr() - this->eback();
			
			auto p_off = this->pptr() - this->pbase();
			auto pe_off = this->epptr() - this->pbase();
			
			vec.push_back(0);
			vec.resize(vec.capacity());
			auto sz = vec.size();
			
			if (gp_off > sz)
			{
				gp_off = sz;
			}
			
			if (p_off > sz)
			{
				p_off = sz;
			}
			
			this->setg(vec.data(), vec.data() + gp_off, vec.data() + vec.size());
			
			this->setp(vec.data(), vec.data() + vec.size());
			this->pbump(p_off);
		}
		
		
		
		protected:
		virtual int_type overflow(int_type c = traits_type::eof()) override
		{
			expand_put();
			if (c != traits_type::eof())
			{
				this->sputc(c);
			}
			
			return 1;
		}
		
		public:
		basic_vector_streambuf() : basic_vector_streambuf(1024)
		{
			
		}
		
		basic_vector_streambuf(size_t size) : Utils::basic_streambuf<T, Traits>(), vec(size)
		{
			//buf = new char[size];
			//setp(buf, buf + size);
			//setg(buf, buf, buf + size);
			this->setp(vec.data(), vec.data() + vec.size());
			this->setg(vec.data(), vec.data(), vec.data() + vec.size());
		}
		
		
		Utils::basic_string<T, Traits, Alloc> str() const
		{
			return Utils::basic_string<T, Traits, Alloc>(this->pbase(), this->pptr() - this->pbase());
		}
		
		Utils::basic_string<T, Traits, Alloc> gstr() const
		{
			return Utils::basic_string<T, Traits, Alloc>(this->gptr(), this->egptr() - this->gptr());
		}
		
		
		void redirect_out_to_in()
		{
			this->setg(this->pbase(), this->pbase(), this->pptr());
		}
	};
	
	typedef basic_vector_streambuf<char> vector_streambuf;
	
}

#endif