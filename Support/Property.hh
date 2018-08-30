#ifndef INCLUDED_SUPPORT_PROPERTY_HH
#define INCLUDED_SUPPORT_PROPERTY_HH

namespace Support
{
	
	template <class T, class Y>
	class gProperty
	{
		public:
		
		protected:
		typedef Y value_type;
		typedef value_type(T::*Fn_t)();
		typedef value_type(T::*cFn_t)() const;
		union
		{
			Fn_t func;
			cFn_t cfunc;
		};
		
		T* _this;
		
		public:
		
		gProperty(T* _this, Fn_t fn) : _this(_this), func(fn)
		{
			
		}
		
		gProperty(T* _this, cFn_t fn) : _this(_this), cfunc(fn)
		{
			
		}
		
		
		
		operator value_type() const
		{
			return (_this->*func)();
		}
		
		
		Fn_t operator&() const
		{
			return func;
		}
		
		
		
	};
	
	
	template <class T, class Y>
	class gProperty<const T, Y>
	{
		public:
		
		protected:
		typedef Y value_type;
		typedef value_type(T::*Fn_t)();
		typedef value_type(T::*cFn_t)() const;
		union
		{
			Fn_t func;
			cFn_t cfunc;
		};
		
		const T* _this;
		
		public:
		
		gProperty(const T* _this, cFn_t fn) : _this(_this), cfunc(fn)
		{
			
		}
		
		
		
		operator value_type() const
		{
			return (_this->*cfunc)();
		}
		
		Fn_t operator&() const
		{
			return func;
		}
		
	};
	
	
	
	
	
	
	
	template <class T, class Y>
	class gProperty<T, Y&>
	{
		public:
		
		protected:
		typedef Y& value_type;
		typedef value_type(T::*Fn_t)();
		typedef value_type(T::*cFn_t)() const;
		union
		{
			Fn_t func;
			cFn_t cfunc;
		};
		
		T* _this;
		
		public:
		
		gProperty(T* _this, Fn_t fn) : _this(_this), func(fn)
		{
			
		}
		
		gProperty(T* _this, cFn_t fn) : _this(_this), cfunc(fn)
		{
			
		}
		
		
		
		operator value_type() const
		{
			return (_this->*func)();
		}
		
		
		Y* operator&() const
		{
			return &((_this->*func)());
		}
		
		
		
	};
	
	
	template <class T, class Y>
	class gProperty<const T, Y&>
	{
		public:
		
		protected:
		typedef Y& value_type;
		typedef value_type(T::*Fn_t)();
		typedef value_type(T::*cFn_t)() const;
		union
		{
			Fn_t func;
			cFn_t cfunc;
		};
		
		const T* _this;
		
		public:
		
		gProperty(const T* _this, cFn_t fn) : _this(_this), cfunc(fn)
		{
			
		}
		
		
		
		operator value_type() const
		{
			return (_this->*cfunc)();
		}
		
		Y* operator&() const
		{
			return &((_this->*func)());
		}
		
	};
	
	
	
	template <class T, class Y>
	class gProperty<T, const Y&>
	{
		public:
		
		protected:
		typedef const Y& value_type;
		typedef const Y* ptr_type;
		typedef value_type(T::*Fn_t)();
		typedef value_type(T::*cFn_t)() const;
		union
		{
			Fn_t func;
			cFn_t cfunc;
		};
		
		T* _this;
		
		public:
		
		gProperty(T* _this, Fn_t fn) : _this(_this), func(fn)
		{
			
		}
		
		gProperty(T* _this, cFn_t fn) : _this(_this), cfunc(fn)
		{
			
		}
		
		
		
		operator value_type() const
		{
			return (_this->*cfunc)();
		}
		
		
		ptr_type operator&() const
		{
			return &((_this->*func)());
		}
		
		
		
	};
	
	
	template <class T, class Y>
	class gProperty<const T, const Y&>
	{
		public:
		
		protected:
		typedef const Y& value_type;
		typedef const Y* ptr_type;
		
		typedef value_type(T::*Fn_t)();
		typedef value_type(T::*cFn_t)() const;
		union
		{
			Fn_t func;
			cFn_t cfunc;
		};
		
		const T* _this;
		
		public:
		
		gProperty(const T* _this, cFn_t fn) : _this(_this), cfunc(fn)
		{
			
		}
		
		
		
		operator value_type() const
		{
			return (_this->*cfunc)();
		}
		
		ptr_type operator&() const
		{
			return &((_this->*cfunc)());
		}
		
	};
	
	
}

#endif