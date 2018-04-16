#ifndef INCLUDED_PRIORITY_QUEUE_HPP
#define INCLUDED_PRIORITY_QUEUE_HPP

#include "priority_queue.hh"
#include "bits/heap.hh"

namespace Utils
{
	template <class T, class Container, class Compare>
	priority_queue<T, Container, Compare>::priority_queue(const value_compare& comp, const container_type& c) : c(c), comp(comp)
	{
		make_heap(c.begin(), c.end(), comp);
	}
	
	template <class T, class Container, class Compare>
	priority_queue<T, Container, Compare>::priority_queue(const value_compare& comp, container_type&& c) : c(forward<container_type&&>(c)), comp(comp)
	{
		make_heap(c.begin(), c.end(), comp);
	}
	
	
	
	
	template <class T, class Container, class Compare>
	auto priority_queue<T, Container, Compare>::top() const -> const_reference
	{
		return c.front();
	}
	
	template <class T, class Container, class Compare>
	auto priority_queue<T, Container, Compare>::size() const -> size_type
	{
		return c.size();
	}
	
	template <class T, class Container, class Compare>
	void priority_queue<T, Container, Compare>::push(const value_type& v)
	{
		c.push_back(v);
		push_heap(c.begin(), c.end(), comp);
	}
	
	template <class T, class Container, class Compare>
	void priority_queue<T, Container, Compare>::push(value_type&& v)
	{
		c.push_back(forward<value_type&&>(v));
		push_heap(c.begin(), c.end(), comp);
	}
	
	template <class T, class Container, class Compare>
	template <class... Args>
	void priority_queue<T, Container, Compare>::emplace(Args&&... args)
	{
		c.emplace_back(forward<Args>(args)...);
		push_heap(c.begin(), c.end(), comp);
	}
	
	template <class T, class Container, class Compare>
	void priority_queue<T, Container, Compare>::pop()
	{
		pop_heap(c.begin(), c.end(), comp);
		c.pop_back();
	}
}


#endif