#ifndef INCLUDED_LIST_HPP
#define INCLUDED_LIST_HPP


namespace Utils
{
	template <class T, class A>
	List<T, A>::List() : head(nullptr), tail(nullptr), _size(0), alloc(), nalloc()
	{
		head = tail = nullptr;
	}
	
	template <class T, class A>
	List<T, A>::List(List&& r) noexcept : head(r.head), tail(r.tail), _size(r._size), alloc(r.alloc), nalloc(r.nalloc)
	{
		r.head = r.tail = nullptr;
		r._size = 0;
	}
	
	template <class T, class A>
	List<T, A>::List(const List& r) : head(nullptr), tail(nullptr), _size(r._size), alloc(r.alloc), nalloc(r.nalloc)
	{
		if (r.head)
		{
			head = r.head->clone(nalloc, true, &tail);
			ASSERT(head != nullptr);
			ASSERT(tail != nullptr);
		}
	}
	
	template <class T, class A>
	List<T, A>::List(const allocator_type& alloc) : head(nullptr), tail(nullptr), _size(0), alloc(alloc), nalloc(alloc)
	{
		
	}
	
	
	template <class T, class A>
	List<T, A>::~List() noexcept
	{
		Node* ptr = head;
		while (head != nullptr)
		{
			head = head->next;
			nalloc.destroy(ptr);
			nalloc.deallocate(ptr, 1);
			ptr = head;
		}
	}
	
	template <class T, class A>
	void List<T, A>::push_back(const T& t)
	{
		auto ptr = nalloc.allocate(1);
		assert(ptr);
		assert((uintptr_t)ptr % alignof(Node) == 0);
		nalloc.construct(ptr, t);
		if (!head)
		{
			head = ptr;
			tail = ptr;
		}
		else
		{
			assert(tail);
			tail->next = ptr;
			ptr->prev = tail;
			tail = ptr;
		}
		++_size;
	}
	
	template <class T, class A>
	void List<T, A>::push_back(T&& t)
	{
		auto ptr = nalloc.allocate(1);
		assert(ptr);
		assert((uintptr_t)ptr % alignof(Node) == 0);
		nalloc.construct(ptr, Utils::forward<T&&>(t));
		if (!head)
		{
			head = ptr;
			tail = ptr;
		}
		else
		{
			assert(tail);
			tail->next = ptr;
			ptr->prev = tail;
			tail = ptr;
		}
		++_size;
	}
	
	template <class T, class A>
	template <class... Args>
	void List<T, A>::emplace_back(Args&&... args)
	{
		auto ptr = nalloc.allocate(1);
		nalloc.construct(ptr, Utils::forward<Args>(args)...);
		if (!head)
		{
			head = ptr;
			tail = ptr;
		}
		else
		{
			tail->next = ptr;
			ptr->prev = tail;
			tail = ptr;
		}
		++_size;
	}
	
	template <class T, class A>
	typename List<T, A>::size_type List<T, A>::size() const noexcept
	{
		return _size;
	}
	
	template <class T, class A>
	void List<T, A>::clear()
	{
		Node* ptr = head;
		while (head != nullptr)
		{
			head = head->next;
			nalloc.destroy(ptr);
			nalloc.deallocate(ptr, 1);
			ptr = head;
		}
		_size = 0;
		tail = nullptr;
	}
	
	template <class T, class A>
	auto List<T, A>::erase(iterator it) -> iterator
	{
		iterator new_it;
		assert(it.n);
		
		new_it.n = it.n->next;
		
		if (it.n == head)
		{
			head = it.n->next;
		}
		if (it.n == tail)
		{
			tail = it.n->prev;
		}
		
		if (it.n->next)
		{
			it.n->next->prev = it.n->prev;
		}
		if (it.n->prev)
		{
			it.n->prev->next = it.n->next;
		}
		
		nalloc.destroy(it.n);
		nalloc.deallocate(it.n, 1);
		
		--_size;
		
		return new_it;
	}
	
	template <class T, class A>
	auto List<T, A>::insert(iterator pos, const T& item) -> iterator
	{
		iterator new_it;
		if (!pos.n)
		{
			push_back(item);
			new_it.n = tail;
			return new_it;
		}
		
		
		auto ptr = nalloc.allocate(1);
		assert(ptr);
		assert((uintptr_t)ptr % alignof(Node) == 0);
		nalloc.construct(ptr, item);
		new_it.n = ptr;
		ptr->next = pos.n;
		ptr->prev = pos.n->prev;
		if (head == pos.n)
		{
			head = ptr;
		}
		else
		{
			pos.n->prev->next = ptr;
		}
		pos.n->prev = ptr;
		
		++_size;
		
		return new_it;
	}
	
	template <class T, class A>
	bool List<T, A>::empty() const noexcept
	{
		return size() == 0;
	}
	
	template <class T, class A>
	auto List<T, A>::front() -> reference
	{
		assert(head);
		return head->elem;
	}
	
	template <class T, class A>
	auto List<T, A>::front() const -> const_reference
	{
		assert(head);
		return head->elem;
	}
	
	template <class T, class A>
	auto List<T, A>::back() -> reference
	{
		assert(tail);
		return tail->elem;
	}
	
	template <class T, class A>
	auto List<T, A>::back() const -> const_reference
	{
		assert(tail);
		return tail->elem;
	}
	
	
	template <class T, class A>
	List<T, A>& List<T, A>::operator=(List&& l)
	{
		Node* ptr = head;
		while (head != nullptr)
		{
			head = head->next;
			nalloc.destroy(ptr);
			nalloc.deallocate(ptr, 1);
			ptr = head;
		}
		
		head = l.head;
		tail = l.tail;
		alloc = l.alloc;
		nalloc = l.nalloc;
		_size = l._size;
		
		l.head = l.tail = nullptr;
		l._size = 0;
		
		return *this;
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
	
	template <class T, class A>
	typename List<T, A>::const_iterator List<T, A>::cbegin() const
	{
		const_iterator it;
		it.n = head;
		return it;
	}

	template <class T, class A>
	typename List<T, A>::const_iterator List<T, A>::cend() const
	{
		const_iterator it;
		it.n = nullptr;
		return it;
	}
	
	
}
#endif