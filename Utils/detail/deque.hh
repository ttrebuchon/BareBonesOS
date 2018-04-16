#ifndef INCLUDED_DEQUE_HH
#define INCLUDED_DEQUE_HH

#include <Common.h>
#include <Utils/allocator.hh>
#include <Utils/allocator_traits.hh>
#include <Utils/iterator_traits.hh>

namespace Utils
{
	template <class T, class Alloc = allocator<T>>
	class deque
	{
		public:
		typedef T value_type;
		typedef Alloc allocator_type;
		typedef T& reference;
		typedef const T& const_reference;
		typedef typename allocator_traits<allocator_type>::pointer pointer;
		typedef typename allocator_traits<allocator_type>::const_pointer const_pointer;
		typedef size_t size_type;
		
		class iterator
		{
			public:
			typedef ptrdiff_t difference_type;
			typedef typename deque::value_type value_type;
			typedef typename deque::pointer pointer;
			typedef typename deque::reference reference;
			typedef random_access_iterator_tag iterator_category;
			
		};
		
		class const_iterator
		{
			public:
			typedef ptrdiff_t difference_type;
			typedef const typename deque::value_type value_type;
			typedef typename deque::const_pointer pointer;
			typedef typename deque::const_reference reference;
			typedef random_access_iterator_tag iterator_category;
		};
		
		
		typedef typename iterator_traits<iterator>::difference_type difference_type;
		typedef Utils::reverse_iterator<iterator> reverse_iterator;
		typedef Utils::reverse_iterator<const_iterator> const_reverse_iterator;
		
		
		protected:
		
		
		public:
		deque();
		explicit deque(const allocator_type&);
		explicit deque(size_type, const allocator_type& = allocator_type());
		deque(size_type, const value_type&, const allocator_type& = allocator_type());
		
		
		deque(const deque&);
		deque(const deque&, const allocator_type&);
		deque(deque&&);
		deque(deque&&, const allocator_type&);
		
		
		
		size_type size() const noexcept;
		size_type max_size() const noexcept;
		void resize(size_type);
		void resize(size_type, const value_type&);
		__attribute__((always_inline))
		bool empty() const noexcept
		{ return size() == 0; }
		void shrink_to_fit();
		reference at(size_type);
		const_reference at(size_type) const;
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		void clear();
		void push_back(const value_type&);
		void push_back(value_type&&);
		template <class... Args>
		reference emplace_back(Args&&...);
		void pop_back();
		void push_front(const value_type&);
		void push_front(value_type&&);
		template <class... Args>
		reference emplace_front(Args&&...);
		void pop_front();
		void assign(size_type, const value_type&);
		allocator_type get_allocator() const;
		
		
		
		
		reference operator[](size_type);
		const_reference operator[](size_type) const;
		deque& operator=(const deque&);
		deque& operator=(deque&&) noexcept(allocator_traits<allocator_type>::is_always_equal::value);
		
		
		
		iterator begin() noexcept;
		iterator end() noexcept;
		const_iterator begin() const noexcept;
		const_iterator end() const noexcept;
		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;
		
	};
	
}

#endif