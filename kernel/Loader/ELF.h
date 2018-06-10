#ifndef INCLUDED_ELF_H
#define INCLUDED_ELF_H

#include <Common.h>

START_NS(Kernel)
C_CODE

typedef struct
{
	uint8_t magic[4];   // { 0x7F, 'E', 'L', 'F' }
	uint8_t bitness;    // 1 = 32-bit, 2 = 64-bit
	uint8_t endianess;  // 1 = Little, 2 = Big
	uint8_t ELF_version;
	uint8_t OS_ABI;
	uint8_t unused[8];

	uint16_t kind;      // 1 = Relocatable, 2 = Executable, 3 = Shared, 4 = Core
	uint16_t instruction_set;
	uint32_t ELF_version2;

	uint32_t entry_position;
	uint32_t program_header_offset;
	uint32_t section_header_offset;
	uint32_t flags;
	uint16_t header_size;
	uint16_t program_header_entry_size;
	uint16_t program_header_count;
	uint16_t section_header_entry_size;
	uint16_t section_header_count;
	uint16_t section_header_strings_index;


} __attribute__((__packed__))
ELF32Header_t;


typedef struct
{
	uint8_t magic[4];   // { 0x7F, 'E', 'L', 'F' }
	uint8_t bitness;    // 1 = 32-bit, 2 = 64-bit
	uint8_t endianess;  // 1 = Little, 2 = Big
	uint8_t ELF_version;
	uint8_t OS_ABI;
	uint8_t unused[8];

	uint16_t kind;      // 1 = Relocatable, 2 = Executable, 3 = Shared, 4 = Core
	uint16_t instruction_set;
	uint32_t ELF_version2;

	uint64_t entry_position;
	uint64_t program_header_offset;
	uint64_t section_header_offset;
	uint32_t flags;
	uint16_t header_size;
	uint16_t program_header_entry_size;
	uint16_t program_header_count;
	uint16_t section_header_entry_size;
	uint16_t section_header_count;
	uint16_t section_header_strings_index;


} __attribute__((__packed__))
ELF64Header_t;

#define ELF_KIND_UNKNOWN    0x00
#define ELF_KIND_RELOC      0x01
#define ELF_KIND_EXEC       0x02
#define ELF_KIND_SHARED     0x03
#define ELF_KIND_CORE       0x04


#define ELF_ARCH_X86        0x03



typedef struct
{

	uint32_t type;
	uint32_t file_offset;
	uint32_t virtual_addr;
	uint32_t undefined;
	uint32_t file_segment_size;
	uint32_t memory_segment_size;
	uint32_t flags;
	uint32_t required_alignment;

} __attribute__((__packed__))
ELF32ProgramEntry_t;

typedef struct
{

	uint32_t type;
	uint32_t flags;
	uint64_t file_offset;
	uint64_t virtual_addr;
	uint64_t undefined;
	uint64_t file_segment_size;
	uint64_t memory_segment_size;
	uint64_t required_alignment;

} __attribute__((__packed__))
ELF64ProgramEntry_t;


#define ELF_SEG_NULL    0x0
#define ELF_SEG_LOAD    0x1
#define ELF_SEG_DYN     0x2
#define ELF_SEG_INTERP  0x3
#define ELF_SEG_NOTE    0x4

#define ELF_FLAG_EXEC   0x1
#define ELF_FLAG_WRITE  0x2
#define ELF_FLAG_READ   0x4


typedef struct
{

	uint32_t name_string_index;
	uint32_t type;
	uint32_t flags;
	uint32_t address;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t address_align;
	uint32_t entry_size;

} __attribute__((__packed__))
ELF32SectionHeader_t;

typedef struct
{


	// TODO: NO IDEA IF THIS IS CORRECT (PROBABLY NOT)
	// VERIFY BEFORE ANY MEANINGFUL USE
	uint32_t name_string_index;
	uint32_t type;
	uint32_t flags;
	uint32_t address;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t address_align;
	uint32_t entry_size;

} __attribute__((__packed__))
ELF64SectionHeader_t;


#define ELF_SEC_NULL			0x0
#define ELF_SEC_PROGINFO		0x1
#define ELF_SEC_SYMTABLE		0x2
#define ELF_SEC_STRTABLE		0x3
#define ELF_SEC_RELOC_ADDEND	0x4
#define ELF_SEC_NOT_PRESENT		0x8
#define ELF_SEC_RELOC_NADDEND	0x9
#define ELF_SEC_DYN_SYMTABLE	0xb

#define ELF_SEC_ATTR_WRITE      0x01
#define ELF_SEC_ATTR_ALLOC      0x02



typedef struct
{

	uint32_t name_string_index;
	uint32_t value;
	uint32_t size;
	uint8_t info;
	uint8_t other;
	uint16_t shndx;


} __attribute__((__packed__))
ELF32Symbol_t;


typedef struct
{

	// TODO: NO IDEA IF THIS IS CORRECT (PROBABLY NOT)
	// VERIFY BEFORE ANY MEANINGFUL USE
	uint32_t name_string_index;
	uint32_t value;
	uint32_t size;
	uint8_t info;
	uint8_t other;
	uint16_t shndx;


} __attribute__((__packed__))
ELF64Symbol_t;


#define ELF_SYM_LOCAL	0x0
#define ELF_SYM_GLOBAL	0x1
#define ELF_SYM_WEAK	0x2

#define ELF_SYM_NOTYPE	0x0
#define ELF_SYM_OBJECT	0x1
#define ELF_SYM_FUNC	0x2

#define ELF_SHN_ABS		0xFFF1



typedef struct
{

	uint32_t offset;
	uint32_t info;

} __attribute__((__packed__))
ELF32Rel_t;




typedef struct
{

	// TODO: NO IDEA IF THIS IS CORRECT (PROBABLY NOT)
	// VERIFY BEFORE ANY MEANINGFUL USE

	uint64_t offset;
	uint32_t info;

} __attribute__((__packed__))
ELF64Rel_t;


#ifndef __cplusplus
typedef struct
{

	uint32_t offset;
	uint32_t info;
	int32_t addend;

} __attribute__((__packed__))
ELF32RelAdd_t;


typedef struct
{

	// TODO: NO IDEA IF THIS IS CORRECT (PROBABLY NOT)
	// VERIFY BEFORE ANY MEANINGFUL USE

	uint64_t offset;
	uint32_t info;
	int62_t addend;

} __attribute__((__packed__))
ELF64RelAdd_t;

#else

struct ELF32RelAdd_t : public ELF32Rel_t
{
	int32_t addend;
} __attribute__((__packed__));

struct ELF64RelAdd_t : public ELF64Rel_t
{

	// TODO: NO IDEA IF THIS IS CORRECT (PROBABLY NOT)
	// VERIFY BEFORE ANY MEANINGFUL USE

	int64_t addend;
} __attribute__((__packed__));

#endif


#define ELF_RELOC_386_NONE	0x0
#define ELF_RELOC_386_32	0x1		// Symbol + Offset
#define ELF_RELOC_386_PC32	0x2		// Symbol + Offset - Section_Offset

#define ELF_RELOC_SO_386_32	0x7		// Symbol + Offset



int ELF_verify_magic(const void*);
int ELF_parse_info(const void*, int* is_32_bit, int* is_little_endian, 
					int* is_x86, int* ELF_version, int* ELF_kind);



C_END


#ifdef __cplusplus

namespace detail
{
	template <class T>
	struct elf_entry_type_chooser;

	template <>
	struct elf_entry_type_chooser<ELF32Header_t>
	{
		typedef ELF32ProgramEntry_t		program_type;
		typedef ELF32SectionHeader_t	section_type;
		typedef ELF32Symbol_t			symbol_type;
		typedef uint32_t				word_type;
		typedef ELF32Rel_t				relocation_type;
		typedef ELF32RelAdd_t			relocation_add_type;
		constexpr static bool is_32 = true;
	};

	template <>
	struct elf_entry_type_chooser<ELF64Header_t>
	{
		typedef ELF64ProgramEntry_t program_type;
		typedef ELF64SectionHeader_t section_type;

		typedef uint64_t				word_type;
		typedef ELF64Rel_t				relocation_type;
		typedef ELF64RelAdd_t			relocation_add_type;
		constexpr static bool is_32 = false;
	};
}


template <class>
class ELFSymbolTable;
template <class HeaderType>
class ELFSymbol;

template <class>
class ELFRelocationTable;
template <class>
class ELFReloc;


template <class HeaderType>
class ELFObject
{
	public:
	typedef HeaderType header_type;
	typedef decltype(((header_type*)nullptr)->entry_position) address_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::program_type program_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	//typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type symbol_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;
	typedef ELFRelocationTable<header_type> relocation_table_type;

	protected:
	header_type* __header;
	program_header_type* __program_table;
	symbol_table_type** __symbol_tables;
	size_t __symbol_table_count;
	relocation_table_type** __relocation_tables;
	size_t __relocation_table_count;

	


	void __build_tables();


	public:

	ELFObject(void* header) :	__header(nullptr), __program_table(nullptr), __symbol_tables(nullptr), 
								__symbol_table_count(0), __relocation_tables(nullptr),
								__relocation_table_count(0)
	{
		assert(header);
		assert(ELF_verify_magic(header));

		int is_32;
		int little_end;
		int x86;
		int kind;
		int ver;
		
		int res = ELF_parse_info(header, &is_32, &little_end, &x86, &ver, &kind);
		assert(res == 1);
		if (detail::elf_entry_type_chooser<header_type>::is_32)
		{
			assert(is_32 == 1);
		}
		else
		{
			assert(is_32 == 0);
		}

		// TODO: little_end


		assert(x86 == 1);

		assert(kind != ELF_KIND_UNKNOWN);

		if (kind == ELF_KIND_EXEC)
		{
			assert(NOT_IMPLEMENTED);
		}
		else if (kind == ELF_KIND_CORE)
		{
			assert(NOT_IMPLEMENTED);
		}
		else if (kind == ELF_KIND_SHARED)
		{
			// TODO: All good for now(?)
			//assert(NOT_IMPLEMENTED);
		}
		else if (kind == ELF_KIND_RELOC)
		{
			// TODO: All good for now
		}

		assert(ver == 1);


		__header = reinterpret_cast<header_type*>(header);

		__program_table =
				reinterpret_cast<program_header_type*>(
					reinterpret_cast<unsigned char*>(__header) + __header->program_header_offset
				);
		
		assert(sizeof(program_header_type) == __header->program_header_entry_size);



		// Class is initialized enough to call section_header_table() at this time
		__build_tables();
	}


	size_t program_header_count() const noexcept
	{
		return __header->program_header_count;
	}

	const program_header_type* program_header_table() const noexcept
	{
		return __program_table;
	}

	size_t section_header_count() const noexcept
	{
		return __header->section_header_count;
	}

	size_t section_header_entry_size() const noexcept
	{
		return __header->section_header_entry_size;
	}

	const section_header_type* section_header_table() const noexcept
	{
		return (const section_header_type*)((addr_t)__header + __header->section_header_offset);
	}

	size_t section_names_index() const noexcept
	{
		return __header->section_header_strings_index;
	}

	const section_header_type* section_header(const int index) const noexcept
	{
		assert(index > 0);
		assert(index < __header->section_header_count);
		return &section_header_table()[index];
	}

	const char* section_name(int index) const noexcept
	{
		const char* string_table = this->section_name_table();
		if (string_table)
		{
			auto section = section_header(index);
			if (section)
			{
				if (section->name_string_index != 0)
				{
					return string_table + section->name_string_index;
				}
			}
		}

		return nullptr;
	}

	const char* section_name_table() const noexcept
	{
		return (const char*)__header + section_header(section_names_index())->offset;
	}

	// const symbol_type* symbol_table(int index) const noexcept
	// {
	//     auto sheader = section_header(index);
	//     if (sheader)
	//     {
	//         if (sheader->type == ELF_SEC_SYM)
	//         {
	//             return (const symbol_type*)((addr_t)__header + sheader->offset);
	//         }
	//     }

	//     return nullptr;
	// }

	// const symbol_type* symbol(int table_index, int index) const noexcept
	// {
	// 	auto header = section_header(table_index);
	// 	if (header && header->type == ELF_SEC_SYMTABLE)
	// 	{
	// 		if (index < (header->size / header->entry_size))
	// 		{
	// 			addr_t symbol_address = (addr_t)__header + header->offset;
	// 			auto sym = &((const symbol_type*)symbol_address)[index];
	// 			if (sym->shndx == 0x00)
	// 			{
	// 				// TODO
	// 				return nullptr;
	// 				assert(NOT_IMPLEMENTED);
	// 			}
				
	// 			return sym;
	// 		}

	// 	}
		
		

	// 	return nullptr;
	// }

	template <class T>
	const char* name_lookup(const T* t) const noexcept
	{
		assert(t);

		const char* string_table = this->section_name_table();
		if (string_table)
		{
			if (t->name_string_index != 0)
			{
				return string_table + t->name_string_index;
			}
		}

		return nullptr;
	}


	const section_header_type* section_header(const char* name, int startIndex = 1) const noexcept
	{
		auto index = section_header_index(name, startIndex);
		if (index > 0)
		{
			return section_header(index);
		}
		else
		{
			return nullptr;
		}
	}

	size_t section_header_index(const char* name, int startIndex = 1) const noexcept
	{
		assert(name);

		if (unlikely(startIndex < 1))
		{
			startIndex = 1;
		}

		for (int i = startIndex; i < this->section_header_count(); ++i)
		{
			auto sec_name = section_name(i);
			if (sec_name)
			{
				if (strcmp(sec_name, name) == 0)
				{
					return i;
				}
			}
		}

		return 0;
	}

	size_t symbol_table_count() const noexcept
	{
		return __symbol_table_count;
	}

	const symbol_table_type* symbol_table(int index) const noexcept
	{
		assert(index < __symbol_table_count);
		assert(index >= 0);
		return __symbol_tables[index];
	}

	const symbol_table_type* symbol_table(const char* name) const noexcept;
	const symbol_table_type* symbol_table(const section_header_type* sec) const noexcept;

	const header_type* get() const noexcept
	{
		return __header;
	}

	size_t relocation_table_count() const noexcept
	{
		return __relocation_table_count;
	}

	const relocation_table_type* relocation_table(int index) const noexcept
	{
		assert(index < __relocation_table_count);
		assert(index >= 0);
		return __relocation_tables[index];
	}

	const relocation_table_type* relocation_table(const char* name) const noexcept;

	uint16_t kind() const noexcept
	{
		return __header->kind;
	}


	const symbol_type* symbol(const char* name) const noexcept;



	friend symbol_table_type;
};

typedef ELFObject<ELF32Header_t> ELF32Object;
typedef ELFObject<ELF64Header_t> ELF64Object;



template <class HeaderType, class SectionType>
class ELFSection
{
	public:
	typedef HeaderType header_type;
	typedef SectionType raw_section_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;

	protected:
	ELFObject<header_type>* __object;
	const raw_section_type* __section;

	public:


};


template <class HeaderType>
class ELFSymbolTable
{
	public:
	typedef HeaderType header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type raw_symbol_type;
	typedef ELFSymbol<header_type> symbol_type;

	private:
	ELFObject<header_type>* __object;
	const section_header_type* __section;
	const char* __symbol_string_table;
	size_t __symbol_count;
	symbol_type** __symbols;


	void __build_symbols();

	public:
	ELFSymbolTable(ELFObject<header_type>* parent, const section_header_type* section) 
			:	__object(parent), __section(section),
				__symbol_string_table(nullptr)
	{
		assert(parent);
		assert(section);

		assert(__section->entry_size > 0);
		assert(sizeof(raw_symbol_type) == __section->entry_size);

		auto str_tab = parent->section_header(__section->link);
		if (str_tab)
		{
			__symbol_string_table = reinterpret_cast<const char*>(parent->__header) + str_tab->offset;
		}

		__build_symbols();
	}


	const char* table_name() const noexcept
	{
		if (__section->name_string_index != 0)
		{
			auto ntable = __object->section_name_table();
			if (ntable)
			{
				return ntable + __section->name_string_index;
			}
		}

		return nullptr;
	}

	size_t symbol_count() const noexcept
	{
		return __section->size / __section->entry_size;
	}


	const raw_symbol_type* raw_symbol(int index) const noexcept
	{
		assert(index < symbol_count());
		assert(index > 0);
		return &(reinterpret_cast<const raw_symbol_type*>(((addr_t)__object->__header) + __section->offset)[index]);
	}

	const symbol_type* symbol(int index) const noexcept
	{
		assert(index < symbol_count());
		assert(index > 0);
		assert(__symbols);
		return __symbols[index - 1];
	}

	const section_header_type* get() const noexcept
	{
		return __section;
	}

	const symbol_type* symbol(const char* name) const noexcept;

	friend symbol_type;

};

typedef ELFSymbolTable<ELF32Header_t> ELF32Symbols;
typedef ELFSymbolTable<ELF64Header_t> ELF64Symbols;


template <class HeaderType>
class ELFSymbol
{
	public:
	typedef HeaderType header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type	symbol_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::word_type		word_type;

	private:
	ELFObject<header_type>* __object;
	ELFSymbolTable<header_type>* __table;
	const symbol_type* __symbol;

	public:

	ELFSymbol(ELFObject<header_type>* object, ELFSymbolTable<header_type>* table, const symbol_type* symbol)
			: __object(object), __table(table), __symbol(symbol)
	{
		assert(object);
		assert(table);
		assert(symbol);
	}

	const char* name() const noexcept
	{
		if (__symbol->name_string_index != 0)
		{
			if (__table->__symbol_string_table)
			{
				return __table->__symbol_string_table + __symbol->name_string_index;
			}
		}
		return nullptr;
	}

	uint8_t binding() const noexcept
	{
		return (__symbol->info >> 4);
	}

	uint8_t type() const noexcept
	{
		return (__symbol->info & 0x0F);
	}

	uint16_t shndx() const noexcept
	{
		return __symbol->shndx;
	}

	addr_t value() const noexcept
	{
		if (__symbol->shndx == 0x00)
		{
			// External symbol
			auto s = __object->section_header(__table->__section->link);
			assert(s);
			const char* name = this->name();
			assert(name);

			//extern void* ELF_lookup_symbol(const char* name);
			void* target = nullptr;//ELF_lookup_symbol(name);

			if (!target)
			{
				if (binding() & ELF_SYM_WEAK)
				{
					return 0;
				}
				else
				{
					// DEBUG
					assert(NOT_IMPLEMENTED);
					return 0;
					
				}
			}
			else
			{
				return (addr_t)target;
			}
		}
		else if (__symbol->shndx == ELF_SHN_ABS)
		{
			return __symbol->value;
		}
		else
		{
			auto target = __object->section_header(__symbol->shndx);
			assert(target);

			switch (__object->kind())
			{
				case ELF_KIND_UNKNOWN:
					assert(NOT_IMPLEMENTED);
				
				case ELF_KIND_RELOC:
					return (addr_t)__object->get() + target->offset + __symbol->value;
				
				case ELF_KIND_SHARED:
				case ELF_KIND_EXEC:
					return (addr_t)__object->get() + __symbol->value;

				case ELF_KIND_CORE:
					assert(NOT_IMPLEMENTED);

				default:
					assert(NOT_IMPLEMENTED);
			}
		}
	}

	word_type raw_value() const noexcept
	{
		return __symbol->value;
	}

};

typedef ELFSymbol<ELF32Header_t> ELF32Symbol;
typedef ELFSymbol<ELF64Header_t> ELF64Symbol;



template <class HeaderType>
class ELFRelocationTable
{
	public:
	typedef HeaderType header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::relocation_type raw_relocation_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::relocation_add_type raw_relocation_addend_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::word_type word_type;
	typedef ELFReloc<header_type> relocation_type;

	protected:
	ELFObject<header_type>* __object;
	const section_header_type* __section;
	relocation_type** __relocs;

	void __build_relocs();

	public:

	ELFRelocationTable(ELFObject<header_type>* parent, const section_header_type* section) : __object(parent), __section(section), __relocs(nullptr)
	{
		assert(parent);
		assert(section);


		__build_relocs();
	}


	const char* table_name() const noexcept
	{
		if (__section->name_string_index != 0)
		{
			auto ntable = __object->section_name_table();
			if (ntable)
			{
				return ntable + __section->name_string_index;
			}
		}

		return nullptr;
	}

	bool is_addend() const noexcept
	{
		return __section->type == ELF_SEC_RELOC_ADDEND;
	}

	word_type link() const noexcept
	{
		return __section->link;
	}

	__attribute__((__always_inline__))
	size_t reloc_count() const noexcept
	{
		return count();
	}

	size_t count() const noexcept
	{
		if (is_addend())
		{
			return __section->size / sizeof(raw_relocation_addend_type);
		}
		else
		{
			return __section->size / sizeof(raw_relocation_type);
		}
	}

	const raw_relocation_type* raw_reloc(int index) const noexcept
	{
		assert(index > 0);
		assert(index < count());
		if (is_addend())
		{
			return &(reinterpret_cast<const raw_relocation_addend_type*>(((addr_t)__object->get()) + __section->offset)[index]);
		}
		else
		{
			return &(reinterpret_cast<const raw_relocation_type*>(((addr_t)__object->get()) + __section->offset)[index]);
		}
	}

	const raw_relocation_addend_type* raw_addend_reloc(int index) const noexcept
	{
		assert(index > 0);
		assert(index < count());
		if (is_addend())
		{
			return &(reinterpret_cast<const raw_relocation_addend_type*>(((addr_t)__object->get()) + __section->offset)[index]);
		}
		else
		{
			return nullptr;
		}
	}

	const relocation_type* reloc(int index) const noexcept
	{
		assert(index > 0);
		assert(index < count());

		return __relocs[index-1];
	}

	const relocation_type* addend_reloc(int index) const noexcept
	{
		if (is_addend())
		{
			return reloc(index);
		}
		else
		{
			return nullptr;
		}
	}

	auto info() const noexcept
	{
		return __section->info;
	}
};


template <class HeaderType>
class ELFReloc
{
	public:
	typedef HeaderType header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::relocation_type raw_relocation_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::relocation_add_type raw_relocation_addend_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::word_type word_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;

	protected:
	ELFObject<header_type>* __object;
	ELFRelocationTable<header_type>* __section;
	const raw_relocation_type* __reloc;


	public:
	ELFReloc(ELFObject<header_type>* object, ELFRelocationTable<header_type>* section, const raw_relocation_type* reloc) : __object(object), __section(section), __reloc(reloc)
	{
		assert(object);
		assert(section);
		assert(reloc);
	}

	bool is_addend() const noexcept
	{
		return __section->is_addend();
	}

	word_type info_symbol() const noexcept
	{
		// The bitshifting process for 64-bit may be different, need to check that before using this for ELF64
		static_assert(detail::elf_entry_type_chooser<header_type>::is_32);
		
		return __reloc->info >> 8;
	}

	word_type info_type() const noexcept
	{
		// The bitshifting process for 64-bit may be different, need to check that before using this for ELF64
		static_assert(detail::elf_entry_type_chooser<header_type>::is_32);


		return __reloc->info & 0xFF; 
	}

	
	const symbol_type* symbol() const noexcept
	{
		if (info_symbol() > 0)
		{
			auto hdr = __object->section_header(__section->link());
			assert(hdr);
			auto sym_table = __object->symbol_table(hdr);
			assert(sym_table);

			return sym_table->symbol(info_symbol());
		}
		return nullptr;
	}

	const void* target_loc() const noexcept
	{
		auto target = __object->section_header(__section->info());
		assert(target);

		void* ref = (void*)((addr_t)__object->get() + __reloc->offset);
		
		return ref;
	}

	const void* reloc_value() const noexcept
	{
		word_type* ref = (word_type*)this->target_loc();

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

		switch (info_type())
		{
			case ELF_RELOC_386_NONE:
				assert(NOT_IMPLEMENTED);
				break;
			case ELF_RELOC_386_32:
			case ELF_RELOC_SO_386_32:
				return (const void*)(*ref + (addr_t)symval);

			case ELF_RELOC_386_PC32:
				return (const void*)(*ref + (addr_t)symval - /* --> */ (addr_t)ref /* <-- ????? I'm not sure about this at all. There's a chance the documentation made a mistake*/);
			
			default:
				assert(NOT_IMPLEMENTED);
		}
	}
	
};

typedef ELFReloc<ELF32Header_t> ELF32Reloc;
typedef ELFReloc<ELF64Header_t> ELF64Reloc;


#endif



END_NS

#ifdef __cplusplus
#include "ELF.hpp"
#endif


#endif