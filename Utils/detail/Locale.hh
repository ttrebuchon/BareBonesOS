#ifndef INCLUDED_LOCALE_HH
#define INCLUDED_LOCALE_HH

#include <Common.h>
#include <Debug.h>
#include "../Iterator.hh"
#include <Utils/StringFwd.hh>

namespace Utils
{
	
	
	template <class T, class OutputIt = ostreambuf_iterator<T>>
	class num_put;
	
	class locale;
	class ios_base;
	
	template <class Facet>
	const Facet& use_facet(const locale&);
	template <class Facet>
	bool has_facet(const locale&);
	
	class locale
	{
		private:
		class Internal;
		
		protected:
		
		public:
		
		
		class id
		{
			private:
			static size_t counter;
			
			size_t _value;
			
			
			public:
			id() : _value(counter++)
			{}
			
			const size_t& value() const
			{ return _value; }
			
			id(const id&) = delete;
			
			id& operator=(const id&) = delete;
		};
		
		class facet
		{
			protected:
			mutable size_t refcounter;
			
			public:
			explicit facet(size_t refs = 0) : refcounter(refs + 1)
			{
				
			}
			
			
			facet(const facet&) = delete;
			
			virtual ~facet() = default;
			
			facet& operator=(const facet&) = delete;
			
			private:
			size_t _removeref() const
			{
				return --refcounter;
			}
			size_t _addref() const
			{
				return ++refcounter;
			}
			
			friend class Internal;
		};
		
		
		
		//Categories
		typedef int category;
		
		constexpr static category none = 0x0;
		constexpr static category collate = 0x1;
		constexpr static category ctype = 0x2;
		constexpr static category monetary = 0x4;
		constexpr static category numeric = 0x8;
		constexpr static category time = 0x10;
		constexpr static category messages = 0x20;
		constexpr static category all = collate | ctype | monetary | numeric | time | messages;
		
		//Static Functions
		
		static locale global(const locale& loc);
		static const locale& classic();
		
		
		
		
		//Constructors/Destructor
		locale();
		locale(const locale&);
		explicit locale(const char* name);
		explicit locale(const String& name);
		locale(const locale& other, const char* name, category cat);
		locale(const locale& other, const String& name, category cat);
		locale(const locale& other, const locale& second, category cat);
		template <class Facet>
		locale(const locale&, Facet* f);
		~locale();
		
		
		//Operators
		const locale& operator=(const locale&);
		bool operator==(const locale&) const;
		bool operator!=(const locale&) const;
		
		template <class charT, class Traits, class Alloc>
		bool operator()(const Basic_String<charT, Traits, Alloc>& s1, const Basic_String<charT, Traits, Alloc>& s2) const;
		
		
		
		template <class Facet>
		locale combine(const locale& other) const;
		
		//"*" is default/undetermimed
		String name() const;
		
		
		private:
		
		class Internal
		{
			public:
			size_t rcount;
			
			id** _indexes;
			const facet** _facets;
			size_t _facets_size;
			
			Internal() : rcount(1), _indexes(new id*[6]), _facets(new const facet*[6]), _facets_size(6)
			{
				for (int i = 0; i < 6; ++i)
				{
					_indexes[i] = nullptr;
					_facets[i] = nullptr;
				}
			}
			
			Internal(const Internal& o) : rcount(1), _indexes(new id*[o._facets_size]), _facets(new const facet*[o._facets_size]), _facets_size(o._facets_size)
			{
				for (size_t i = 0; i < _facets_size; ++i)
				{
					_indexes[i] = o._indexes[i];
					_facets[i] = o._facets[i];
				}
			}
			
			template <class F>
			Internal* addFacet(const F* f)
			{
				if (rcount > 1)
				{
					--rcount;
					return (new Internal(*this))->addFacet(f);
				}
				
				
				const auto index = F::id.value();
				if (_facets_size <= index)
				{
					
					auto tmpInd = new id*[_facets_size*2];
					auto tmpFac = new const facet*[_facets_size*2];
					for (size_t i = 0; i < _facets_size; ++i)
					{
						tmpInd[i] = _indexes[i];
						tmpFac[i] = _facets[i];
					}
					
					delete[] _indexes;
					_indexes = tmpInd;
					delete[] _facets;
					_facets = tmpFac;
					
					_facets_size *= 2;
					return addFacet(f);
				}
				if (_facets[index])
				{
					if (_facets[index]->_removeref() == 0)
					{
						delete _facets[index];
					}
				}
				_facets[index] = f;
				_indexes[index] = &F::id;
				return this;
			}
		};
		
		explicit locale(Internal*);
		
		Internal* imp;
		
		static locale* _global;
		static locale& getGlobal();
		static locale& setGlobal(const locale&);
		
		template <class Facet>
		friend const Facet& use_facet(const locale&);
		template <class Facet>
		friend bool has_facet(const locale&);
		
	};
	
	
	
	template <class T, class It>
	It __write(It s, const T* str, size_t len);
	template <class T, class It>
	It __write(It s, T str);
	
	
	
	
	//Locale Facet Support
	template <class Facet>
	const Facet& use_facet(const locale& loc)
	{
		const size_t i = Facet::id.value();
		const locale::facet** _facets = loc.imp->_facets;
		if (!(i < loc.imp->_facets_size && _facets[i]))
		{
			//TODO
		}
		return static_cast<const Facet&>(*_facets[i]);
	}
	
	template <class Facet>
	bool has_facet(const locale& loc)
	{
		const size_t i = Facet::id.value();
		const locale::facet** _facets = loc.imp->_facets;
		if (i < loc.imp->_facets_size)
		{
			
			return (loc.imp->_facets[i] != nullptr);
		}
		return false;
	}
}

#endif