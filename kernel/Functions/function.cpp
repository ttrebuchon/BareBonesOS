#include "function.hh"

namespace Kernel
{
	template <class F>
	struct fn_wrapper
	{
		F fn;
		void* data;
	};
	
	
	
	#define LAMBDA_ARGS int argc, const char** argv, cwd_type cwd, Utils::istream& in, Utils::ostream& out, struct process_info* proc, const sysfn_t* f
	
	
	/* Without process */
	
	// Without istream and ostream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
	
	
	// With istream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
	
	
	
	
	// With ostream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::ostream&, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, out, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::ostream&))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, out);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::ostream&, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, out, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::ostream&))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, out);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
	
	
	
	// With istream AND ostream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in, out, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in, out);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in, out, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in, out);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/* With process */
	
	// Without istream and ostream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
	
	
	// With istream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
	
	
	
	
	// With ostream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::ostream&, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, out, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::ostream&, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, out, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::ostream&, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, out, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::ostream&, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, out, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
	
	
	
	// With istream AND ostream
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in, out, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, cwd_type, Utils::istream&, Utils::ostream&, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, cwd, in, out, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&, struct process_info*, void*), void* data)
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in, out, proc, f->user_data);
		};
		sfn->user_data = data;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	int init_system_fn(sysfn_t* sfn, int(*fn)(int, const char**, Utils::istream&, Utils::ostream&, struct process_info*))
	{
		assert(sfn);
		typedef decltype(fn) F;
		sfn->fn = [](LAMBDA_ARGS)
		{
			return ((F)f->sub_fn)(argc, argv, in, out, proc);
		};
		sfn->user_data = nullptr;
		sfn->sub_fn = (void*)fn;
		return 0;
	}
	
	
	
}