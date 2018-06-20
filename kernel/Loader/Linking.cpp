#include <kernel/Loader/Linking.hh>
#include <boot/MultiBoot.hh>
#include <Utils/iostream>

namespace Kernel
{
	ELFSymbols* ELF_kernel_symbols = nullptr;

	extern "C" void link_callback_handler();


	struct LinkerCallbackArg_t
	{
		typename ELFSymbols::object_type* object;
		typename ELFSymbols::symbol_type* GOT;

	};


	void ELFSymbols::link_object(object_type* obj)
	{
		assert(obj);
		obj->verify_integrity();
		objects.push_back(obj);

		for (size_t i = 0; i < obj->symbol_table_count(); ++i)
		{
			symbol_tables.push_back(const_cast<symbol_table_type*>(obj->symbol_table(i)));
		}

		for (size_t i = 0; i < obj->symbol_table_count(); ++i)
		{
			auto tbl = obj->symbol_table(i);
			assert(tbl);
			for (size_t j = 0; j < tbl->symbol_count(); ++j)
			{
				auto sym = tbl->symbol(j);
				assert(sym);
			}
		}

		TRACE("Getting GOT...");
		obj->verify_integrity();
		auto GOT = obj->symbol("_GLOBAL_OFFSET_TABLE_");
		TRACE("Got GOT.");
		if (GOT)
		{
			void** linker_callback = (void**)(GOT->value() + 2*sizeof(addr_t));
			void** linker_callback_arg = (void**)(GOT->value() + sizeof(addr_t));

			auto arg = new LinkerCallbackArg_t;
			arg->object = obj;
			arg->GOT = const_cast<typename ELFSymbols::symbol_type*>(GOT);
			*linker_callback_arg = arg;
			*linker_callback = (void*)&link_callback_handler;
			std::cout << "arg: " << arg << std::endl << "linker_callback: " << (void*)&link_callback_handler << std::endl;
		}
		else
		{
			assert(false);
		}
		obj->verify_integrity();

		std::cout << "obj: " << obj << std::endl;
	}

	void* ELFSymbols::resolve_symbol(LinkerCallbackArg_t* arg, addr_t index)
	{
		assert(arg);
		assert(arg->object);
		assert(arg->GOT);

		relocation_type* reloc = nullptr;
		
		size_t searched = 0;

		for (size_t i = 0; i < arg->object->relocation_table_count() && reloc == nullptr; ++i)
		{
			auto tbl = arg->object->relocation_table(i);
			assert(tbl);

			for (size_t j = 0; j < tbl->count(); ++j)
			{
				++searched;
				auto r = tbl->reloc(j);
				assert(r);
				
				if (r->target() == (void*)(arg->GOT->value() + (index / 2) + 3*sizeof(addr_t)))
				{
					reloc = const_cast<relocation_type*>(r);
					break;
				}
				else
				{
					std::cout << "Not it: '" << r->symbol()->name() << "   " << (void*)((addr_t)r->target() - arg->GOT->value()) << std::endl;
				}
			}
		}

		std::cout << "Searched " << searched << "." << std::endl;
		assert(reloc);

		const char* name = nullptr;
		{
			auto sym = reloc->symbol();
			assert(sym);
			name = sym->name();
		}
		assert(name);

		std::cout << "Name: " << name << std::endl;

		searched = 0;

		for (auto object : this->objects)
		{
			// if (object == arg->object)
			// {
			// 	continue;
			// }

			

			auto sym = object->symbol(name);
			if (sym)
			{
				++searched;
				if (sym->raw_value() != 0)
				{
					std::cout << "Found it!\n" << std::endl;
					std::cout << "Setting " << (void*)reloc->target() << std::endl;
					*(void**)reloc->target() = (void*)sym->value();
					return (void*)sym->value();
				}
			}
		}

		std::cout << "Searched " << searched << "." << std::endl;
		assert(false);
	}




	extern "C"
	{
		void* linker_callback(LinkerCallbackArg_t* arg1, addr_t arg2)
		{
			TRACE("CALLBACK!");
			std::cout << arg1 << std::endl;
			std::cout << arg2 << std::endl;
			assert(ELF_kernel_symbols);
			auto result = ELF_kernel_symbols->resolve_symbol(arg1, arg2);
			std::cout << "Returning result " << result << std::endl;
			ASM_CODE("xchg %bx, %bx");
			return result;
			while (true) ;
		}
	}



	void init_kernel_symbols()
	{
		assert(ELF_kernel_symbols == nullptr);

		ELF_kernel_symbols = new ELFSymbols;

		assert(boot::mboot->has_ELF_symbols());
		ELF32SectionHeader_t* sec_table = ((ELF32SectionHeader_t*)boot::mboot->raw()->u.elf_sec.addr);

		
		for (uint32_t i = 1; i < boot::mboot->raw()->u.elf_sec.num; ++i)
		{
			sec_table[i].offset = sec_table[i].address - (addr_t)sec_table;
		}

		// auto syms = new typename ELFSymbols::object_type(
		// 			nullptr,
		// 			0,
		// 			0,
		// 			boot::mboot->raw()->u.elf_sec.addr,
		// 			boot::mboot->raw()->u.elf_sec.num,
		// 			ELF_KIND_EXEC,
		// 			boot::mboot->raw()->u.elf_sec.shndx
		// );

		std::cout << "Building ELF object..." << std::endl;

		auto syms = new typename ELFSymbols::object_type(
					(typename ELFSymbols::object_type::section_header_type*)boot::mboot->raw()->u.elf_sec.addr,
					boot::mboot->raw()->u.elf_sec.num,
					0x0,
					boot::mboot->raw()->u.elf_sec.shndx,
					ELF_KIND_EXEC
		);

		std::cout << "LINKING" << std::endl;

		ELF_kernel_symbols->link_object(syms);
		std::cout << "LINKED!" << std::endl;
	}








	ELF32Object* ELF32_unpack_parse(const uint8_t* buffer, size_t length, const void* sections, size_t sections_count, const void* program_headers, size_t program_headers_count, int section_names_index)
	{
		assert(buffer);
		assert(length > 0);
		assert(sections);
		assert(program_headers);

		size_t needed_size = 0;

		const Kernel::ELF32ProgramEntry_t* pheaders = reinterpret_cast<const Kernel::ELF32ProgramEntry_t*>(program_headers);
		for (size_t i = 0; i < program_headers_count; ++i)
		{

			if (needed_size < (pheaders[i].virtual_addr + pheaders[i].memory_segment_size))
			{
				needed_size = (pheaders[i].virtual_addr + pheaders[i].memory_segment_size);
			}
		}
		
		size_t sec_table_size = sizeof(Kernel::ELF32SectionHeader_t)*sections_count;
		size_t meta_size = sec_table_size;// + sizeof(Kernel::ELF32ProgramEntry_t)*program_headers_count;

		const Kernel::ELF32SectionHeader_t* sheaders = reinterpret_cast<const Kernel::ELF32SectionHeader_t*>(sections);
		for (size_t i = 1; i < sections_count; ++i)
		{
			auto header = &sheaders[i];
			if (!(header->flags & ELF_SEC_ATTR_ALLOC))
			{
				meta_size += header->size;
			}
		}

		size_t total_size = meta_size + needed_size;
		uint8_t* program = new uint8_t[total_size];
		std::cout << "Program: " << (void*)program << std::endl;
		memset(program, 0, total_size);
		memcpy(program, sections, sec_table_size);

		uint8_t* loaded = program + meta_size;
		std::cout << "Loaded: " << (void*)loaded << std::endl;
		std::cout << "Total Size: " << total_size << " (" << (void*)total_size << ")" << std::endl;
		std::cout << "Size for Loaded: " << ((program + total_size) - loaded) << " (" << (void*)((program + total_size) - loaded) << ")" << std::endl;
		std::cout << "End Address: " << (void*)(program + total_size) << std::endl;
		std::cout << "Meta Size: " << meta_size << " (" << (void*)meta_size << ")" << std::endl;

		size_t sec_index = 1;
		uint8_t* section_contents_it = program + sec_table_size;
		for (size_t i = 1; i < sections_count; ++i)
		{
			auto header = &sheaders[i];
			auto nheader = reinterpret_cast<Kernel::ELF32SectionHeader_t*>(program + (sizeof(Kernel::ELF32SectionHeader_t)*i));
			if (!(header->flags & ELF_SEC_ATTR_ALLOC))
			{
				memcpy(section_contents_it, (const void*)((addr_t)buffer + header->offset), header->size);
				nheader->offset = (section_contents_it - program);
				nheader->address = (addr_t)section_contents_it; // ?
				section_contents_it += header->size;
			}
			else
			{
				nheader->offset = meta_size + header->address;
				//nheader->address = nheader->offset;
			}
			assert(header->name_string_index == nheader->name_string_index);
		}

		assert(section_contents_it == loaded);

		for (size_t i = 0; i < program_headers_count; ++i)
		{
			std::cout << (void*)(pheaders[i].file_offset + buffer) << " -> " << (void*)(pheaders[i].virtual_addr + loaded) << std::endl;
			std::cout << (void*)((pheaders[i].file_offset + buffer) - buffer) << " -> " << (void*)((pheaders[i].virtual_addr + loaded) - program) << std::endl;
			memcpy(pheaders[i].virtual_addr + loaded, pheaders[i].file_offset + buffer, pheaders[i].file_segment_size);
		}

		auto foo_hard = (uint32_t(*)())(loaded + 0x450);
		assert(foo_hard() == 0xDEADBABA);
		std::cout << (void*)foo_hard() << std::endl;
		std::cout << "foo_hard: " << (void*)foo_hard << std::endl;


		auto new_sheaders = reinterpret_cast<const Kernel::ELF32SectionHeader_t*>(program);
		assert(new_sheaders[1].name_string_index == sheaders[1].name_string_index);

		ELF32Object* obj = new ELF32Object(new_sheaders, sections_count, loaded, section_names_index, ELF_KIND_SHARED);
		// ELF32Object* obj = new ELF32Object(loaded, 0, 0, 0, sections_count, ELF_KIND_RELOC, section_names_index);
		obj->verify_integrity();
		
		

		auto names_table1 = (const char*)((sheaders + section_names_index)->offset + buffer);
		auto names_table2 = (const char*)((reinterpret_cast<const Kernel::ELF32SectionHeader_t*>(program) + section_names_index)->address);

		// for (int i = 0; i < obj->section_header_count(); ++i)
		// {
		// 	std::cout << obj->name_lookup(obj->section_header(i)) << std::endl;
		// }

		// for (int i = 0; i < obj->symbol_table_count(); ++i)
		// {
		// 	auto tbl = obj->symbol_table(i);
		// 	assert(tbl);
		// 	for (int j = 0; j < tbl->symbol_count(); ++j)
		// 	{
		// 		auto sym = tbl->symbol(j);
		// 		assert(sym);
		// 		std::cout << sym->name() << std::endl;
		// 		std::cout << "\t" << (void*)sym->value() << "\t" << (void*)(sym->value() - (addr_t)program) << std::endl;
		// 	}
		// }

		obj->verify_integrity();


		for (size_t i = 0; i < obj->relocation_table_count(); ++i)
		{

			
			auto rtbl = obj->relocation_table(i);
			assert(rtbl);
			
			std::cout << "\n\n";
			if (rtbl->has_info())
			{
				
				auto targ = obj->section(rtbl->info());
				assert(targ);
				std::cout << rtbl->name() << " -> " << targ->name() << "  [ A: " << (void*)(addr_t)targ->header()->address << ", O: " << (void*)(addr_t)targ->header()->offset << " ]" << std::endl;
			}
			
			for (size_t j = 0; j < rtbl->count(); ++j)
			{
				auto rloc = rtbl->reloc(j);
				assert(rloc);
				if (rloc->reloc_value() != nullptr)
				{
					if (rloc->symbol())
					{
						if (rloc->symbol()->name())
						{
							std::cout << rloc->symbol()->name() << std::endl;
						}
					}
					std::cout << "\t " << (void**)rloc->target() << " \t " << (void*)rloc->reloc_value() << std::endl;
					std::cout << "\t *(" << (void**)((addr_t)rloc->target() - (addr_t)loaded)  << ") = \t " << (void*)((addr_t)rloc->reloc_value() - (addr_t)loaded) << std::endl;
					std::cout << "\t (" << *(void**)(addr_t)rloc->target()  << ") = \t " << (void*)((addr_t)rloc->reloc_value() - (addr_t)loaded) << std::endl;
					std::cout << "\t (Type = " << rloc->info_type() << ")" << std::endl;
					std::cout << "\t [Symval = " << (void*)rloc->symbol()->value() << "]" << std::endl;
					std::cout << "\t [Symval (Relative) = " << (void*)(rloc->symbol()->value() - (addr_t)loaded) << "]" << std::endl;
					std::cout << "\t [Symval (Raw) = " << (void*)rloc->symbol()->raw_value() << "]" << std::endl;
					std::cout << "\t [Reloc Offset: " << (void*)(addr_t)rloc->reloc_offset() << "]" << std::endl;
					*(void**)rloc->target() = rloc->reloc_value();
					

				}
				
			}

			std::cout << "\n\n";
		}

		obj->verify_integrity();

		return obj;

		// for (int i = 0; i < obj->symbol_table_count(); ++i)
		// {
		// 	auto tbl = obj->symbol_table(i);
		// 	assert(tbl);
		// 	for (int j = 0; j < tbl->symbol_count(); ++j)
		// 	{
		// 		auto sym = tbl->symbol(j);
		// 		assert(sym);
		// 		std::cout << sym->name() << std::endl;
		// 		std::cout << "\t" << (void*)sym->value() << "\t" << (void*)(sym->value() - (addr_t)program) << std::endl;
		// 	}
		// }

		auto got_plt = obj->section_header(".got.plt");
		assert(got_plt);
		obj->verify_integrity();

		//assert(got_plt->offset - got_plt->offset
		

		auto foo_sym = obj->symbol("foo");
		assert(foo_sym);
		std::cout << "foo_sym->name(): " << foo_sym->name() << std::endl;
		obj->verify_integrity();
		auto foo = (uint32_t(*)())foo_sym->value();
		assert(foo);
		std::cout << "foo: " << (void*)foo << std::endl;
		std::cout << "foo: " << (void*)((addr_t)(void*)foo - (addr_t)loaded) << std::endl;
		std::cout << "foo: " << (void*)((addr_t)(void*)foo - (addr_t)program) << std::endl;
		std::cout << "foo_hard (Relative): " << (void*)((addr_t)(void*)foo_hard - (addr_t)loaded) << std::endl;
		assert(foo_hard() == 0xDEADBABA);
		std::cout << "Foo_hard worked." << std::endl;
		assert(foo() == 0xDEADBABA);

		std::cout << "Foo checks out." << std::endl;

		auto foo2_sym = obj->symbol("foo2");
		assert(foo2_sym);
		auto foo2 = (uint32_t(*)())foo2_sym->value();
		assert(foo2);
		std::cout << "foo2: " << (void*)foo2 << std::endl;
		std::cout << "foo2: " << (void*)((addr_t)(void*)foo2 - (addr_t)loaded) << std::endl;
		std::cout << "foo2: " << (void*)((addr_t)(void*)foo2 - (addr_t)program) << std::endl;
		{
			irq_guard lock;
			ASM_CODE("xchg %bx, %bx");
			assert(foo2() == 0xDEADBABA);
			ASM_CODE("xchg %bx, %bx");
		}

		assert(false);
	}
}