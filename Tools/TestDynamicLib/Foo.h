class Foo_Class
{
    public:
    int x;

    virtual int foo_member() = 0;
};


extern "C"
Foo_Class* get_poly();


extern "C" void some_extern_foo(int some_arg);
