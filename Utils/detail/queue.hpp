#ifndef INCLUDED_QUEUE_HPP
#define INCLUDED_QUEUE_HPP

#include "Queue.hh"

namespace Utils
{
	template <class T, class Container>
	queue<T, Container>::queue(const container_type& c) : c(c)
	{
		
	}
	
	template <class T, class Container>
	queue<T, Container>::queue(container_type&& c) : c(forward<container_type&&>(c))
	{
		
	}
	
	template <class T, class Container>
	template <class Alloc>
	queue<T, Container>::queue(const Alloc& a) : c(a)
	{
		
	}
	
	template <class T, class Container>
	auto queue<T, Container>::front() -> reference
	{
		return c.front();
	}
	
	template <class T, class Container>
	auto queue<T, Container>::front() const -> const_reference
	{
		return c.front();
	}
	
	template <class T, class Container>
	auto queue<T, Container>::back() -> reference
	{
		return c.back();
	}
	
	template <class T, class Container>
	auto queue<T, Container>::back() const -> const_reference
	{
		return c.back();
	}
	
	template <class T, class Container>
	auto queue<T, Container>::size() const -> size_type
	{
		return c.size();
	}
	
	template <class T, class Container>
	void queue<T, Container>::push(const value_type& v)
	{
		c.push_back(v);
	}
	
	template <class T, class Container>
	void queue<T, Container>::push(value_type&& v)
	{
		c.push_back(forward<value_type&&>(v));
	}
	
	template <class T, class Container>
	template <class... Args>
	decltype(auto) queue<T, Container>::emplace(Args&&... args)
	{
		return c.emplace_back(forward<Args...>(args)...);
	}
	
	template <class T, class Container>
	void queue<T, Container>::pop()
	{
		c.pop_front();
	}
}

#endif