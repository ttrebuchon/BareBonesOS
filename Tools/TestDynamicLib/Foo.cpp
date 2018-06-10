
#include "Foo.h"
#include <stdint.h>


class Bar_Class : public Foo_Class
{
    public:
    virtual int foo_member() override
    {
		some_extern_foo(42);
        return 42;
    }
};


extern "C"
__attribute__ ((visibility ("default")))
uint32_t foo()
{
    return 0xDEADBABA;
}

extern "C"
__attribute__ ((visibility ("default")))
uint32_t foo2()
{
    return foo();
}

extern "C"
int bar(int x)
{
    return x % 2;
}


extern "C"
Foo_Class* get_poly()
{
    return new Bar_Class;
}