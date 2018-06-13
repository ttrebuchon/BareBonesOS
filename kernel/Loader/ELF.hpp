#ifndef INCLUDED_ELF_HPP
#define INCLUDED_ELF_HPP

#include "ELF.h"
#include <Std/string.h>

namespace Kernel
{
	template <class HeaderType>
	void ELFObject<HeaderType>::__build_tables()
	{
		auto section_header_table = this->section_header_table();
		assert(section_header_table);
		for (int i = 1; i < __section_header_count; ++i)
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
		
		int j = 0;
		this->__symbol_tables = new symbol_table_type*[__symbol_table_count];
		for (int i = 1; i < __section_header_count && j < __symbol_table_count; ++i)
		{
			if (section_header_table[i].type == ELF_SEC_SYMTABLE || section_header_table[i].type == ELF_SEC_DYN_SYMTABLE)
			{
				__symbol_tables[j++] = new symbol_table_type(this, &section_header_table[i], i);
			}
		}
		assert(j == __symbol_table_count);

		this->__relocation_tables = new relocation_table_type*[__relocation_table_count];
		j = 0;
		for (int i = 1; i < __section_header_count && j < __relocation_table_count; ++i)
		{
			if (section_header_table[i].type == ELF_SEC_RELOC_NADDEND || section_header_table[i].type == ELF_SEC_RELOC_ADDEND)
			{
				__relocation_tables[j++] = new relocation_table_type(this, &section_header_table[i], i);
			}
		}
		assert(j == __relocation_table_count);
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
			if (&section_header_table()[symbol_table(i)->index() + 1] == sec)
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
	const typename ELFObject<HeaderType>::relocation_type* ELFObject<HeaderType>::reloc(const char* name, int skip) const noexcept
	{
		assert(name);

		for (int i = 0; i < this->relocation_table_count(); ++i)
		{
			auto tbl = this->relocation_table(i);
			assert(tbl);
			for (int j = 0; j < tbl->count(); ++j)
			{
				auto r = tbl->reloc(j);
				assert(r);
				auto sym = r->symbol();
				if (sym)
				{
					auto sname = sym->name();
					if (sname)
					{
						if (strcmp(sname, name) == 0)
						{
							return r;
						}
					}
				}
			}
		}
		return nullptr;
	}








	template <class HeaderType>
	void ELFSymbolTable<HeaderType>::__build_symbols()
	{
		assert(symbol_count() >= 1);
		__symbols = new symbol_type*[symbol_count()];

		for (int i = 0; i < symbol_count(); ++i)
		{
			__symbols[i] = new symbol_type(__object, this, raw_symbol(i));
		}
	}


	template <class HeaderType>
	const typename ELFSymbolTable<HeaderType>::symbol_type* ELFSymbolTable<HeaderType>::symbol(const char* name) const noexcept
	{
		assert(name);

		for (int i = 0; i < this->symbol_count(); ++i)
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
			for (int i = 0; i < count(); ++i)
			{
				__relocs[i] = new relocation_type(__object, this, &(reinterpret_cast<const raw_relocation_addend_type*>(((addr_t)__object->base_address()) + __section->offset)[i]));
			}
		}
		else
		{
			for (int i = 0; i < count(); ++i)
			{
				__relocs[i] = new relocation_type(__object, this, &(reinterpret_cast<const raw_relocation_type*>(((addr_t)__object->base_address()) + __section->offset)[i]));
			}
		}
	}

}


#endif
