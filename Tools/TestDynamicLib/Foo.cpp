
#include "Foo.h"
#include <stdint.h>
#include <Utils/vector>
#include <drivers/VGA.hh>


class Bar_Class : public Foo_Class
{
    public:
    virtual int foo_member() override
    {
		
		this->str = __func__;
		Utils::vector<int> v;
		for (int i = 0; i < 100; ++i)
		{
			v.push_back(i + (v.size() % 10));
		}
		some_extern_foo(x);
		int old_x = x;
		x = v.size();

		Drivers::VGA::Write("foo_member() says hello!\n");

        return old_x;
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
	static_assert(sizeof(Bar_Class) == 12);
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