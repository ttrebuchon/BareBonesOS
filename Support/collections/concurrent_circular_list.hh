#ifndef INCLUDED_SUPPORT_COLLECTIONS_CONCURRENT_CIRCULAR_LIST_HH
#define INCLUDED_SUPPORT_COLLECTIONS_CONCURRENT_CIRCULAR_LIST_HH

#include <Common.h>
#include <Utils/Allocator.hh>
#include <Utils/Allocator_Traits.hh>
#include <Support/threading/Mutex.hh>

namespace Support::Collections
{
	namespace detail
	{
		template <class T>
		class concurrent_circular_list_node
		{
			private:
			
			template <class NA>
			concurrent_circular_list_node* clone(NA& alloc, concurrent_circular_list_node* prev, concurrent_circular_list_node** tail) const
			{
				concurrent_circular_list_node* n = alloc.allocate(1);
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
			concurrent_circular_list_node *prev, *next;
			T elem;
			
			concurrent_circular_list_node(const T& value) : prev(nullptr), next(nullptr), elem(value)
			{
				
			}
			
			concurrent_circular_list_node(T&& value) : prev(nullptr), next(nullptr), elem(Utils::forward<T&&>(value))
			{
				
			}
			
			template <class... Args>
			concurrent_circular_list_node(Args... args) : prev(nullptr), next(nullptr), elem(Utils::forward<Args>(args)...)
			{
				
			}
			
			template <class NA>
			concurrent_circular_list_node* clone(NA& alloc, bool includeNext = true, concurrent_circular_list_node** tail = nullptr) const
			{
				typename NA::template rebind<concurrent_circular_list_node>::other nalloc(alloc);
				if (includeNext)
				{
					return clone(nalloc, nullptr, tail);
				}
				concurrent_circular_list_node* n = nalloc.allocate(1);
				nalloc.construct(n, elem);
				if (tail && !next)
				{
					*tail = n;
				}
				return n;
			}
		};
		
	}
	
	template <class T, class Mutex = Support::threading::spin_yield_mutex, class Alloc = Utils::allocator<T>>
	class concurrent_circular_list
	{
		protected:
		typedef detail::concurrent_circular_list_node<T> Node;
		
		public:
		
		typedef T value_type;
		typedef Alloc allocator_type;
        typedef typename Utils::Allocator_Traits<Alloc>::pointer pointer;
        typedef typename Utils::Allocator_Traits<Alloc>::const_pointer const_pointer;
        typedef typename Utils::Allocator_Traits<Alloc>::reference reference;
        typedef typename Utils::Allocator_Traits<Alloc>::const_reference const_reference;
        //typedef Utils::atomic<size_t> size_type;
        typedef size_t size_type;
        typedef Mutex mutex_type;
		
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

			friend concurrent_circular_list<T, Mutex, Alloc>;
		};
		
		class const_iterator
		{
			const Node* n;

			public:

			const_iterator& operator++()
			{
				n = n->next;
				return *this;
			}

			bool operator==(const const_iterator it2) const
			{
				return n == it2.n;
			}

			bool operator!=(const const_iterator it2) const
			{
				return n != it2.n;
			}

			const T& operator*()
			{
				return n->elem;
			}

			friend concurrent_circular_list<T, Mutex, Alloc>;
		};
		
		protected:
		Node* head;
		Node* tail;
		size_type _size;
		allocator_type alloc;
		typename allocator_type::template rebind<Node>::other nalloc;
		mutex_type mutex;
		
		
		void insert_node(Node*);
		
		
		public:
		
		//Constructors
		concurrent_circular_list();
		concurrent_circular_list(const size_t);
		concurrent_circular_list(const concurrent_circular_list&);
		template <class InputIt>
		concurrent_circular_list(InputIt start, InputIt end);
		concurrent_circular_list(concurrent_circular_list&&) noexcept;
		
		//Destructor
		~concurrent_circular_list() noexcept;
		
		// Operations
		void push_back(const T&);
		void push_back(T&&);
		template <class... Args>
		void emplace_back(Args&&...);
		size_type size() const noexcept;
		void clear();
		iterator erase(iterator);
		
		
		// Operators
		concurrent_circular_list& operator=(concurrent_circular_list&&);
		
		
		
		// Iterator Methods
		iterator begin();
		iterator end();
		const_iterator cbegin() const;
		const_iterator cend() const;
		const_iterator begin() const
		{
			return cbegin();
		}
		const_iterator end() const
		{
			return cend();
		}
		
		
		auto get_n(iterator it) const
		{
			return it.n;
		}
	};
	
}
#endif
#include "concurrent_circular_list.hpp"