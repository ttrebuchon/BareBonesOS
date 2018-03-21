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
	List<T, A>::List(List&& r) : head(r.head), tail(r.tail), _size(r._size), alloc(r.alloc), nalloc(r.nalloc)
	{
		r.head = r.tail = nullptr;
		r._size = 0;
	}
	
	
	template <class T, class A>
	List<T, A>::~List() noexcept
	{
		Node* ptr = head;
		while (head != tail)
		{
			head = head->next;
			ptr->uninit(alloc);
			nalloc.destroy(ptr);
			nalloc.deallocate(ptr, 1);
			ptr = head;
		}
		if (tail != nullptr)
		{
			nalloc.destroy(tail);
			nalloc.deallocate(tail, 1);
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
	
	template <class T, class A>
	template <class... Args>
	void List<T, A>::emplace_back(Args&&... args)
	{
		tail->init(alloc, forward<Args>(args)...);
		auto ptr = nalloc.allocate(1);
		new (ptr) Node();
		tail->next = ptr;
		ptr->prev = tail;
		tail = ptr;
		++_size;
	}
	
	template <class T, class A>
	typename List<T, A>::size_type List<T, A>::size() const
	{
		return _size;
	}

	template <class T, class A>
	typename List<T, A>::iterator List<T, A>::begin()
	{
		iterator it;
		it.n = head;
		return it;
	}

	template <class T, class A>
	typename List<T, A>::iterator List<T, A>::end()
	{
		iterator it;
		it.n = nullptr;
		return it;
	}
	
	
}
#endif