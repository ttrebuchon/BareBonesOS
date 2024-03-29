#include <new>
#define private public
#include <Utils/weak_ptr>
#include <Utils/shared_ptr>
#include <Utils/unique_ptr>
#undef private
#include <Utils/Allocator.hh>
#include "Tests.hh"
#include <vector>







class Foo_t_Container
{
	// Make sure it works with forward decs
	public:
	Utils::shared_ptr<Foo_t> ptr;
};

class Base_t
{
	
	public:
	virtual void foo() = 0;
};

class Deriv_t : public Base_t
{
	public:
	int x = 0;
	
	virtual void foo() override
	{
		++x;
	}
	
};

template <template <class...> class ptr_t>
void baseCaller(ptr_t<Base_t> p)
{
	p->foo();
}




template <int N>
void foo(int*)
{
	std::cout << "Foo" << N << "() called!" << std::endl;
	Foo_t::callers.push_back(N);
}

template <template <class...> class _Base, class T>
class NoisyAllocator : _Base<T>
{
	private:
	static int y;
	int x;
	
	public:
	typedef _Base<T> Base;
	
	using Base::value_type;
	using Base::pointer;
	using Base::const_pointer;
	
	using Base::construct;
	using Base::destroy;
	
	const int& n;
	
	NoisyAllocator(const int z) : Base(), x(z), n(x)
	{
		
	}
	
	NoisyAllocator() : NoisyAllocator(y++)
	{
		
	}
	
	NoisyAllocator(const NoisyAllocator& a) : Base(a), x(a.x), n(x)
	{
		
	}
	
	//NoisyAllocator(NoisyAllocator&&) = delete;
	template <class Y>
	NoisyAllocator(const NoisyAllocator<_Base, Y>& other) : Base(other), x(other.x), n(x)
	{
		
	}
	/*template <class Y>
	NoisyAllocator(NoisyAllocator<_Base, Y>&&);*/
	
	T* allocate(size_t s)
	{
		T* ptr = nullptr;
		if (n == -1)
		{
			ptr = Base::allocate(s);
		}
		else
		{
			std::clog << std::endl << std::endl << "NoisyAllocator allocating while marked as invalid!!!" << std::endl << std::endl;
		}
		std::cout << "Allocated " << s << " at " << ptr << std::endl;
		return ptr;
	}
	
	void deallocate(T* ptr, size_t s)
	{
		std::cout << "Deallocated " << s << " at " << ptr << std::endl;
		Base::deallocate(ptr, s);
	}
	
	template <class Y>
	struct rebind
	{
		typedef NoisyAllocator<_Base, Y> other;
	};
	
	
	
	template <template <class...> class, class>
	friend class NoisyAllocator;
};

template <template <class...> class _Base, class T>
int NoisyAllocator<_Base, T>::y = 0;

template <class T>
NoisyAllocator<Utils::allocator, T> NA_foo()
{
	return NoisyAllocator<Utils::allocator, T>();
}


static void shared_from_this_test();

TEST(shared_ptr)
{
	{
		Utils::shared_ptr<int> ptr;
	{
		Utils::shared_ptr<int> ptr1((int*)nullptr, foo<0>);
		Utils::shared_ptr<int> ptr2((int*)nullptr, foo<1>);
		Utils::shared_ptr<int> ptr3((int*)nullptr, foo<2>);
		Utils::shared_ptr<int> ptr4(ptr2);
		Utils::shared_ptr<int> ptr5(nullptr);
		ptr = Utils::shared_ptr<int>(ptr2);
		assert(ptr == nullptr);
		assert(!ptr);
		assert(ptr.use_count() == 0);
	}
	}
	assert(Foo_t::callers.size() == 0);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	
	{
		int* test_int = new int(4);
	{
		Utils::shared_ptr<int> ptr;
	{
		Utils::shared_ptr<int> ptr1(test_int, foo<0>);
		Utils::shared_ptr<int> ptr2(test_int, foo<1>);
		Utils::shared_ptr<int> ptr3(test_int, foo<2>);
		Utils::shared_ptr<int> ptr4(ptr2);
		Utils::shared_ptr<int> ptr5(nullptr);
		ptr = Utils::shared_ptr<int>(ptr2);
		assert(ptr == test_int);
		assert(ptr);
		
		// ptr, ptr2, ptr4 all refer to the
		// same control
		ASSERTEQ(ptr.use_count(), 3);
	}
	}
	delete test_int;
	}
	ASSERTEQ(Foo_t::callers.size(), 3);
	assert(Foo_t::callers[0] == 2);
	assert(Foo_t::callers[1] == 0);
	assert(Foo_t::callers[2] == 1);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	
	
	{
		typedef NoisyAllocator<Utils::allocator, int> NoisyAlloc;
		typedef typename NoisyAlloc::template rebind<char>::other NoisyAlloc_Re;
		//static_assert(Utils::detail::alloc_traits::copy_construct_ret<NoisyAlloc>::Instance == 0);	static_assert(Utils::detail::alloc_traits::copy_construct<NoisyAlloc, NoisyAlloc_Re>::Instance == 1);
		static_assert(std::is_same<Utils::detail::alloc_traits::copy_construct_ret<NoisyAlloc>::type, decltype(Utils::detail::alloc_traits::copy_construct<NoisyAlloc, NoisyAlloc_Re>::call(std::declval<NoisyAlloc_Re>()))>::value);
		
		auto na0 = NA_foo<int>();
		int na0_n = na0.n;
		auto na0_0 = Utils::allocator_traits<NoisyAllocator<Utils::allocator, int>>::copy_create(na0);
		ASSERT(na0_0.n == na0.n);
	
		
		
		Utils::shared_ptr<int> ptr;
	{
		int* test_int = new int(4);
	{
		NoisyAlloc na(-1);
		Utils::shared_ptr<int> ptr1(test_int, foo<0>, na);
		Utils::shared_ptr<int> ptr2(test_int, foo<1>);
		Utils::shared_ptr<int> ptr3(test_int, foo<2>);
		Utils::shared_ptr<int> ptr4(ptr2);
		Utils::shared_ptr<int> ptr5(nullptr);
		ptr = Utils::shared_ptr<int>(ptr2);
		assert(ptr == test_int);
		assert(ptr);
		
		// ptr, ptr2, ptr4 all refer to the
		// same control
		ASSERTEQ(ptr.use_count(), 3);
	}
		delete test_int;
	}
	}
	assert(Foo_t::callers.size() == 3);
	assert(Foo_t::callers[0] == 2);
	assert(Foo_t::callers[1] == 0);
	assert(Foo_t::callers[2] == 1);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	
	
	
	{
		Utils::shared_ptr<Foo_t> ptr;
	}
	assert(Foo_t::callers.size() == 0);
	
	{
		Utils::shared_ptr<Foo_t> ptr(new Foo_t(0));
		Utils::shared_ptr<Foo_t> ptr1(new Foo_t(1));
		Utils::shared_ptr<Foo_t> ptr2(new Foo_t(2));
		assert(Foo_t::callers.size() == 0);
		assert(Foo_t::count == 3);
		assert(ptr->n == 0);
		assert(ptr1->n == 1);
		assert(ptr2->n == 2);
	}
	assert(Foo_t::count == 0);
	assert(Foo_t::callers.size() == 3);
	assert(Foo_t::callers[0] == 2);
	assert(Foo_t::callers[1] == 1);
	assert(Foo_t::callers[2] == 0);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	{
		auto ptr = Utils::make_shared<Foo_t>(4);
		assert(Foo_t::count == 1);
		assert(Foo_t::callers.size() == 0);
		assert(ptr);
		assert(ptr->n == 4);
	}
	assert(Foo_t::callers.size() == 1);
	assert(Foo_t::callers[0] == 4);
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	{
		Utils::weak_ptr<Foo_t> wptr2;
		{
		Utils::shared_ptr<Foo_t> ptr(new Foo_t(0));
		Utils::shared_ptr<Foo_t> ptr1(new Foo_t(1));
		Utils::shared_ptr<Foo_t> ptr2(new Foo_t(2));
		assert(Foo_t::callers.size() == 0);
		assert(Foo_t::count == 3);
		assert(ptr->n == 0);
		assert(ptr1->n == 1);
		assert(ptr2->n == 2);
		auto ctrl = ptr2.ctrl;
		assert(ctrl->usecount == 1);
		
		
		Utils::weak_ptr<Foo_t> wptr(ptr);
		assert(!wptr.expired());
		
		assert(ctrl->usecount == 1);
		wptr2 = Utils::weak_ptr<Foo_t>(ptr2);
		assert(ptr2.ctrl);
		assert(wptr2.ctrl == ptr2.ctrl);
		ctrl = ptr2.ctrl;
		assert(ctrl->usecount == 1);
		assert(ptr.use_count() == 1);
		assert(ctrl->refcount == 2);
		assert(ctrl->usecount == 1);
		}
		auto ctrl = wptr2.ctrl;
		assert(ctrl);
		assert(ctrl->refcount == 1);
		assert(ctrl->usecount == 0);
		assert(wptr2.expired());
		assert(!wptr2.lock());
	}
	assert(Foo_t::callers.size() == 3);
	assert(Foo_t::callers[0] == 2);
	assert(Foo_t::callers[1] == 1);
	assert(Foo_t::callers[2] == 0);
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	
	{
		Utils::unique_ptr<Foo_t> ptr((Foo_t*)nullptr);
		assert(!ptr);
		assert(ptr == nullptr);
		
		Utils::unique_ptr<Foo_t> ptr2(new Foo_t(0));
		assert(ptr2);
		assert(ptr2 != nullptr);
	}
	
	assert(Foo_t::callers.size() == 1);
	assert(Foo_t::callers[0] == 0);
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	{
		Foo_t* rptr;
		{
		Utils::unique_ptr<Foo_t> ptr(new Foo_t(10));
		assert(ptr);
		assert(ptr != nullptr);
		
		Utils::unique_ptr<Foo_t> ptr2(new Foo_t(0));
		assert(ptr2);
		assert(ptr2 != nullptr);
		
		assert(Foo_t::callers.size() == 0);
		assert(Foo_t::count == 2);
		
		rptr = ptr.release();
		assert(!ptr);
		assert(ptr == nullptr);
		assert(ptr2);
		assert(ptr2 != nullptr);
		assert(Foo_t::callers.size() == 0);
		assert(Foo_t::count == 2);
		assert(rptr != nullptr);
		}
		assert(Foo_t::callers.size() == 1);
		assert(Foo_t::callers[0] == 0);
		assert(Foo_t::count == 1);
		delete rptr;
	}
	assert(Foo_t::callers.size() == 2);
	assert(Foo_t::callers[0] == 0);
	assert(Foo_t::callers[1] == 10);
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	{
		auto ptr = Utils::make_unique<Foo_t>(6);
		assert(ptr->n == 6);
	}
	assert(Foo_t::callers.size() == 1);
	assert(Foo_t::callers[0] == 6);
	assert(Foo_t::count == 0);
	Foo_t::callers.clear();
	assert(Foo_t::callers.size() == 0);
	
	
	{
		Utils::shared_ptr<Deriv_t> ptr(new Deriv_t());
		ASSERTEQ(ptr->x, 0);
		baseCaller<Utils::shared_ptr>(ptr);
		ASSERTEQ(ptr->x, 1);
	}
	
	shared_from_this_test();
}



class TClass_1 : public Base_t, public Utils::enable_shared_from_this<TClass_1>
{
	public:
	
	virtual void foo() override
	{
		
	}
};

class TClass_2 : public TClass_1, public Utils::enable_shared_from_this<TClass_2>
{
	public:
	
	virtual void foo() override
	{
		
	}
};

class TClass_3 : public TClass_2
{
	public:
	
	virtual void foo() override
	{
		
	}
};

class TClass_4 : public Base_t, public Utils::enable_shared_from_this<TClass_4>
{
	public:
	
	virtual void foo() override
	{
		
	}
};


static void shared_from_this_test()
{
	{
		
		auto obj1 = Utils::make_shared<TClass_1>();
		assert(obj1 == obj1->shared_from_this());
		assert(obj1);
		
		auto obj2 = new TClass_4;
		auto sobj2 = Utils::shared_ptr<Base_t>(obj2);
		assert(sobj2);
		assert(sobj2 == obj2);
		assert(obj2->shared_from_this() == sobj2);
	}
}