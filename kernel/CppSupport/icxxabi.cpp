#include "icxxabi.h"
#include <Common.h>

extern "C"
{
    atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
    uarch_t __atexit_func_count = 0;
    
    #ifdef __NOSTDLIB__
    void* __dso_handle;
    #endif

    int __cxa_atexit(void (*f)(void*), void* objptr, void* dso)
    {
        if (__atexit_func_count >= ATEXIT_MAX_FUNCS)
        {
            return -1;
        }

        __atexit_funcs[__atexit_func_count].destructor_func = f;
        __atexit_funcs[__atexit_func_count].obj_ptr = objptr;
        __atexit_funcs[__atexit_func_count].dso_handle = dso;
        ++__atexit_func_count;
        return 0;
    }


    void __cxa_finalize(void* f)
    {
        uarch_t i = __atexit_func_count;
        if (!f)
        {
            while (--i)
            {
                if (__atexit_funcs[i].destructor_func)
                {
                    (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
                }
            }
            return;
        }

        while (--i)
        {
            if (__atexit_funcs[i].destructor_func == f)
            {
                (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
                __atexit_funcs[i].destructor_func = 0;

                while (i < __atexit_func_count)
                {
                    __atexit_funcs[i] = __atexit_funcs[i+1];
                    ++i;
                }
                --__atexit_func_count;
                break;
            }
        }
    }


}

extern "C" {
	int dl_iterate_phdr(int (*)(struct dl_phdr_info*, size_t, void*), void*)
	{
		// TODO
		
		return -1;
	}
	
	#ifdef __EXCEPTIONS
	#ifdef __USING_CXXRT__
	void __cxa_throw_bad_array_new_length()
	{
		// TODO
		throw 0;
	}
	#endif
	#endif
}

namespace __cxxabiv1
{
	
}