class Foo_Class
{
    public:
    int x;
	const char* str;

	Foo_Class() : x(0), str(nullptr)
	{

	}

    virtual int foo_member() = 0;
};


extern "C"
Foo_Class* get_poly();


extern "C" void some_extern_foo(int some_arg);
