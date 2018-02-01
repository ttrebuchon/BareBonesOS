#ifndef INCLUDED_LIST_HH
#define INCLUDED_LIST_HH

#include "Allocator.hh"
#include "Allocator_Traits.hh"

namespace Utils
{
	namespace detail
	{
		template <class T, class A>
		class List_Node
		{
			List_Node *prev, *next;
			unsigned char elem[sizeof(T)];
			
			List_Node() : prev(nullptr), next(nullptr), elem()
			{
				
			}
			
			List_Node(const T& t, A& alloc) : prev(nullptr), next(nullptr), elem()
			{
				alloc.template construct<T>(reinterpret_cast<T*>(elem), t);
			}
			
			
			template <class... Args>
			void init(A& alloc, Args&&... args)
			{
				alloc.template construct<T>(reinterpret_cast<T*>(elem), args...);
			}
			
			template <class, class>
			friend class List;
		};
		
	}
	
	template <class T, class Alloc = Allocator<T>>
	class List
	{
		protected:
		typedef detail::List_Node<T, Alloc> Node;
		
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
		
		//Destructor
		~List() noexcept;
		
		
		void push_back(const T&);
		void push_back(T&&);
	};
	
	
	
}
#endif
#include "List.hpp"