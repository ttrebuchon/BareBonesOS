#ifndef INCLUDED_ELF_HPP
#define INCLUDED_ELF_HPP

#include "ELF.h"

namespace Kernel
{
	template <class HeaderType>
	void ELFObject<HeaderType>::__build_tables()
	{
		auto section_header_table = this->section_header_table();
		assert(section_header_table);
		for (int i = 1; i < __header->section_header_count; ++i)
		{
			if (section_header_table[i].type == ELF_SEC_SYMTABLE || section_header_table[i].type == ELF_SEC_DYN_SYMTABLE)
			{
				++__symbol_table_count;
			}
			else if (section_header_table[i].type == ELF_SEC_RELOC_NADDEND || section_header_table[i].type == ELF_SEC_RELOC_ADDEND)
			{
				++__relocation_table_count;
			}
		}
		
		this->__symbol_tables = new symbol_table_type*[__symbol_table_count];
		for (int i = 1, j = 0; i < __header->section_header_count && j < __symbol_table_count; ++i)
		{
			if (section_header_table[i].type == ELF_SEC_SYMTABLE || section_header_table[i].type == ELF_SEC_DYN_SYMTABLE)
			{
				__symbol_tables[j++] = new symbol_table_type(this, &section_header_table[i]);
			}
		}

		this->__relocation_tables = new relocation_table_type*[__relocation_table_count];
		for (int i = 1, j = 0; i < __header->section_header_count && j < __relocation_table_count; ++i)
		{
			if (section_header_table[i].type == ELF_SEC_RELOC_NADDEND || section_header_table[i].type == ELF_SEC_RELOC_ADDEND)
			{
				__relocation_tables[j++] = new relocation_table_type(this, &section_header_table[i]);
			}
		}
	}


	template <class HeaderType>
	const typename ELFObject<HeaderType>::symbol_table_type* ELFObject<HeaderType>::symbol_table(const char* name) const noexcept
	{
		assert(name);
		for (int i = 0; i < symbol_table_count(); ++i)
		{
			auto tbl = this->symbol_table(i);
			assert(tbl);

			auto tbl_name = tbl->table_name();
			if (tbl_name)
			{
				if (strcmp(tbl_name, name) == 0)
				{
					return tbl;
				}
			}
		}
		return nullptr;
	}

	template <class HeaderType>
	const typename ELFObject<HeaderType>::symbol_table_type* ELFObject<HeaderType>::symbol_table(const section_header_type* sec) const noexcept
	{
		assert(sec);

		for (int i = 0; i < symbol_table_count(); ++i)
		{
			if (symbol_table(i)->get() == sec)
			{
				return symbol_table(i);
			}
		}

		return nullptr;
	}

	template <class HeaderType>
	const typename ELFObject<HeaderType>::relocation_table_type* ELFObject<HeaderType>::relocation_table(const char* name) const noexcept
	{
		assert(name);
		for (int i = 0; i < relocation_table_count(); ++i)
		{
			auto tbl = this->relocation_table(i);
			assert(tbl);

			auto tbl_name = tbl->table_name();
			if (tbl_name)
			{
				if (strcmp(tbl_name, name) == 0)
				{
					return tbl;
				}
			}
		}
		return nullptr;
	}


	template <class HeaderType>
	const typename ELFObject<HeaderType>::symbol_type* ELFObject<HeaderType>::symbol(const char* name) const noexcept
	{
		assert(name);
		for (int i = 0; i < symbol_table_count(); ++i)
		{
			auto sym_tbl = this->symbol_table(i);
			assert(sym_tbl);

			auto sym = sym_tbl->symbol(name);
			if (sym)
			{
				return sym;
			}
		}
		return nullptr;
	}








	template <class HeaderType>
	void ELFSymbolTable<HeaderType>::__build_symbols()
	{
		assert(symbol_count() > 1);
		__symbols = new symbol_type*[symbol_count() - 1];

		for (int i = 1; i < symbol_count(); ++i)
		{
			__symbols[i-1] = new symbol_type(__object, this, raw_symbol(i));
		}
	}


	template <class HeaderType>
	const typename ELFSymbolTable<HeaderType>::symbol_type* ELFSymbolTable<HeaderType>::symbol(const char* name) const noexcept
	{
		assert(name);

		for (int i = 1; i < this->symbol_count(); ++i)
		{
			auto sym = this->symbol(i);
			assert(sym);
			auto sym_name = sym->name();
			if (sym_name)
			{
				if (strcmp(sym_name, name) == 0)
				{
					return sym;
				}
			}
		}
		return nullptr;
	}






	template <class HeaderType>
	void ELFRelocationTable<HeaderType>::__build_relocs()
	{
		__relocs = new relocation_type*[count()];

		if (is_addend())
		{
			for (int i = 1; i < count(); ++i)
			{
				__relocs[i-1] = new relocation_type(__object, this, &(reinterpret_cast<const raw_relocation_addend_type*>(((addr_t)__object->get()) + __section->offset)[i]));
			}
		}
		else
		{
			for (int i = 1; i < count(); ++i)
			{
				__relocs[i-1] = new relocation_type(__object, this, &(reinterpret_cast<const raw_relocation_type*>(((addr_t)__object->get()) + __section->offset)[i]));
			}
		}
	}

}


#endif
