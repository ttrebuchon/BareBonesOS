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
			
		};
		
		template <class T, class Code>
		struct hash_node : public hash_node_base
		{
			T value;
			Code code;
		};
	}
	
	template <class Key, class T, class Selector, class Hash = hash<Key>, class Equals = equal_to<T>, class Alloc = Allocator<T>, class Policy = detail::hash_table_policy>
	class hash_table : protected Policy
	{
		public:
		typedef Key key_type;
		typedef T value_type;
		typedef Hash hasher;
		typedef Equals equals_type;
		typedef Alloc allocator_type;
		typedef /*size_t*/int size_type;
		
		
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
		
		Bucket* buckets;
		detail::hash_node_base* pre;
		size_type bucketCount;
		size_type elemCount;
		detail::hash_node_base beforeBegin;
		
		
		// Protected Methods
		
		_VNode* findNode(const size_type bucketIndex, const Key&, const hash_code);
		const _VNode* findNode(const size_type bucketIndex, const Key&, const hash_code) const;
		detail::hash_node_base* findBeforeNode(const size_type bucketIndex, const Key&, const hash_code);
		_VNode* allocateNode(const Key&) const;
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
		
		
		
		_VNode* get(const Key&);
		const _VNode* get(const Key&) const;
		_VNode* get(Key&&);
		const _VNode* get(Key&&) const;
		_VNode* get(const T&);
		_VNode* getCreate(const Key&);
		_VNode* getCreate(const T&);
		_VNode* getCreate(Key&&);
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
		
		
		
		
		
		size_type bucketIndex(const T& t) const;
		size_type bucketIndex(const Key& k) const;
		size_type bucketIndex(const hash_code h) const;
		size_type bucketIndex(const _VNode*) const;
		bool equiv(const Key&, const _VNode*) const;
		bool equiv(const Key&, const Key&) const;
		bool equiv(const Key&, const T&) const;
		inline bool equiv(const T& l, const _VNode* r) const
		{
			return equiv(select(l), r);
		}
		inline bool equiv(const T& l, const Key& r) const
		{
			return equiv(select(l), r);
		}
		inline bool equiv(const T& l, const T& r) const
		{
			return equiv(select(l), r);
		}
		bool equiv(const _VNode*, const _VNode*) const;
		
		
		inline hash_code getCode(const T&) const;
		inline hash_code getCode(const Key&) const;
		inline hash_code getCode(const _VNode*) const;
		
		
		
		
		
		// Iteration
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
		
		
	};
	
}

#endif