#ifndef INCLUDED_ELF_H
#define INCLUDED_ELF_H

#include <Common.h>
#include <Std/string.h>
#include <Utils/vector>
#include <Utils/map>
#include <Utils/string>

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


#define ELF_RELOC_386_NONE			0x0
#define ELF_RELOC_386_32			0x1		// Symbol + Offset
#define ELF_RELOC_386_PC32			0x2		// Symbol + Offset - Section_Offset
#define ELF_RELOC_386_GLOB_DAT		0x6		// ??

#define ELF_RELOC_386_JUMP_SLOT		0x7		// Symbol + Offset



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
		typedef int32_t					sword_type;
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
		typedef int64_t					sword_type;
		typedef ELF64Rel_t				relocation_type;
		typedef ELF64RelAdd_t			relocation_add_type;
		constexpr static bool is_32 = false;
	};
}


template <class HeaderType>
class ELFObjectBase;

template <class HeaderType>
class ELFSection;

template <class>
class ELFSymbolTable;
template <class HeaderType>
class ELFSymbol;

template <class>
class ELFRelocationTable;
template <class>
class ELFReloc;








template <class HeaderType>
class ELFObjectBase
{
	public:
	typedef HeaderType header_type;
	typedef decltype(((header_type*)nullptr)->entry_position) address_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::program_type program_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type raw_symbol_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;
	typedef ELFRelocationTable<header_type> relocation_table_type;
	typedef ELFReloc<header_type> relocation_type;
	typedef ELFSection<header_type> section_type;

	protected:
	Utils::vector<const program_header_type*> _program_headers;
	Utils::vector<const section_header_type*> _section_headers;
	Utils::vector<symbol_table_type*> _symbol_tables;
	Utils::vector<relocation_table_type*> _relocation_tables;
	void* _sections_base_address;
	void* _program_base_address;
	uint16_t _kind;
	
	Utils::map<int, section_type*> _sections;
	Utils::map<Utils::string, symbol_type*> _symbols;
	Utils::map<Utils::string, relocation_type*> _relocations;

	public:

	ELFObjectBase(const header_type* h) : ELFObjectBase(h, (void*)h) { }
	ELFObjectBase(const header_type*, void* program_base);
	ELFObjectBase(const section_header_type* section_table, int sections_count, void* program_base, int section_name_index, uint16_t kind);
	ELFObjectBase(void* section_base, void* program_base, uint16_t kind);


	void add_symbol_table(const char* name, int index, const section_header_type* header, const void* section);
	void add_section(const char* name, int index, const section_header_type*, const void* section);
	void add_relocation_table(const char* name, int index, const section_header_type* header, const void* section);


	size_t symbol_table_count() const noexcept;
	size_t relocation_table_count() const noexcept;
	size_t section_count() const noexcept;


	const symbol_table_type* symbol_table(const size_t) const noexcept;
	symbol_table_type* symbol_table(const size_t) noexcept;

	const symbol_type* symbol(const char* name) const noexcept;
	symbol_type* symbol(const char* name) noexcept;

	
	const relocation_table_type* relocation_table(const size_t) const noexcept;
	relocation_table_type* relocation_table(const size_t) noexcept;

	const relocation_type* relocation(const char* name) const noexcept;
	relocation_type* relocation(const char* name) noexcept;


	const section_type* section(const size_t) const noexcept;
	section_type* section(const size_t) noexcept;
	const section_type* section(const char* name) const noexcept;
	section_type* section(const char* name) noexcept;

	const section_header_type* section_header(const size_t) const noexcept;
	const section_header_type* section_header(const char* name) const noexcept;


	uint16_t kind() const noexcept
	{
		return _kind;
	}
};

typedef ELFObjectBase<ELF32Header_t> ELF32Object;
typedef ELFObjectBase<ELF64Header_t> ELF64Object;



template <class HeaderType>
class ELFSection
{
	public:
	typedef HeaderType header_type;
	typedef decltype(((header_type*)nullptr)->entry_position) address_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::program_type program_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type raw_symbol_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;
	typedef ELFRelocationTable<header_type> relocation_table_type;
	typedef ELFReloc<header_type> relocation_type;

	protected:
	const char* __name;
	ELFObjectBase<header_type>* __object;
	const section_header_type* __section_header;
	const void* __section;


	public:
	ELFSection(const char* name, ELFObjectBase<header_type>* parent, const section_header_type* header, const void* section);

	
	virtual const void* section() const noexcept
	{
		return __section;
	}

	virtual size_t count() const noexcept { return (__section_header->size / __section_header->entry_size); }

	virtual const section_header_type* header() const noexcept { return __section_header; }
	virtual const ELFObjectBase<header_type>* parent() const noexcept { return __object; }
	virtual const char* name() const noexcept { return __name; }
};




template <class HeaderType>
class ELFSymbolTable : public ELFSection<HeaderType>
{
	public:
	typedef HeaderType header_type;
	typedef decltype(((header_type*)nullptr)->entry_position) address_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::program_type program_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type raw_symbol_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;
	typedef ELFRelocationTable<header_type> relocation_table_type;
	typedef ELFReloc<header_type> relocation_type;

	protected:
	const void* value_base_address;
	const char* symbol_names_table;
	Utils::vector<ELFSymbol<HeaderType>*> _symbols;

	public:

	ELFSymbolTable(const char* name, const void* value_base, ELFObjectBase<HeaderType>* parent, int _count, const void* table, const char* symbol_names_table);
	ELFSymbolTable(const char* name, const void* value_base, ELFObjectBase<HeaderType>* parent, const section_header_type* section_header, const void* table, const char* symbol_names_table);


	size_t symbol_count() const noexcept
	{ return this->count(); }
	virtual size_t count() const noexcept override;
	const symbol_type* symbol(const size_t) const noexcept;
	symbol_type* symbol(const size_t) noexcept;
	const symbol_type* symbol(const char* name) const noexcept;
	symbol_type* symbol(const char* name) noexcept;



	template <class>
	friend class ELFSymbol;

};



typedef ELFSymbolTable<ELF32Header_t> ELF32Symbols;
typedef ELFSymbolTable<ELF64Header_t> ELF64Symbols;



template <class HeaderType>
class ELFSymbol
{
	public:
	typedef HeaderType header_type;
	typedef decltype(((header_type*)nullptr)->entry_position) address_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::program_type program_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type raw_symbol_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;
	typedef ELFRelocationTable<header_type> relocation_table_type;
	typedef ELFReloc<header_type> relocation_type;
	typedef ELFSection<header_type> section_type;

	protected:
	const char* _name;
	int index;
	symbol_table_type* _table;
	raw_symbol_type* _raw_symbol;


	public:
	ELFSymbol(const char* name, int index, symbol_table_type* parent, raw_symbol_type* raw);


	const char* name() const noexcept { return _name; }
	address_type value() const noexcept;
	address_type raw_value() const noexcept { return _raw_symbol->value; }
	uint8_t binding() const noexcept { return (_raw_symbol->info >> 4); }
	uint8_t type() const noexcept { return (_raw_symbol->info & 0x0F); }
};

typedef ELFSymbol<ELF32Header_t> ELF32Symbol;
typedef ELFSymbol<ELF64Header_t> ELF64Symbol;



template <class HeaderType>
class ELFRelocationTable : public ELFSection<HeaderType>
{
	public:
	typedef HeaderType header_type;
	typedef decltype(((header_type*)nullptr)->entry_position) address_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::program_type program_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type raw_symbol_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::relocation_type raw_relocation_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;
	typedef ELFRelocationTable<header_type> relocation_table_type;
	typedef ELFReloc<header_type> relocation_type;
	typedef ELFSection<header_type> section_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::word_type word_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::sword_type sword_type;

	protected:
	const void* value_base_address;
	Utils::vector<ELFReloc<HeaderType>*> _relocs;

	public:

	ELFRelocationTable(const char* name, const void* value_base, ELFObjectBase<HeaderType>* parent, const section_header_type* section_header, const void* table);


	size_t reloc_count() const noexcept { return this->count(); }
	const relocation_type* reloc(const size_t) const noexcept;
	relocation_type* reloc(const size_t) noexcept;
	const relocation_type* reloc(const char* name) const noexcept;
	relocation_type* reloc(const char* name) noexcept;

	bool has_info() const noexcept
	{
		return this->__section_header->info != 0;
	}

	auto info() const noexcept
	{
		return this->__section_header->info - 1;
	}

		bool is_addend() const noexcept
	{
		return this->__section_header->type == ELF_SEC_RELOC_ADDEND;
	}

	word_type link() const noexcept
	{
		assert(this->__section_header->link > 0);
		return this->__section_header->link;
	}


	template <class>
	friend class ELFReloc;

};


template <class HeaderType>
class ELFReloc
{
	public:
	typedef HeaderType header_type;
	typedef decltype(((header_type*)nullptr)->entry_position) address_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::program_type program_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::section_type section_header_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::symbol_type raw_symbol_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::relocation_type raw_relocation_type;
	typedef ELFSymbolTable<header_type> symbol_table_type;
	typedef ELFSymbol<header_type> symbol_type;
	typedef ELFRelocationTable<header_type> relocation_table_type;
	typedef ELFReloc<header_type> relocation_type;
	typedef ELFSection<header_type> section_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::word_type word_type;
	typedef typename detail::elf_entry_type_chooser<header_type>::sword_type sword_type;

	protected:
	int index;
	relocation_table_type* _table;
	raw_relocation_type* _raw_reloc;


	public:
	ELFReloc(int index, relocation_table_type* parent, raw_relocation_type* raw);


	address_type* target() noexcept;
	const address_type* target() const noexcept;
	const symbol_type* symbol() const noexcept;
	symbol_type* symbol() noexcept;

	void* reloc_value() const noexcept;

	sword_type info_symbol() const noexcept;
	word_type info_type() const noexcept;

	auto reloc_offset() const noexcept { return _raw_reloc->offset; }

};

typedef ELFReloc<ELF32Header_t> ELF32Reloc;
typedef ELFReloc<ELF64Header_t> ELF64Reloc;


#endif



END_NS

#ifdef __cplusplus
#include "ELF.hpp"
#endif


#endif