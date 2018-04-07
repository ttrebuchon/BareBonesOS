#ifndef INCLUDED_LIST_HH
#define INCLUDED_LIST_HH

#include "Allocator.hh"
#include "Allocator_Traits.hh"

namespace Utils
{
	namespace detail
	{
		template <class T>
		class List_Node
		{
			private:
			
			template <class NA>
			List_Node* clone(NA& alloc, List_Node* prev, List_Node** tail) const
			{
				List_Node* n = alloc.allocate(1);
				alloc.construct(n, elem);
				if (prev)
				{
					n->prev = prev;
				}
				if (next)
				{
					n->next = next->clone(alloc, n, tail);
				}
				else if (tail)
				{
					*tail = n;
				}
				return n;
			}
			
			public:
			List_Node *prev, *next;
			T elem;
			
			List_Node(const T& value) : prev(nullptr), next(nullptr), elem(value)
			{
				
			}
			
			List_Node(T&& value) : prev(nullptr), next(nullptr), elem(forward<T&&>(value))
			{
				
			}
			
			template <class... Args>
			List_Node(Args... args) : prev(nullptr), next(nullptr), elem(forward<Args>(args)...)
			{
				
			}
			
			template <class NA>
			List_Node* clone(NA& alloc, bool includeNext = true, List_Node** tail = nullptr) const
			{
				typename NA::template rebind<List_Node>::other nalloc(alloc);
				if (includeNext)
				{
					return clone(nalloc, nullptr, tail);
				}
				List_Node* n = nalloc.allocate(1);
				nalloc.construct(n, elem);
				if (tail && !next)
				{
					*tail = n;
				}
				return n;
			}
		};
		
	}
	
	template <class T, class Alloc = Allocator<T>>
	class List
	{
		protected:
		typedef detail::List_Node<T> Node;
		
		public:
		
		typedef T               value_type;
		typedef Alloc           allocator_type;
        typedef typename Allocator_Traits<Alloc>::pointer pointer;
        typedef typename Allocator_Traits<Alloc>::const_pointer const_pointer;
        typedef typename Allocator_Traits<Alloc>::reference reference;
        typedef typename Allocator_Traits<Alloc>::const_reference const_reference;
        typedef size_t          size_type;
		
		class iterator
		{
			Node* n;

			public:

			iterator& operator++()
			{
				n = n->next;
				return *this;
			}

			bool operator==(const iterator it2) const
			{
				return n == it2.n;
			}

			bool operator!=(const iterator it2) const
			{
				return n != it2.n;
			}

			T& operator*()
			{
				return n->elem;
			}

			friend List<T, Alloc>;
		};
		
		protected:
		Node* head;
		Node* tail;
		size_type _size;
		Alloc alloc;
		typename Alloc::template rebind<Node>::other nalloc;
		
		
		public:
		
		//Constructors
		List();
		List(const size_t);
		List(const List&);
		template <class InputIt>
		List(InputIt start, InputIt end);
		List(List&&) noexcept;
		
		//Destructor
		~List() noexcept;
		
		
		void push_back(const T&);
		void push_back(T&&);
		template <class... Args>
		void emplace_back(Args&&...);

		size_type size() const;

		iterator begin();
		iterator end();
	};
	
	template <class T, class A = Allocator<T>>
	using list = List<T, A>;
	
}
#endif
#include "List.hpp"