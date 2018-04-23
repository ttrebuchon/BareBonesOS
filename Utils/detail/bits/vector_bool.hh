#ifndef INCLUDED_BITS_VECTOR_BOOL_HH
#define INCLUDED_BITS_VECTOR_BOOL_HH

#include <Common.h>
#include <Utils/vector.hh>

namespace Utils
{
	
	template <class Alloc>
	class vector<bool, Alloc>
	{
		protected:
		typedef uchar storage_type;
		typedef typename Alloc::template rebind<storage_type>::other _alloc_type;
		typedef Allocator_Traits<_alloc_type> ATraits;
		
		public:
		
		typedef bool               value_type;
		typedef Alloc           allocator_type;
        typedef typename Allocator_Traits<Alloc>::pointer pointer;
        typedef typename Allocator_Traits<Alloc>::const_pointer const_pointer;
        typedef bool const_reference;
        typedef size_t size_type;
        
        class reference
        {
        	protected:
        	storage_type& item;
        	storage_type mask;
        	
        	reference(storage_type& s, const storage_type mask) noexcept : item(s), mask(mask)
        	{}
        	
        	public:
        	~reference() = default;
        	
        	operator bool() const noexcept
        	{
        		return (item & mask) != 0;
        	}
        	
        	reference& operator=(bool x) noexcept
        	{
        		if (x)
        		{
        			item |= mask;
        		}
        		else
        		{
        			item &= ~mask;
        		}
        		return *this;
        	}
        	
        	reference& operator=(const reference& rhs) noexcept
        	{
        		return (*this) = ((bool)rhs);
        	}
        	
        	reference& flip() noexcept
        	{
        		item &= ~(mask & item);
        		return *this;
        	}
        	
        	bool operator~() const noexcept
        	{
        		return ~(mask & item) != 0;
        	}
        	
        	friend class vector<bool, Alloc>;
        };
        
        
        
        class iterator
        {
        	protected:
        	storage_type* ptr;
        	storage_type mask;
        	
        	public:
        };
        
        class const_iterator
        {
        	protected:
        	const storage_type* ptr;
        	storage_type mask;
        	
        	public:
        };
        
        
        
		
		
		protected:
		storage_type* _data;
		size_type _size;
		size_type _cap;
		size_type _under_count;
		allocator_type alloc;
		
		constexpr static size_t storage_bits = sizeof(storage_type)*8;
		
		void ensure_capacity(size_type);
		void ensure_room_for(size_type n)
		{
			ensure_capacity(_size + n);
		}
		void reallocate(size_type);
		_alloc_type getAlloc()
		{
			return _alloc_type(alloc);
		}
		
		
		
		public:
		constexpr vector();
		constexpr explicit vector(const allocator_type&);
		explicit vector(size_type n, const allocator_type& alloc = allocator_type());
		vector(size_type n, const value_type& v, const allocator_type& alloc = allocator_type());
		template <class InputIt>
		vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type());
		vector(const vector&);
		vector(const vector&, const allocator_type&);
		vector(vector&&);
		vector(vector&&, const allocator_type&);
		//vector(initializer_list<value_type> il, const allocator_type& = allocator_type());
		~vector();
		
		// Member Functions
		constexpr size_type size() const noexcept;
		constexpr size_type max_size() const noexcept;
		void resize(size_type);
		void resize(size_type, const value_type&);
		constexpr size_type capacity() const noexcept;
		constexpr bool empty() const noexcept;
		void reserve(size_type);
		void shrink_to_fit();
		reference at(size_type);
		const_reference at(size_type) const;
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		
		template <class InputIt>
		void assign(InputIt first, InputIt last);
		void assign(size_type n, const value_type&);
		//void assign(initializer_list<value_type>);
		void push_back(const value_type&);
		void push_back(value_type&&);
		void pop_back();
		iterator insert(const_iterator pos, const value_type&);
		iterator insert(const_iterator pos, size_type n, const value_type&);
		template <class InputIt>
		iterator insert(const_iterator pos, InputIt first, InputIt last);
		iterator insert(const_iterator, value_type&&);
		//iterator insert(const_iterator, initializer_list<value_type>);
		iterator erase(const_iterator);
		iterator erase(const_iterator, const_iterator);
		void swap(vector&);
		void clear() noexcept;
		template <class... Args>
		iterator emplace(const_iterator, Args&&... args);
		template <class... Args>
		void emplace_back(Args&&... args);
		allocator_type get_allocator() const noexcept;
		void flip();
		static void swap(reference x, reference y);
		
		
		// Operators
		reference operator[](size_type i);
		const_reference operator[](size_type) const;
		
		
		// Iterators
		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;
		
	};

}
#endif
