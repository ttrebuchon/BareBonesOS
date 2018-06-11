#ifndef INCLUDED_KERNEL_LOADER_LINKING_HH
#define INCLUDED_KERNEL_LOADER_LINKING_HH

#include <Common.h>
#include <Utils/list>
#include "ELF.h"

namespace Kernel
{

	class ELFSymbols
	{
		public:
		#if __P_ARCH__ == 32
		typedef ELFObject<ELF32Header_t> object_type;

		#elif __P_ARCH__ == 64
		typedef ELFObject<ELF64Header_t> object_type;

		#else

		#error Unknown architecture

		#endif

		typedef typename object_type::symbol_table_type			symbol_table_type;
		typedef typename object_type::symbol_type				symbol_type;
		typedef typename object_type::relocation_table_type		relocation_table_type;
		typedef typename object_type::relocation_type			relocation_type;
		

		protected:
		Utils::list<object_type*> objects;
		Utils::list<symbol_table_type*> symbol_tables;

		public:


		void link_object(object_type*);
		
	};


	extern "C" ELFSymbols* ELF_kernel_symbols;
}



#endif