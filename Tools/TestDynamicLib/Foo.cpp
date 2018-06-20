
#include "Foo.h"
#include <stdint.h>
//#include <Utils/iostream>


class Bar_Class : public Foo_Class
{
    public:
    virtual int foo_member() override
    {
		//std::cout << __func__ << ": " << x << std::endl;
		some_extern_foo(x);
        return x;
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
	static_assert(sizeof(Bar_Class) == 8);
    return new Bar_Class;
}

extern "C" unsigned char* test_new()
{
	auto arr = new unsigned char[4];
	arr[0] = 0xDE;
	arr[1] = 0xAD;
	arr[2] = 0xBA;
	arr[3] = 0xBA;
	return arr;
}

extern "C"
{
	struct SomeStruct
	{
		int x, y, z;
	};
}


extern "C"
SomeStruct* test_some_struct(int x, int y, int z)
{
	auto ptr = new SomeStruct;
	ptr->x = x;
	ptr->y = y;
	ptr->z = z;
	return ptr;
}