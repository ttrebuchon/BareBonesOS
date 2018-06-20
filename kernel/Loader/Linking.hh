#ifndef INCLUDED_KERNEL_LOADER_LINKING_HH
#define INCLUDED_KERNEL_LOADER_LINKING_HH

#include <Common.h>
#include <Utils/list>
#include "ELF.h"

namespace Kernel
{

	struct LinkerCallbackArg_t;

	class ELFSymbols
	{
		public:
		#if __P_ARCH__ == 32
		typedef ELFObjectBase<ELF32Header_t> object_type;

		#elif __P_ARCH__ == 64
		typedef ELFObjectBase<ELF64Header_t> object_type;

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
		void* resolve_symbol(LinkerCallbackArg_t*, addr_t index);
		
	};


	extern "C" ELFSymbols* ELF_kernel_symbols;

	void init_kernel_symbols();
	
	extern "C"
	void* ELF32_unpack(const uint8_t* buffer, size_t length, const void* sections, size_t sections_count, const void* program_headers, size_t program_headers_count, int section_names_index);

	ELF32Object* ELF32_unpack_parse(const uint8_t* buffer, size_t length, const void* sections, size_t sections_count, const void* program_headers, size_t program_headers_count, int section_names_index);
}



#endif