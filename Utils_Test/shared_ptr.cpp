#include <new>
#define private public
#include <Utils/weak_ptr>
#include <Utils/shared_ptr>
#include <Utils/unique_ptr>
#undef private
#include "Tests.hh"
#include <vector>







class Foo_t_Container
{
	// Make sure it works with forward decs
	public:
	Utils::shared_ptr<Foo_t> ptr;
};





template <int N>
void foo(int*)
{
	std::cout << "Foo" << N << "() called!" << std::endl;
	Foo_t::callers.push_back(N);
}

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
		Utils::shared_ptr<int> ptr6((unsigned int*)nullptr);
		ptr = Utils::shared_ptr<int>(ptr2);
		assert(ptr == nullptr);
		assert(!ptr);
		assert(ptr.use_count() == 0);
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
}