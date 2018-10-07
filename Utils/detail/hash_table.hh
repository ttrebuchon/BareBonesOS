#ifndef INCLUDED_HASH_TABLE_HH
#define INCLUDED_HASH_TABLE_HH

#include <Common.h>
#include "hash_table_policy.hh"
#include <Utils/Compare.hh>
#include <Utils/Allocator.hh>
#include <Utils/Allocator_Traits.hh>
#include <Utils/hash>

namespace Utils
{
	namespace detail
	{
		struct hash_node_base
		{
			hash_node_base* next;
			
			hash_node_base() : next(nullptr)
			{
				
			}
		};
		
		template <class T, class Code>
		struct hash_node : public hash_node_base
		{
			T value;
			Code code;
			
			template <class... Args>
			hash_node(Args&&... args) : hash_node_base(), value(forward<Args&&>(args)...), code(0)
			{
				
			}
		};
		
		template <class Key, class T, class Selector, class Equals, class Table>
		struct hash_table_conflict_resolution
		{
			constexpr hash_table_conflict_resolution() = default;
			
			enum class Action
			{
				None = 0,
				ReinsertLeft = 1,
				ReinsertRight = 2,
				RemoveLeft = 5,
				RemoveRight = 10,
				
			};
			
			Action operator()(const Key& key, T& left, T& right, const Selector& select, const Equals& eq, Table& table) noexcept
			{
				int act = (int)Action::None;
				if (!eq(select(left), key))
				{
					
					act |= (int)Action::ReinsertLeft;
				}
				
				if (!eq(select(right), key))
				{
					act |= (int)Action::ReinsertRight;
				}
				
				if (!act)
				{
					equal_to<T> teq;
					if (teq(left, right))
					{
						act |= (int)Action::RemoveRight;
					}
				}
				
				return (Action)act;
			}
		};
		
		template <class Key, class T, class Code, class Alloc>
		struct hash_table_node_allocator
		{
			typedef hash_node<T, Code> node_type;
			typedef typename Alloc::template rebind<node_type>::other node_allocator_type;
			
			
			node_type* allocate_node_key(node_allocator_type& alloc, const Key& k)
			{
				node_type* ptr = alloc.allocate(1);
				ptr->next = nullptr;
				ptr->code = 0;
				alloc.construct(&ptr->value, k);
				return ptr;
			}
			
			node_type* allocate_node_value(node_allocator_type& alloc, const T& t)
			{
				node_type* ptr = alloc.allocate(1);
				ptr->next = nullptr;
				ptr->code = 0;
				alloc.construct(&ptr->value, t);
				return ptr;
			}
			
			node_type* allocate_node_value(node_allocator_type& alloc, T&& t)
			{
				node_type* ptr = alloc.allocate(1);
				ptr->next = nullptr;
				ptr->code = 0;
				alloc.construct(&ptr->value, forward<T&&>(t));
				return ptr;
			}
			
			void deallocate_node(node_allocator_type& alloc, node_type* n)
			{
				assert(n);
				alloc.destroy(n);
				alloc.deallocate(n, 1);
			}
		};
		
		/*template <class T, class Code, class Alloc>
		struct hash_table_node_allocator<const T, T, Code, Alloc>
		{
			typedef hash_node<T, Code> node_type;
			typedef typename Alloc::template rebind<node_type>::other node_allocator_type;
			
			node_type* allocate_node_value(node_allocator_type& alloc, const T& t)
			{
				node_type* ptr = alloc.allocate(1);
				ptr->next = nullptr;
				ptr->code = 0;
				alloc.construct(&ptr->value, t);
				return ptr;
			}
			
			node_type* allocate_node_value(node_allocator_type& alloc, T&& t)
			{
				node_type* ptr = alloc.allocate(1);
				ptr->next = nullptr;
				ptr->code = 0;
				alloc.construct(&ptr->value, forward<T&&>(t));
				return ptr;
			}
			
			
		};*/
	}
	
	
	
	template <class Key, class T, class Selector, class Hash = hash<Key>, class Equals = equal_to<Key>, class Alloc = allocator<T>, class Policy = detail::hash_table_policy, template <class...> class Resolution = detail::hash_table_conflict_resolution>
	class hash_table : protected Policy, protected detail::hash_table_node_allocator<Key, T, decltype(declval<Hash>()(declval<Key>())), Alloc>
	{
		public:
		typedef Key key_type;
		typedef T value_type;
		typedef Hash hasher;
		typedef Equals equals_type;
		typedef Alloc allocator_type;
		typedef /*size_t*/int size_type;
		typedef Resolution<key_type, value_type, Selector, equals_type, hash_table> resolution_type;
		
		
		protected:
		
		// Type Definitions
		typedef decltype(declval<hasher>()(declval<key_type>())) hash_code;
		typedef detail::hash_node<T, hash_code>* Bucket;
		typedef detail::hash_node<T, hash_code> _VNode;
		typedef const _VNode c_VNode;
		
		
		typedef typename allocator_type::template rebind<_VNode>::other node_allocator_type;
		
		
		
		// Members
		Selector select;
		hasher hash;
		equals_type eq;
		mutable allocator_type alloc;
		mutable node_allocator_type nalloc;
		Policy _policy;
		resolution_type resolver;
		bool _check_conflicts;
		
		Bucket* buckets;
		detail::hash_node_base* pre;
		size_type bucketCount;
		size_type elemCount;
		detail::hash_node_base beforeBegin;
		
		
		// Protected Methods
		
		_VNode* findNode(const size_type bucketIndex, const Key&, const hash_code);
		const _VNode* findNode(const size_type bucketIndex, const Key&, const hash_code) const;
		detail::hash_node_base* findBeforeNode(const size_type bucketIndex, const Key&, const hash_code);
		//_VNode* allocateNode(const Key&) const;
		//_VNode* allocateNode(const T&) const;
		void _rehash(size_type bCount);
		_VNode* _begin();
		const _VNode* _begin() const;
		int resolve(Bucket&, const key_type&, const hash_code, _VNode* prev_left, _VNode* left, _VNode* prev_right, _VNode* right);
		
		
		
		
		// Protected Static Methods
		
		static _VNode*& next(detail::hash_node_base*);
		static const _VNode* const& next(const detail::hash_node_base*);
		
		public:
		
		// Iterator Types
		
		class const_iterator;
		class iterator
		{
			protected:
			_VNode* node;
			
			iterator(_VNode*);
			
			public:
			
			iterator();
			iterator(const iterator&);
			
			
			iterator& operator++();
			iterator operator++(int);
			value_type& operator*() const;
			value_type* operator->() const;
			
			explicit operator const_iterator() const;
			
			bool operator==(const iterator&) const;
			bool operator!=(const iterator&) const;
			bool operator==(const const_iterator&) const;
			bool operator!=(const const_iterator&) const;
			
			friend class const_iterator;
			friend class hash_table;
		};
		
		class const_iterator
		{
			protected:
			const _VNode* node;
			
			const_iterator(const _VNode*);
			
			public:
			
			const_iterator();
			const_iterator(const const_iterator&);
			explicit const_iterator(const iterator&);
			
			const_iterator& operator++();
			const_iterator operator++(int);
			const value_type& operator*() const;
			
			bool operator==(const const_iterator&) const;
			bool operator!=(const const_iterator&) const;
			bool operator==(const iterator&) const;
			bool operator!=(const iterator&) const;
			
			
			friend class iterator;
			friend class hash_table;
		};
		
		
		
		// Constructors / Destructors 
		
		hash_table(size_type bucketHint, const Selector&, const hasher& hash, const equals_type&, const allocator_type&, bool check_conflicts = false);
		hash_table(const hash_table&);
		//hash_table(hash_table&&);
		~hash_table();
		
		
		hash_table& operator=(const hash_table&);
		hash_table& operator=(hash_table&&);
		
		
		
		_VNode* get(const Key&);
		const _VNode* get(const Key&) const;
		//_VNode* get(Key&&);
		//const _VNode* get(Key&&) const;
		//_VNode* get(const T&);
		//_VNode* getCreate(const Key&, bool* created = nullptr);
		_VNode* getCreate(const T&, bool* created = nullptr);
		//_VNode* getCreate(Key&&, bool* created = nullptr);
		_VNode* getCreate(T&&, bool* created = nullptr);
		bool insertNode(_VNode*);
		_VNode* insertValue(T&& t);
		bool deleteNode(_VNode*);
		__attribute__((__always_inline__))
		bool erase(_VNode* n)
		{
			return deleteNode(n);
		}
		bool erase(const Key& k)
		{
			auto n = get(k);
			if (n)
			{
				return deleteNode(n);
			}
			else
			{
				return false;
			}
		}
		inline size_type size() const
		{
			return elemCount;
		}
		
		allocator_type get_allocator() const noexcept;
		
		void clear();
		inline void rehash(size_type n);
		void reserve(size_type n);
		inline size_type bucketsSize() const noexcept
		{
			return bucketCount;
		}
		inline size_type max_bucketsSize() const noexcept
		{
			return policy().max_buckets();
		}
		inline constexpr const Policy& policy() const noexcept;
		constexpr float max_load_factor() const noexcept;
		void max_load_factor(float);
		constexpr hasher hash_function() const noexcept;
		constexpr equals_type key_eq() const noexcept;
		size_type size_at(size_type) const;
		bool check_conflicts() const noexcept
		{ return _check_conflicts; }
		bool check_conflicts(bool nval) noexcept
		{
			bool old = _check_conflicts;
			_check_conflicts = nval;
			return old;
		}
		
		
		
		
		
		
		size_type bucketIndex_value(const T& t) const;
		size_type bucketIndex_key(const Key& k) const;
		size_type bucketIndex(const hash_code h) const;
		size_type bucketIndex(const _VNode*) const;
		bool equiv_kn(const Key&, const _VNode*) const;
		bool equiv_key(const Key&, const Key&) const;
		bool equiv_kv(const Key&, const T&) const;
		inline bool equiv_vn(const T& l, const _VNode* r) const
		{
			return equiv_kn(select(l), r);
		}
		inline bool equiv_vk(const T& l, const Key& r) const
		{
			return equiv_key(select(l), r);
		}
		inline bool equiv_value(const T& l, const T& r) const
		{
			return equiv_key(select(l), select(r));
		}
		bool equiv_node(const _VNode*, const _VNode*) const;
		
		
		inline hash_code getCode_value(const T&) const;
		inline hash_code getCode_key(const Key&) const;
		inline hash_code getCode_node(const _VNode*) const;
		
		
		
		
		
		// Iteration
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
		
		
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*template <class T, class Selector, class Hash, class Equals, class Alloc, class Policy>
	class hash_table<T, T, Selector, Hash, Equals, Alloc, Policy> : protected Policy
	{
		public:
		typedef T key_type;
		typedef T value_type;
		typedef Hash hasher;
		typedef Equals equals_type;
		typedef Alloc allocator_type;
		typedef int size_type;
		
		
		protected:
		
		// Type Definitions
		typedef decltype(declval<hasher>()(declval<key_type>())) hash_code;
		typedef detail::hash_node<T, hash_code>* Bucket;
		typedef detail::hash_node<T, hash_code> _VNode;
		typedef const _VNode c_VNode;
		
		
		typedef typename allocator_type::template rebind<_VNode>::other node_allocator_type;
		
		
		
		// Members
		hasher hash;
		equals_type eq;
		mutable allocator_type alloc;
		mutable node_allocator_type nalloc;
		Policy _policy;
		
		Bucket* buckets;
		detail::hash_node_base* pre;
		size_type bucketCount;
		size_type elemCount;
		detail::hash_node_base beforeBegin;
		
		
		// Protected Methods
		
		_VNode* findNode(const size_type bucketIndex, const key_type&, const hash_code);
		const _VNode* findNode(const size_type bucketIndex, const key_type&, const hash_code) const;
		detail::hash_node_base* findBeforeNode(const size_type bucketIndex, const key_type&, const hash_code);
		_VNode* allocateNode(const T&) const;
		void _rehash(size_type bCount);
		_VNode* _begin();
		const _VNode* _begin() const;
		
		
		
		
		// Protected Static Methods
		
		static _VNode*& next(detail::hash_node_base*);
		static const _VNode* const& next(const detail::hash_node_base*);
		
		public:
		
		// Iterator Types
		
		class const_iterator;
		class iterator
		{
			protected:
			_VNode* node;
			
			iterator(_VNode*);
			
			public:
			
			iterator();
			iterator(const iterator&);
			
			
			iterator& operator++();
			iterator operator++(int);
			value_type& operator*() const;
			value_type* operator->() const;
			
			explicit operator const_iterator() const;
			
			bool operator==(const iterator&) const;
			bool operator!=(const iterator&) const;
			bool operator==(const const_iterator&) const;
			bool operator!=(const const_iterator&) const;
			
			friend class const_iterator;
			friend class hash_table;
		};
		
		class const_iterator
		{
			protected:
			const _VNode* node;
			
			const_iterator(const _VNode*);
			
			public:
			
			const_iterator();
			const_iterator(const const_iterator&);
			explicit const_iterator(const iterator&);
			
			const_iterator& operator++();
			const_iterator operator++(int);
			const value_type& operator*() const;
			
			bool operator==(const const_iterator&) const;
			bool operator!=(const const_iterator&) const;
			bool operator==(const iterator&) const;
			bool operator!=(const iterator&) const;
			
			
			friend class iterator;
			friend class hash_table;
		};
		
		
		
		// Constructors / Destructors 
		
		hash_table(size_type bucketHint, const Selector&, const hasher& hash, const equals_type&, const allocator_type&);
		hash_table(const hash_table&);
		~hash_table();
		
		
		hash_table& operator=(const hash_table&);
		hash_table& operator=(hash_table&&);
		
		
		
		_VNode* get(const key_type&);
		const _VNode* get(const key_type&) const;
		_VNode* get(key_type&&);
		const _VNode* get(key_type&&) const;
		_VNode* getCreate(const key_type&, bool* created = nullptr);
		_VNode* getCreate(key_type&&, bool* created = nullptr);
		inline size_type size() const
		{
			return elemCount;
		}
		
		allocator_type get_allocator() const noexcept;
		
		void clear();
		inline void rehash(size_type n);
		void reserve(size_type n);
		inline size_type bucketsSize() const noexcept
		{
			return bucketCount;
		}
		inline size_type max_bucketsSize() const noexcept
		{
			return policy().max_buckets();
		}
		inline constexpr const Policy& policy() const noexcept;
		constexpr float max_load_factor() const noexcept;
		void max_load_factor(float);
		constexpr hasher hash_function() const noexcept;
		constexpr equals_type key_eq() const noexcept;
		size_type size_at(size_type) const;
		
		
		
		
		
		size_type bucketIndex(const key_type& k) const;
		size_type bucketIndex(const hash_code h) const;
		size_type bucketIndex(const _VNode*) const;
		bool equiv(const key_type&, const _VNode*) const;
		bool equiv(const key_type&, const key_type&) const;
		bool equiv(const _VNode*, const _VNode*) const;
		
		
		inline hash_code getCode(const key_type&) const;
		inline hash_code getCode(const _VNode*) const;
		
		
		
		
		
		// Iteration
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
		
		
	};*/
	
}

#endif