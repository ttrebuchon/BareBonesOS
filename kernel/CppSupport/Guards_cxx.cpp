namespace __cxxabiv1
{
	#ifndef __USING_CXXRT__
    __extension__ typedef int __guard __attribute__((mode(__DI__)));

    extern "C" int __cxa_guard_acquire(__guard*);
    extern "C" void __cxa_guard_release(__guard*);
    extern "C" void __cxa_guard_abort(__guard*);


    extern "C" int __cxa_guard_acquire(__guard* g)
    {
        return !*(char*)(g);
    }

    extern "C" void __cxa_guard_release(__guard* g)
    {
        *(char*)g = 1;
    }

    extern "C" void __cxa_guard_abort(__guard* g)
    {

    }
    #endif
}