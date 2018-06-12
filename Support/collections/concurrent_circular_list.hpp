#ifndef INCLUDED_SUPPORT_COLLECTIONS_CONCURRENT_LIST_HPP
#define INCLUDED_SUPPORT_COLLECTIONS_CONCURRENT_LIST_HPP

#include "concurrent_circular_list.hh"
#include <Utils/detail/locks.hh>

namespace Support::Collections
{
	template <class T, class Mut, class A>
	concurrent_circular_list<T, Mut, A>::concurrent_circular_list() : head(nullptr), tail(nullptr), _size(0), alloc(), nalloc()
	{
		head = tail = nullptr;
	}
	
	template <class T, class Mut, class A>
	concurrent_circular_list<T, Mut, A>::concurrent_circular_list(concurrent_circular_list&& r) noexcept : head(r.head), tail(r.tail), _size(r._size), alloc(r.alloc), nalloc(r.nalloc)
	{
		r.head = r.tail = nullptr;
		r._size = 0;
	}
	
	
	template <class T, class Mut, class A>
	concurrent_circular_list<T, Mut, A>::~concurrent_circular_list() noexcept
	{
		clear();
		/*Node* ptr = head;
		while (head != nullptr)
		{
			head = head->next;
			nalloc.destroy(ptr);
			nalloc.deallocate(ptr, 1);
			ptr = head;
		}*/
	}
	
	template <class T, class Mut, class A>
	void concurrent_circular_list<T, Mut, A>::push_back(const T& t)
	{
		
		auto ptr = nalloc.allocate(1);
		nalloc.construct(ptr, Utils::forward<const T&>(t));
		insert_node(ptr);
		/*__sync_synchronize();
		if (__sync_bool_compare_and_swap(&tail, nullptr, ptr))
		{
			__sync_synchronize();
			head = ptr;
		}
		else
		{
			auto t = tail;
			while (!__sync_bool_compare_and_swap(&t->next, nullptr, ptr))
			{
				t = tail;
			}
			
			__sync_synchronize();
			tail = ptr;
			__sync_synchronize();
		}
		__sync_add_and_fetch(&_size, 1);*/
	}
	
	template <class T, class Mut, class A>
	void concurrent_circular_list<T, Mut, A>::push_back(T&& t)
	{
		auto ptr = nalloc.allocate(1);
		nalloc.construct(ptr, Utils::forward<T&&>(t));
		insert_node(ptr);
		/*__sync_synchronize();
		if (__sync_bool_compare_and_swap(&tail, nullptr, ptr))
		{
			__sync_synchronize();
			head = ptr;
		}
		else
		{
			auto t = tail;
			while (!__sync_bool_compare_and_swap(&t->next, nullptr, ptr))
			{
				t = tail;
			}
			
			__sync_synchronize();
			tail = ptr;
			__sync_synchronize();
		}
		__sync_add_and_fetch(&_size, 1);*/
	}
	
	template <class T, class Mut, class A>
	template <class... Args>
	void concurrent_circular_list<T, Mut, A>::emplace_back(Args&&... args)
	{
		auto ptr = nalloc.allocate(1);
		nalloc.construct(ptr, Utils::forward<Args>(args)...);
		insert_node(ptr);
		/*__sync_synchronize();
		if (__sync_bool_compare_and_swap(&tail, nullptr, ptr))
		{
			__sync_synchronize();
			head = ptr;
		}
		else
		{
			auto t = tail;
			while (!__sync_bool_compare_and_swap(&t->next, nullptr, ptr))
			{
				t = tail;
			}
			
			__sync_synchronize();
			tail = ptr;
			__sync_synchronize();
		}
		__sync_add_and_fetch(&_size, 1);*/
	}
	
	template <class T, class Mut, class A>
	typename concurrent_circular_list<T, Mut, A>::size_type concurrent_circular_list<T, Mut, A>::size() const noexcept
	{
		return _size;
	}
	
	template <class T, class Mut, class A>
	void concurrent_circular_list<T, Mut, A>::clear()
	{
		Node* ptr = head;
		if (ptr)
		{
			if (ptr->prev)
			{
				ptr->prev->next = nullptr;
			}
		}
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
	
	template <class T, class Mut, class A>
	auto concurrent_circular_list<T, Mut, A>::erase(iterator it) -> iterator
	{
		iterator new_it;
		assert(it.n);
		
		new_it.n = it.n->next;
		
		auto n = it.n;
		assert(n->prev);
		assert(n->prev->next);
		assert(n->prev->next == n);
		
		
		
		//while (!__sync_bool_compare_and_swap(&n->prev->next, n, n->next)) ;
		n->next->prev = n->prev;
		
		__sync_synchronize();
		if (head == n || tail == n)
		{
			Utils::lock_guard<mutex_type> lock(mutex);
			new_it.n = it.n->next;
			if (head == n || tail == n)
			{
				if (tail != n)
				{
					assert(head == n);
					head = head->next;
					assert(head != n);
					head->prev = n->prev;
					head->prev->next = head;
				}
				else if (head != n)
				{
					assert(tail == n);
					tail = tail->prev;
					assert(tail != n);
					tail->next = n->next;
					n->next->prev = tail;
				}
				else // <- head == tail
				{
					head = tail = nullptr;
				}
				nalloc.destroy(n);
				nalloc.deallocate(n, 1);
				__sync_sub_and_fetch(&_size, 1);
				return new_it;
			}
		}
		
		assert(__sync_bool_compare_and_swap(&n->prev->next, n, n->next));
		
		__sync_sub_and_fetch(&_size, 1);
		nalloc.destroy(it.n);
		nalloc.deallocate(it.n, 1);
		return new_it;
		
		
		
		if (head == tail)
		{
			assert(head == it.n);
			head = tail = nullptr;
		}
		else if (it.n == head)
		{
			head = it.n->next;
		}
		if (it.n == tail)
		{
			if (head == nullptr)
			{
				
			}
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
	
	
	template <class T, class Mut, class A>
	concurrent_circular_list<T, Mut, A>& concurrent_circular_list<T, Mut, A>::operator=(concurrent_circular_list&& l)
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

	template <class T, class Mut, class A>
	typename concurrent_circular_list<T, Mut, A>::iterator concurrent_circular_list<T, Mut, A>::begin()
	{
		iterator it;
		it.n = head;
		return it;
	}

	template <class T, class Mut, class A>
	typename concurrent_circular_list<T, Mut, A>::iterator concurrent_circular_list<T, Mut, A>::end()
	{
		iterator it;
		it.n = nullptr;
		return it;
	}
	
	template <class T, class Mut, class A>
	typename concurrent_circular_list<T, Mut, A>::const_iterator concurrent_circular_list<T, Mut, A>::cbegin() const
	{
		const_iterator it;
		it.n = head;
		return it;
	}

	template <class T, class Mut, class A>
	typename concurrent_circular_list<T, Mut, A>::const_iterator concurrent_circular_list<T, Mut, A>::cend() const
	{
		const_iterator it;
		it.n = nullptr;
		return it;
	}
	
	
	
	
	
	
	
	
	
	
	
	// Protected Methods
	
	
	template <class T, class Mut, class A>
	void concurrent_circular_list<T, Mut, A>::insert_node(Node* ptr)
	{
		Utils::lock_guard<mutex_type> lock(mutex);
		ptr->next = head;
		if (!head)
		{
			ptr->next = ptr;
			ptr->prev = ptr;
			head = tail = ptr;
		}
		else
		{
			assert(tail);
			ptr->prev = tail;
			ptr->next = tail->next;
			tail->next->prev = ptr;
			tail->next = ptr;
			tail = ptr;
		}
		__sync_add_and_fetch(&_size, 1);
		assert(ptr->prev);
		assert(ptr->prev->next == ptr);
	}
	
}
#endif