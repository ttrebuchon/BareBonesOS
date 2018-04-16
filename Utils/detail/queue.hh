#ifndef INCLUDED_QUEUE_HH
#define INCLUDED_QUEUE_HH

#include <Common.h>
#include <Utils/deque>

namespace Utils
{
	template <class T, class Container = deque<T>>
	class queue
	{
		public:
		typedef T value_type;
		typedef Container container_type;
		typedef typename Container::reference reference;
		typedef typename Container::const_reference const_reference;
		typedef size_t size_type;
		
		
		
		protected:
		container_type c;
		
		
		public:
		explicit queue(const container_type&);
		explicit queue(container_type&& = container_type());
		template <class Alloc>
		explicit queue(const Alloc&);
		
		~queue() = default;
		
		
		
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		
		__attribute__((always_inline))
		[[ nodiscard ]]
		bool empty() const
		{ return size() == 0; }
		
		size_type size() const;
		void push(const value_type&);
		void push(value_type&&);
		
		template <class... Args>
		decltype(auto) emplace(Args&&...);
		void pop();
		
		
		
	};
	
}

#endif