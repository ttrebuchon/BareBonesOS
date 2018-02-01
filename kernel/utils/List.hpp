#ifndef INCLUDED_LIST_HPP
#define INCLUDED_LIST_HPP


namespace Utils
{
	template <class T, class A>
	List<T, A>::List() : head(nullptr), tail(nullptr), _size(0), alloc(), nalloc()
	{
		head = tail = nalloc.allocate(1);
		new (head) Node();
	}
	
	
	template <class T, class A>
	List<T, A>::~List() noexcept
	{
		Node* ptr = head;
		while (head != nullptr)
		{
			head = head->next;
			nalloc.destroy(ptr);
			ptr = head;
		}
	}
	
	template <class T, class A>
	void List<T, A>::push_back(const T& t)
	{
		tail->init(alloc, t);
		auto ptr = nalloc.allocate(1);
		new (ptr) Node();
		tail->next = ptr;
		ptr->prev = tail;
		tail = ptr;
		++_size;
	}
	
	template <class T, class A>
	void List<T, A>::push_back(T&& t)
	{
		tail->init(alloc, t);
		auto ptr = nalloc.allocate(1);
		new (ptr) Node();
		tail->next = ptr;
		ptr->prev = tail;
		tail = ptr;
		++_size;
	}
	
	
	
	
}
#endif