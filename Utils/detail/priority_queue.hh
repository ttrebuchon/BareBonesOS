#ifndef INCLUDED_PRIORITY_QUEUE_HH
#define INCLUDED_PRIORITY_QUEUE_HH

#include <Common.h>
#include <Utils/vector>
#include <Utils/Compare.hh>

namespace Utils
{
	template <class T, class Container = vector<T>, class Compare = less<typename Container::value_type>>
	class priority_queue
	{
		public:
		typedef T value_type;
		typedef Container container_type;
		typedef Compare value_compare;
		typedef typename Container::reference reference;
		typedef typename Container::const_reference const_reference;
		typedef typename container_type::size_type size_type;
		
		
		
		protected:
		container_type c;
		value_compare comp;
		
		
		public:
		priority_queue(const value_compare&, const container_type&);
		explicit priority_queue(const value_compare& = value_compare(), container_type&& = container_type());
		
		template <class Alloc>
		explicit priority_queue(const Alloc&);
		
		template <class Alloc>
		priority_queue(const value_compare&, const Alloc&);
		
		template <class Alloc>
		priority_queue(const value_compare&, const container_type&, const Alloc&);
		
		template <class Alloc>
		priority_queue(const value_compare&, container_type&&, const Alloc&);
		
		~priority_queue() = default;
		
		
		
		const_reference top() const;
		
		__attribute__((always_inline))
		[[ nodiscard ]]
		bool empty() const
		{ return size() == 0; }
		
		size_type size() const;
		void push(const value_type&);
		void push(value_type&&);
		
		template <class... Args>
		void emplace(Args&&...);
		void pop();
		
		
		
	};
	
}

#endif