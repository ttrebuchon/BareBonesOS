#ifndef INCLUDED_ELF_HPP
#define INCLUDED_ELF_HPP

#include "ELF.h"
#include <Std/string.h>

namespace Kernel
{

	template <class HeaderType>
	ELFObjectBase<HeaderType>::ELFObjectBase(void* section_base, void* program_base, uint16_t kind) :
		_program_headers(),
		_section_headers(),
		_symbol_tables(),
		_relocation_tables(),
		_sections_base_address(section_base),
		_program_base_address(program_base),
		_kind(kind),
		_symbols(),
		_relocations()
	{
		
	}

	template <class HeaderType>
	ELFObjectBase<HeaderType>::ELFObjectBase(const section_header_type* section_table, int sections_count, void* program_base, int section_name_index, uint16_t kind) :
		ELFObjectBase((void*)section_table, program_base, kind)
	{
		_section_headers.resize(sections_count - 1);

		auto sh_names = (const char*)(section_table[section_name_index].offset + (const uint8_t*)section_table);
		assert(sh_names);

		for (int i = 1; i < sections_count; ++i)
		{
			auto& head = section_table[i];
			_section_headers[i-1] = &head;

			if (head.type == ELF_SEC_SYMTABLE || head.type == ELF_SEC_DYN_SYMTABLE)
			{
				continue;
			}
			else if (head.type == ELF_SEC_RELOC_ADDEND || head.type == ELF_SEC_RELOC_NADDEND)
			{
				continue;
			}

			
			auto nsection = new section_type(head.name_string_index + sh_names, this, &head, head.offset + (const uint8_t*)section_table);
			this->_sections[i] = nsection;
			assert(this->_sections[i]);
		}

		TRACE("Non-Special Sections added...");

		for (int i = 1; i < sections_count; ++i)
		{
			if (!this->_sections[i])
			{
				auto& head = section_table[i];
				assert(head.name_string_index != 0);
				TRACE(head.name_string_index + sh_names);
				add_section(head.name_string_index + sh_names, i, &head, head.offset + (const uint8_t*)section_table);
				TRACE("Section added...");
			}
		}
	}

	template <class HeaderType>
	void ELFObjectBase<HeaderType>::add_symbol_table(const char* name, int index, const section_header_type* header, const void* section)
	{
		assert(header->type == ELF_SEC_SYMTABLE || header->type == ELF_SEC_DYN_SYMTABLE);

		assert(!this->_sections[index]);

		assert(this->_sections[header->link]);

		const char* name_table = (const char*)_sections[header->link]->section();
		assert(name_table);

		auto sec = new symbol_table_type(name, _program_base_address, this, header, section, name_table);

		_symbol_tables.push_back(sec);
		this->_sections[index] = sec;

		bool already_present = false;
		for (size_t i = 0; i < this->_section_headers.size(); ++i)
		{
			if (this->_section_headers.at(i) == header)
			{
				already_present = true;
				break;
			}
		}

		if (!already_present)
		{
			this->_section_headers.push_back(header);
		}

		

		auto sym_count = sec->symbol_count();
		for (size_t i = 0; i < sym_count; ++i)
		{
			auto sym = sec->symbol(i);
			assert(sym);

			auto sym_name = sym->name();
			assert(sym_name);

			if (strlen(sym_name) > 0 && !_symbols[sym_name])
			{
				_symbols[sym_name] = sym;
				assert(strcmp(_symbols[sym_name]->name(), sym_name) == 0);
			}

			// assert(_symbols.count(sym_name) == 0);
			// _symbols[sym_name] = sym;
		}
	}

	template <class HeaderType>
	void ELFObjectBase<HeaderType>::add_section(const char* name, int index, const section_header_type* header, const void* section)
	{
		assert(!this->_sections[index]);
		assert(name);
		assert(index >= 0);
		assert(header);
		assert(section);

		if (header->type == ELF_SEC_SYMTABLE || header->type == ELF_SEC_DYN_SYMTABLE)
		{
			this->add_symbol_table(name, index, header, section);
			return;
		}
		else if (header->type == ELF_SEC_RELOC_ADDEND || header->type == ELF_SEC_RELOC_NADDEND)
		{
			this->add_relocation_table(name, index, header, section);
			return;
		}


		auto nsection = new section_type(name, this, header, section);
		this->_sections[index] = nsection;
		this->_section_headers.push_back(header);
	}

	template <class HeaderType>
	void ELFObjectBase<HeaderType>::add_relocation_table(const char* name, int index, const section_header_type* header, const void* section)
	{
		assert(header->type == ELF_SEC_RELOC_ADDEND || header->type == ELF_SEC_RELOC_NADDEND);
		assert(!this->_sections[index]);
		assert(this->_sections[header->link]);

		auto sec = new relocation_table_type(name, _program_base_address, this, header, section);
		_relocation_tables.push_back(sec);
		this->_sections[index] = sec;

		bool already_present = false;
		for (size_t i = 0; i < this->_section_headers.size(); ++i)
		{
			if (this->_section_headers.at(i) == header)
			{
				already_present = true;
				break;
			}
		}

		if (!already_present)
		{
			this->_section_headers.push_back(header);
		}

		auto reloc_count = sec->count();
		for (size_t i = 0; i < reloc_count; ++i)
		{
			auto reloc = sec->reloc(i);
			assert(reloc);

			auto sym = reloc->symbol();
			if (sym)
			{
				auto sym_name = sym->name();
				assert(sym_name);

				assert(_relocations.count(sym_name) == 0);
				_relocations[sym_name] = reloc;
			}
		}
	}

	template <class HeaderType>
	size_t ELFObjectBase<HeaderType>::symbol_table_count() const noexcept
	{
		return _symbol_tables.size();
	}

	template <class HeaderType>
	size_t ELFObjectBase<HeaderType>::relocation_table_count() const noexcept
	{
		return _relocation_tables.size();
	}

	template <class HeaderType>
	size_t ELFObjectBase<HeaderType>::section_count() const noexcept
	{
		assert(_sections.size() == _section_headers.size());
		return _sections.size();
	}


	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::symbol_table_type* ELFObjectBase<HeaderType>::symbol_table(const size_t i) const noexcept
	{
		return _symbol_tables.at(i);
	}

	template <class HeaderType>
	typename ELFObjectBase<HeaderType>::symbol_table_type* ELFObjectBase<HeaderType>::symbol_table(const size_t i) noexcept
	{
		return _symbol_tables.at(i);
	}

	
	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::symbol_type* ELFObjectBase<HeaderType>::symbol(const char* name) const noexcept
	{
		if (_symbols.count(name))
		{
			Utils::string _name = name;
			Utils::string _other = _symbols.at(_name)->name();

			assert(_name == _other);
			assert(_symbols.at(name)->name() == _name);
			return _symbols.at(_name);
		}
		else
		{
			return nullptr;
		}
	}
	
	template <class HeaderType>
	typename ELFObjectBase<HeaderType>::symbol_type* ELFObjectBase<HeaderType>::symbol(const char* name) noexcept
	{
		if (_symbols.count(name))
		{
			if (strcmp(_symbols.at(name)->name(), name))
			{
				auto sym = _symbols.at(name);
				TRACE(sym->name());
				TRACE(name);

				bool found = false;

				for (auto& pair : _symbols)
				{
					if (pair.second == sym)
					{
						if (&pair.second == &_symbols.at(name))
						{
							found = true;
							assert(pair.first == name);
							TRACE("----");
							TRACE(pair.first);
							TRACE(name);
							Utils::string name1(pair.first);
							Utils::string name2(name);
							TRACE("----");
							assert(name1 == name2);
							TRACE(name1);
							TRACE(name2);
							if (name1.length() != name2.length())
							{
								assert(name1.compare(name2) != 0);
								assert(strcmp(name1.c_str(), name2.c_str()) != 0);
							}
							assert(name1.length() == name2.length());
							break;
						}
					}
				}
				assert(found);
			}
			assert(strcmp(_symbols.at(name)->name(), name) == 0);
		}
		else
		{
			assert(_symbols[name] == nullptr);
		}
		return _symbols[name];
	}





	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::relocation_table_type* ELFObjectBase<HeaderType>::relocation_table(const size_t i) const noexcept
	{
		return _relocation_tables.at(i);
	}

	template <class HeaderType>
	typename ELFObjectBase<HeaderType>::relocation_table_type* ELFObjectBase<HeaderType>::relocation_table(const size_t i) noexcept
	{
		return _relocation_tables.at(i);
	}


	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::relocation_type* ELFObjectBase<HeaderType>::relocation(const char* name) const noexcept
	{
		if (_relocations.count(name))
		{
			return _relocations.at(name);
		}
		else
		{
			return nullptr;
		}
	}
	
	template <class HeaderType>
	typename ELFObjectBase<HeaderType>::relocation_type* ELFObjectBase<HeaderType>::relocation(const char* name) noexcept
	{
		return _relocations[name];
	}


	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::section_type* ELFObjectBase<HeaderType>::section(const size_t i) const noexcept
	{
		if (_sections.count(i))
		{
			return _sections.at(i);
		}
		else
		{
			return nullptr;
		}
		
	}

	template <class HeaderType>
	typename ELFObjectBase<HeaderType>::section_type* ELFObjectBase<HeaderType>::section(const size_t i) noexcept
	{
		return _sections[i];
	}

	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::section_type* ELFObjectBase<HeaderType>::section(const char* name) const noexcept
	{
		if (!name)
		{
			return section("");
		}

		for (const auto& pair : _sections)
		{
			auto sec = pair.second;
			assert(sec);
			if (strcmp(sec->name(), name) == 0)
			{
				return sec;
			}
		}
		return nullptr;
	}

	template <class HeaderType>
	typename ELFObjectBase<HeaderType>::section_type* ELFObjectBase<HeaderType>::section(const char* name) noexcept
	{
		if (!name)
		{
			return section("");
		}

		for (auto& pair : _sections)
		{
			auto sec = pair.second;
			assert(sec);
			if (strcmp(sec->name(), name) == 0)
			{
				return sec;
			}
		}
		return nullptr;
	}


	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::section_header_type* ELFObjectBase<HeaderType>::section_header(const size_t i) const noexcept
	{
		assert(this->section(i + 1));
		assert(this->section(i + 1)->header() == _section_headers.at(i));
		return _section_headers.at(i);
	}

	// template <class HeaderType>
	// typename ELFObjectBase<HeaderType>::section_header_type* ELFObjectBase<HeaderType>::section_header(const size_t i) noexcept
	// {
	// 	assert(this->section(i));
	// 	assert(this->section(i)->header() == _section_headers.at(i));
	// 	return _section_headers.at(i);
	// }


	// template <class HeaderType>
	// typename ELFObjectBase<HeaderType>::section_header_type* ELFObjectBase<HeaderType>::section_header(const char* name) noexcept
	// {
	// 	return this->section(name)->header();
	// }

	template <class HeaderType>
	const typename ELFObjectBase<HeaderType>::section_header_type* ELFObjectBase<HeaderType>::section_header(const char* name) const noexcept
	{
		return this->section(name)->header();
	}




	// #ifdef DEBUG
	template <class HeaderType>
	void ELFObjectBase<HeaderType>::verify_integrity()
	{
		TRACE("Verifying...");
		for (const auto& sym : this->_symbols)
		{
			if (!sym.second)
			{
				TRACE("Null symbol: ");
				TRACE(sym.first);
				continue;
			}

			if (!sym.second->name())
			{
				TRACE("No Name");
				continue;
			}

			assert(sym.first == sym.second->name());
			assert(strlen(sym.second->name()) == sym.first.length());
			// TRACE("Verified: ");
			// TRACE(sym.first);
			// TRACE(sym.second->name());
			// TRACE("-----");
		}
		if (this->_symbols.size() == 0)
		{
			TRACE("No symbols to verify.");
		}

		TRACE("Verified");
	}
	// #endif












	template <class HeaderType>
	ELFSection<HeaderType>::ELFSection(const char* name, ELFObjectBase<header_type>* parent, const section_header_type* header, const void* section) :
		__name(name),
		__object(parent),
		__section_header(header),
		__section(section)
	{
		assert(name);
		assert(parent);
		assert(header);
		assert(section);
	}












	template <class HeaderType>
	ELFSymbolTable<HeaderType>::ELFSymbolTable(
						const char* name,
						const void* value_base,
						ELFObjectBase<HeaderType>* parent,
						const section_header_type* header,
						const void* table,
						const char* symbol_names_table
					)
					:
					ELFSection<HeaderType>(name, parent, header, table),
					value_base_address(value_base),
					symbol_names_table(symbol_names_table),
					_symbols()
	{
		int count = (this->__section_header->size / this->__section_header->entry_size);
		assert(count > 0);
		assert(symbol_names_table);

		const raw_symbol_type* typed_table = reinterpret_cast<const raw_symbol_type*>(table);

		for (int i = 0; i < count; ++i)
		{
			const raw_symbol_type& raw = typed_table[i];
			const char* sname = symbol_names_table + raw.name_string_index;
			if (raw.name_string_index == 0)
			{
				sname = "";
			}
			auto sym = new symbol_type(sname, i, this, const_cast<raw_symbol_type*>(typed_table + i));
			this->_symbols.push_back(sym);

		}
	}


	template <class HeaderType>
	size_t ELFSymbolTable<HeaderType>::count() const noexcept
	{
		return _symbols.size();
	}


	template <class HeaderType>
	const typename ELFSymbolTable<HeaderType>::symbol_type* ELFSymbolTable<HeaderType>::symbol(const size_t i) const noexcept
	{
		return _symbols.at(i);
	}

	template <class HeaderType>
	typename ELFSymbolTable<HeaderType>::symbol_type* ELFSymbolTable<HeaderType>::symbol(const size_t i) noexcept
	{
		return _symbols.at(i);
	}


	template <class HeaderType>
	const typename ELFSymbolTable<HeaderType>::symbol_type* ELFSymbolTable<HeaderType>::symbol(const char* name) const noexcept
	{
		if (!name)
		{
			return this->symbol("");
		}
		for (size_t i = 0; i < this->count(); ++i)
		{
			if (strcmp(_symbols.at(i)->name(), name) == 0)
			{
				return _symbols.at(i);
			}
		}
		return nullptr;
	}

	template <class HeaderType>
	typename ELFSymbolTable<HeaderType>::symbol_type* ELFSymbolTable<HeaderType>::symbol(const char* name) noexcept
	{
		if (!name)
		{
			return this->symbol("");
		}
		for (size_t i = 0; i < this->count(); ++i)
		{
			if (strcmp(_symbols.at(i)->name(), name) == 0)
			{
				return _symbols.at(i);
			}
		}
		return nullptr;
	}








	template <class HeaderType>
	ELFSymbol<HeaderType>::ELFSymbol(const char* name, int index, symbol_table_type* parent, raw_symbol_type* raw) : _name(name), index(index), _table(parent), _raw_symbol(raw)
	{
		assert(name);
		assert(index >= 0);
		assert(parent);
		assert(raw);
	}


	template <class HeaderType>
	typename ELFSymbol<HeaderType>::address_type ELFSymbol<HeaderType>::value() const noexcept
	{
		if (_raw_symbol->shndx == 0x0)
		{
			// External symbol
			assert(this->_table->header()->link > 0);
			auto s = _table->parent()->section_header(_table->header()->link - 1);
			assert(s);
			const char* name = this->name();
			assert(name);

			//extern void* ELF_lookup_symbol(const char* name);
			void* target = nullptr;//ELF_lookup_symbol(name);

			if (!target)
			{
				if (this->binding() & ELF_SYM_WEAK)
				{
					return 0;
				}
				else
				{
					// DEBUG
					//assert(NOT_IMPLEMENTED);
					return 0;
					
				}
			}
			else
			{
				return (addr_t)target;
			}
		}
		else if (_raw_symbol->shndx == ELF_SHN_ABS)
		{
			return _raw_symbol->value;
		}
		else
		{
			auto obj = _table->parent();

			auto target = obj->section_header(_raw_symbol->shndx - 1);
			assert(target);

			static_assert(!(bool)NOT_IMPLEMENTED);

			switch (obj->kind())
			{
				case ELF_KIND_UNKNOWN:
					assert(NOT_IMPLEMENTED);
					
				
				case ELF_KIND_RELOC:
					return (addr_t)_table->value_base_address + target->offset + _raw_symbol->value;
				
				case ELF_KIND_SHARED:
				case ELF_KIND_EXEC:
					return (addr_t)_table->value_base_address + _raw_symbol->value;

				case ELF_KIND_CORE:
					assert(NOT_IMPLEMENTED);

				default:
					assert(NOT_IMPLEMENTED);
			}
		}
	}








	template <class HeaderType>
	ELFRelocationTable<HeaderType>::ELFRelocationTable(
							const char* name,
							const void* value_base,
							ELFObjectBase<HeaderType>* parent,
							const section_header_type* section_header,
							const void* table
						)
						:
						ELFSection<HeaderType>(name, parent, section_header, table),
						value_base_address(value_base),
						_relocs()

	{
		int count = (this->__section_header->size / this->__section_header->entry_size);
		assert(count > 0);

		const raw_relocation_type* typed_table = reinterpret_cast<const raw_relocation_type*>(table);

		for (int i = 0; i < count; ++i)
		{
			auto reloc = new relocation_type(i, this, const_cast<raw_relocation_type*>(typed_table + i));
			this->_relocs.push_back(reloc);
		}
	}

	template <class HeaderType>
	typename ELFRelocationTable<HeaderType>::relocation_type* ELFRelocationTable<HeaderType>::reloc(const size_t i) noexcept
	{
		return _relocs.at(i);
	}

	template <class HeaderType>
	const typename ELFRelocationTable<HeaderType>::relocation_type* ELFRelocationTable<HeaderType>::reloc(const size_t i) const noexcept
	{
		return _relocs.at(i);
	}











	template <class HeaderType>
	ELFReloc<HeaderType>::ELFReloc(int index, relocation_table_type* parent, raw_relocation_type* raw) : index(index), _table(parent), _raw_reloc(raw)
	{
		assert(index >= 0);
		assert(parent);
		assert(raw);
	}

	template <class HeaderType>
	typename ELFReloc<HeaderType>::address_type* ELFReloc<HeaderType>::target() noexcept
	{
		return const_cast<typename ELFReloc<HeaderType>::address_type*>(((const ELFReloc<HeaderType>*)this)->target());
	}

	template <class HeaderType>
	const typename ELFReloc<HeaderType>::address_type* ELFReloc<HeaderType>::target() const noexcept
	{
		auto obj = _table->parent();
		if (_table->has_info() != 0)
		{
			auto target = obj->section_header(_table->info());
			assert(target);
			// return (void*)((addr_t)__object->get() + _raw_reloc->offset + target->offset - target->address);
			return (address_type*)((addr_t)_table->value_base_address + _raw_reloc->offset);
		}
		assert(_raw_reloc->offset > 0);
		// assert(_table->info() > 0);
		// auto target = obj->section_header(_table->info());
		// assert(target);

		address_type* ref = (address_type*)((addr_t)_table->value_base_address + _raw_reloc->offset);
		
		return ref;
	}

	template <class HeaderType>
	const typename ELFReloc<HeaderType>::symbol_type* ELFReloc<HeaderType>::symbol() const noexcept
	{
		if (info_symbol() >= 0)
		{
			auto l = _table->link();
			assert(l >= 0);
			auto sec = _table->parent()->section(l);
			assert(sec);
			assert(sec->header()->type == ELF_SEC_SYMTABLE || sec->header()->type == ELF_SEC_DYN_SYMTABLE);
			auto sym_table = (const symbol_table_type*)sec;
			assert(sym_table);

			return sym_table->symbol(info_symbol());
		}
		return nullptr;
	}

	template <class HeaderType>
	typename ELFReloc<HeaderType>::symbol_type* ELFReloc<HeaderType>::symbol() noexcept
	{
		if (info_symbol() >= 0)
		{
			auto l = _table->link();
			assert(l >= 0);
			auto sec = _table->parent()->section(l);
			assert(sec);
			if (!(sec->header()->type == ELF_SEC_SYMTABLE || sec->header()->type == ELF_SEC_DYN_SYMTABLE))
			{
				TRACE(sec->header()->type);
			}
			assert(sec->header()->type == ELF_SEC_SYMTABLE || sec->header()->type == ELF_SEC_DYN_SYMTABLE);
			auto sym_table = (symbol_table_type*)sec;
			assert(sym_table);

			return sym_table->symbol(info_symbol());
		}
		return nullptr;
	}

	template <class HeaderType>
	typename ELFReloc<HeaderType>::sword_type ELFReloc<HeaderType>::info_symbol() const noexcept
	{
		// The bitshifting process for 64-bit may be different, need to check that before using this for ELF64
		static_assert(detail::elf_entry_type_chooser<header_type>::is_32);
		
		return ((sword_type)(_raw_reloc->info >> 8))/* - 1*/;
	}

	template <class HeaderType>
	typename ELFReloc<HeaderType>::word_type ELFReloc<HeaderType>::info_type() const noexcept
	{
		// The bitshifting process for 64-bit may be different, need to check that before using this for ELF64
		static_assert(detail::elf_entry_type_chooser<header_type>::is_32);

	
		return _raw_reloc->info & 0xFF; 
	}

	template <class HeaderType>
	void* ELFReloc<HeaderType>::reloc_value() const noexcept
	{
		auto obj = _table->parent();
		word_type* ref = (word_type*)this->target();

		void* symval = nullptr;

		if (info_symbol() != 0)
		{
			auto sym = this->symbol();
			if (!sym)
			{
				return nullptr;
			}

			symval = (void*)sym->value();
		}
		else
		{
			assert(NOT_IMPLEMENTED);
		}

		switch (info_type())
		{
			case ELF_RELOC_386_NONE:
				assert(NOT_IMPLEMENTED);
				break;
			case ELF_RELOC_386_32:
				// assert(NOT_IMPLEMENTED);
				return (void*)(*ref + (addr_t)symval);

			case ELF_RELOC_386_PC32:
				//assert(NOT_IMPLEMENTED);
				return (void*)(*ref + (addr_t)symval - /* --> */ (addr_t)ref /* <-- ????? I'm not sure about this at all. There's a chance the documentation made a mistake*/);

			case ELF_RELOC_386_JUMP_SLOT:
				assert(_table->has_info());
				{
					auto target = obj->section_header(_table->info());
					assert(target);
					if (false)//(symval == 0)
					{
						symval = (void*)_table->value_base_address;
						return (void*)(*(word_type*)((addr_t)_table->value_base_address + _raw_reloc->offset - target->address) + (addr_t)symval);
					}
					else
					{
						// TRACE("------");
						// TRACE((void*)(symbol()->value() - symbol()->raw_value() - target->address));
						// TRACE(_table->value_base_address);
						// TRACE("------");
						return (void*)(*ref + (addr_t)_table->value_base_address);
						return (void*)(*ref + this->symbol()->raw_value());
						return (void*)(*ref + (addr_t)symval);
					}
					
				}
				//assert(NOT_IMPLEMENTED);
				// return (const void*)(*ref + (addr_t)symval - /* --> */ (addr_t)ref /* <-- ????? I'm not sure about this at all. There's a chance the documentation made a mistake*/);

			case ELF_RELOC_386_GLOB_DAT:
				// TODO
				// return (void*)(*ref + (addr_t)symbol()->raw_value());
				return (void*)(*ref + (addr_t)symval);
				return nullptr;
			
			default:
				TRACE(info_type());
				return nullptr;
				assert(NOT_IMPLEMENTED);
		}
	}











	// template <class HeaderType>
	// void ELFObject<HeaderType>::__build_tables()
	// {
	// 	auto section_header_table = this->section_header_table();
	// 	assert(section_header_table);
	// 	for (int i = 1; i < __section_header_count; ++i)
	// 	{
	// 		if (section_header_table[i].type == ELF_SEC_SYMTABLE || section_header_table[i].type == ELF_SEC_DYN_SYMTABLE)
	// 		{
	// 			++__symbol_table_count;
	// 		}
	// 		else if (section_header_table[i].type == ELF_SEC_RELOC_NADDEND || section_header_table[i].type == ELF_SEC_RELOC_ADDEND)
	// 		{
	// 			++__relocation_table_count;
	// 		}
	// 	}
		
	// 	int j = 0;
	// 	this->__symbol_tables = new symbol_table_type*[__symbol_table_count];
	// 	for (int i = 1; i < __section_header_count && j < __symbol_table_count; ++i)
	// 	{
	// 		if (section_header_table[i].type == ELF_SEC_SYMTABLE || section_header_table[i].type == ELF_SEC_DYN_SYMTABLE)
	// 		{
	// 			__symbol_tables[j++] = new symbol_table_type(this, &section_header_table[i], i);
	// 		}
	// 	}
	// 	assert(j == __symbol_table_count);

	// 	this->__relocation_tables = new relocation_table_type*[__relocation_table_count];
	// 	j = 0;
	// 	for (int i = 1; i < __section_header_count && j < __relocation_table_count; ++i)
	// 	{
	// 		if (section_header_table[i].type == ELF_SEC_RELOC_NADDEND || section_header_table[i].type == ELF_SEC_RELOC_ADDEND)
	// 		{
	// 			__relocation_tables[j++] = new relocation_table_type(this, &section_header_table[i], i);
	// 		}
	// 	}
	// 	assert(j == __relocation_table_count);
	// }


	// template <class HeaderType>
	// const typename ELFObject<HeaderType>::symbol_table_type* ELFObject<HeaderType>::symbol_table(const char* name) const noexcept
	// {
	// 	assert(name);
	// 	for (int i = 0; i < symbol_table_count(); ++i)
	// 	{
	// 		auto tbl = this->symbol_table(i);
	// 		assert(tbl);

	// 		auto tbl_name = tbl->table_name();
	// 		if (tbl_name)
	// 		{
	// 			if (strcmp(tbl_name, name) == 0)
	// 			{
	// 				return tbl;
	// 			}
	// 		}
	// 	}
	// 	return nullptr;
	// }

	// template <class HeaderType>
	// const typename ELFObject<HeaderType>::symbol_table_type* ELFObject<HeaderType>::symbol_table(const section_header_type* sec) const noexcept
	// {
	// 	assert(sec);

	// 	for (int i = 0; i < symbol_table_count(); ++i)
	// 	{
	// 		if (&section_header_table()[symbol_table(i)->index() + 1] == sec)
	// 		{
	// 			return symbol_table(i);
	// 		}
	// 	}

	// 	return nullptr;
	// }

	// template <class HeaderType>
	// const typename ELFObject<HeaderType>::relocation_table_type* ELFObject<HeaderType>::relocation_table(const char* name) const noexcept
	// {
	// 	assert(name);
	// 	for (int i = 0; i < relocation_table_count(); ++i)
	// 	{
	// 		auto tbl = this->relocation_table(i);
	// 		assert(tbl);

	// 		auto tbl_name = tbl->table_name();
	// 		if (tbl_name)
	// 		{
	// 			if (strcmp(tbl_name, name) == 0)
	// 			{
	// 				return tbl;
	// 			}
	// 		}
	// 	}
	// 	return nullptr;
	// }


	// template <class HeaderType>
	// const typename ELFObject<HeaderType>::symbol_type* ELFObject<HeaderType>::symbol(const char* name) const noexcept
	// {
	// 	assert(name);
	// 	for (int i = 0; i < symbol_table_count(); ++i)
	// 	{
	// 		auto sym_tbl = this->symbol_table(i);
	// 		assert(sym_tbl);

	// 		auto sym = sym_tbl->symbol(name);
	// 		if (sym)
	// 		{
	// 			return sym;
	// 		}
	// 	}
	// 	return nullptr;
	// }

	// template <class HeaderType>
	// const typename ELFObject<HeaderType>::relocation_type* ELFObject<HeaderType>::reloc(const char* name, int skip) const noexcept
	// {
	// 	assert(name);

	// 	for (int i = 0; i < this->relocation_table_count(); ++i)
	// 	{
	// 		auto tbl = this->relocation_table(i);
	// 		assert(tbl);
	// 		for (int j = 0; j < tbl->count(); ++j)
	// 		{
	// 			auto r = tbl->reloc(j);
	// 			assert(r);
	// 			auto sym = r->symbol();
	// 			if (sym)
	// 			{
	// 				auto sname = sym->name();
	// 				if (sname)
	// 				{
	// 					if (strcmp(sname, name) == 0)
	// 					{
	// 						return r;
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// 	return nullptr;
	// }








	// template <class HeaderType>
	// void ELFSymbolTable<HeaderType>::__build_symbols()
	// {
	// 	assert(symbol_count() >= 1);
	// 	__symbols = new symbol_type*[symbol_count()];

	// 	for (int i = 0; i < symbol_count(); ++i)
	// 	{
	// 		__symbols[i] = new symbol_type(__object, this, raw_symbol(i));
	// 	}
	// }


	// template <class HeaderType>
	// const typename ELFSymbolTable<HeaderType>::symbol_type* ELFSymbolTable<HeaderType>::symbol(const char* name) const noexcept
	// {
	// 	assert(name);

	// 	for (int i = 0; i < this->symbol_count(); ++i)
	// 	{
	// 		auto sym = this->symbol(i);
	// 		assert(sym);
	// 		auto sym_name = sym->name();
	// 		if (sym_name)
	// 		{
	// 			if (strcmp(sym_name, name) == 0)
	// 			{
	// 				return sym;
	// 			}
	// 		}
	// 	}
	// 	return nullptr;
	// }






	// template <class HeaderType>
	// void ELFRelocationTable<HeaderType>::__build_relocs()
	// {
	// 	__relocs = new relocation_type*[count()];

	// 	if (is_addend())
	// 	{
	// 		for (int i = 0; i < count(); ++i)
	// 		{
	// 			__relocs[i] = new relocation_type(__object, this, &(reinterpret_cast<const raw_relocation_addend_type*>(((addr_t)__object->base_address()) + __section->offset)[i]));
	// 		}
	// 	}
	// 	else
	// 	{
	// 		for (int i = 0; i < count(); ++i)
	// 		{
	// 			__relocs[i] = new relocation_type(__object, this, &(reinterpret_cast<const raw_relocation_type*>(((addr_t)__object->base_address()) + __section->offset)[i]));
	// 		}
	// 	}
	// }

}


#endif
