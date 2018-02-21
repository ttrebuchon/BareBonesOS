#ifndef INCLUDED_METAINFO_HH
#define INCLUDED_METAINFO_HH

namespace MetaInfo {
	
	class Printer
	{
		protected:
		typedef void(*__Func)(void*, const char*);
		void* ent;
		__Func printer;
		
		public:
		
		void print(const char* cstr) const
		{
			(*printer)(ent, cstr);
		}
	};
	
	template <class C, class R, class T = C>
	void printClassPrinter(void*, const char*);
	
	template <class C, class R, class T = C>
	class ClassPrinter : public Printer
	{
		private:
		
		protected:
		
		public:
		typedef R(C::*PrintFunc)(const char*);
		T obj;
		PrintFunc func;
		
		ClassPrinter(T obj, PrintFunc f) : obj(obj), func(f)
		{
			this->ent = this;
			this->printer = printClassPrinter<C, R, T>;
		}
		
	};
	
	template <class C, class R, class T>
	void printClassPrinter(void* obj, const char* cstr)
	{
		auto p = ((ClassPrinter<C, R, T>*)obj);
		(p->obj.*p->func)(cstr);
	}
	
	
	long registerPrinter(Printer*);
	Printer* unregisterPrinter(long);
}

#endif