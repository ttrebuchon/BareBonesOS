#include "function_manager.hh"
#include <Utils/iostream>
#include <Utils/vector>

#define DEFAULT_IN_SIZE 0
#define DEFAULT_OUT_SIZE 1024

namespace Kernel
{
	class array_streambuf : public Utils::streambuf
	{
		private:
		Utils::vector<char> vec;
		bool resize(size_t sz)
		{
			if (vec.capacity() >= sz && sz >= vec.size())
			{
				vec.resize(sz);
				return true;
			}
			
			
			auto gp_off = gptr() - eback();
			auto ge_off = egptr() - eback();
			
			auto p_off = pptr() - pbase();
			auto pe_off = epptr() - pbase();
			
			vec.resize(sz);
			
			if (gp_off > sz)
			{
				gp_off = sz;
			}
			
			if (p_off > sz)
			{
				p_off = sz;
			}
			
			setg(vec.data(), vec.data() + gp_off, vec.data() + vec.size());
			
			setp(vec.data(), vec.data() + vec.size());
			pbump(p_off);
			return true;
		}
		
		void expand_put()
		{
			if (vec.capacity() > vec.size())
			{
				vec.push_back(0);
				return;
			}
			
			auto gp_off = gptr() - eback();
			auto ge_off = egptr() - eback();
			
			auto p_off = pptr() - pbase();
			auto pe_off = epptr() - pbase();
			
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
			
			setg(vec.data(), vec.data() + gp_off, vec.data() + vec.size());
			
			setp(vec.data(), vec.data() + vec.size());
			pbump(p_off);
		}
		
		
		
		protected:
		virtual int_type overflow(int_type c = traits_type::eof()) override
		{
			expand_put();
			if (c != traits_type::eof())
			{
				sputc(c);
			}
			
			return 1;
		}
		
		public:
		array_streambuf() : array_streambuf(1024)
		{
			
		}
		
		array_streambuf(size_t size) : Utils::streambuf(), vec(size)
		{
			//buf = new char[size];
			//setp(buf, buf + size);
			//setg(buf, buf, buf + size);
			setp(vec.data(), vec.data() + vec.size());
			setg(vec.data(), vec.data(), vec.data() + vec.size());
		}
		
		~array_streambuf()
		{
			
		}
		
		
		Utils::string str() const
		{
			return Utils::string(pbase(), pptr() - pbase());
		}
		
		
	};
	
	
	bool sys_functions::add_fn(const char* name, const function_type& fn)
	{
		if (funcs.count(name))
		{
			return false;
		}
		
		funcs[name] = fn;
		return true;
		
		// TODO
		assert(NOT_IMPLEMENTED);
	}
	
	bool sys_functions::add_fn(const Utils::string& name, const function_type& fn)
	{
		if (funcs.count(name))
		{
			return false;
		}
		
		funcs[name] = fn;
		return true;
		
		// TODO
		assert(NOT_IMPLEMENTED);
	}
	
	
	
	
	
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream& in, Utils::ostream& out) const
	{
		assert(this->count(fn) > 0);
		const auto& sys_fn = funcs.at(fn);
		return sys_fn.fn(argc, argv, cwd, in, out, nullptr, &sys_fn);
	}
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream& in) const
	{
		int ret;
		{
			array_streambuf sbuf(DEFAULT_OUT_SIZE);
			{
				Utils::ostream out(&sbuf);
				ret = exec(fn, argc, argv, cwd, in, out, nullptr);
				auto str = sbuf.str();
				TRACE_VAL(str.c_str());
			}
		}
		return ret;
	}
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::ostream& out) const
	{
		int ret;
		{
			array_streambuf sbuf(DEFAULT_IN_SIZE);
			{
				Utils::istream in(&sbuf);
				ret = exec(fn, argc, argv, cwd, in, out, nullptr);
			}
		}
		return ret;
	}
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd) const
	{
		int ret;
		{
			array_streambuf sbuf(DEFAULT_IN_SIZE);
			{
				Utils::istream in(&sbuf);
				ret = exec(fn, argc, argv, cwd, in, nullptr);
			}
		}
		return ret;
	}
	
	
	
	
	
	Utils::string sys_functions::exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream& in, int* result_code) const
	{
		array_streambuf sbuf(DEFAULT_OUT_SIZE);
		{
			Utils::ostream out(&sbuf);
			int ret = exec(fn, argc, argv, cwd, in, out, nullptr);
			if (result_code)
			{
				*result_code = ret;
			}
		}
		return sbuf.str();
	}
	
	Utils::string sys_functions::exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, int* result_code) const
	{
		Utils::string ret;
		{
			array_streambuf sbuf(DEFAULT_IN_SIZE);
			{
				Utils::istream in(&sbuf);
				ret = exec_str(fn, argc, argv, cwd, in, nullptr, result_code);
			}
		}
		return ret;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream& in, Utils::ostream& out, struct process_info* proc) const
	{
		assert(this->count(fn) > 0);
		const auto& sys_fn = funcs.at(fn);
		return sys_fn.fn(argc, argv, cwd, in, out, proc, &sys_fn);
	}
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream& in, struct process_info* proc) const
	{
		int ret;
		{
			array_streambuf sbuf(DEFAULT_OUT_SIZE);
			{
				Utils::ostream out(&sbuf);
				ret = exec(fn, argc, argv, cwd, in, out, proc);
				auto str = sbuf.str();
				TRACE_VAL(str.c_str());
			}
		}
		return ret;
	}
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::ostream& out, struct process_info* proc) const
	{
		int ret;
		{
			array_streambuf sbuf(DEFAULT_IN_SIZE);
			{
				Utils::istream in(&sbuf);
				ret = exec(fn, argc, argv, cwd, in, out, proc);
			}
		}
		return ret;
	}
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, struct process_info* proc) const
	{
		int ret;
		{
			array_streambuf sbuf(DEFAULT_IN_SIZE);
			{
				Utils::istream in(&sbuf);
				ret = exec(fn, argc, argv, cwd, in, proc);
			}
		}
		return ret;
	}
	
	
	
	
	
	Utils::string sys_functions::exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::istream& in, struct process_info* proc, int* result_code) const
	{
		array_streambuf sbuf(DEFAULT_OUT_SIZE);
		{
			Utils::ostream out(&sbuf);
			int ret = exec(fn, argc, argv, cwd, in, out, proc);
			if (result_code)
			{
				*result_code = ret;
			}
		}
		
		return sbuf.str();
	}
	
	Utils::string sys_functions::exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, struct process_info* proc, int* result_code) const
	{
		Utils::string ret;
		{
			array_streambuf sbuf(DEFAULT_IN_SIZE);
			{
				Utils::istream in(&sbuf);
				ret = exec_str(fn, argc, argv, cwd, in, proc, result_code);
			}
		}
		return ret;
	}
	
	
	
	
	
	
	// streambuf args instead:
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, Utils::streambuf* out) const
	{
		int ret;
		if (in)
		{
			Utils::istream is(in);
			if (out)
			{
				Utils::ostream os(out);
				ret = exec(fn, argc, argv, cwd, is, os);
			}
			else
			{
				ret = exec(fn, argc, argv, cwd, is);
			}
		}
		else if (out)
		{
			Utils::ostream os(out);
			ret = exec(fn, argc, argv, cwd, os);
		}
		else
		{
			ret = exec(fn, argc, argv, cwd);
		}
		return ret;
	}
	
	
	
	Utils::string sys_functions::exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, int* result_code) const
	{
		Utils::string ret;
		if (in)
		{
			Utils::istream is(in);
			ret = exec_str(fn, argc, argv, cwd, is, result_code);
		}
		else
		{
			ret = exec_str(fn, argc, argv, cwd, result_code);
		}
		return ret;
	}
	
	int sys_functions::exec(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, Utils::streambuf* out, struct process_info* proc) const
	{
		int ret;
		if (in)
		{
			Utils::istream is(in);
			if (out)
			{
				Utils::ostream os(out);
				ret = exec(fn, argc, argv, cwd, is, os, proc);
			}
			else
			{
				ret = exec(fn, argc, argv, cwd, is, proc);
			}
		}
		else if (out)
		{
			Utils::ostream os(out);
			ret = exec(fn, argc, argv, cwd, os, proc);
		}
		else
		{
			ret = exec(fn, argc, argv, cwd, proc);
		}
		return ret;
	}
	
	Utils::string sys_functions::exec_str(const Utils::string& fn, int argc, const char** argv, cwd_type cwd, Utils::streambuf* in, struct process_info* proc, int* result_code) const
	{
		Utils::string ret;
		if (in)
		{
			Utils::istream is(in);
			ret = exec_str(fn, argc, argv, cwd, is, proc, result_code);
		}
		else
		{
			ret = exec_str(fn, argc, argv, cwd, proc, result_code);
		}
		return ret;
	}
	
	
}