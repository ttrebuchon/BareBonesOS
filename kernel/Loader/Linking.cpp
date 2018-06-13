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
		objects.push_back(obj);

		for (int i = 0; i < obj->symbol_table_count(); ++i)
		{
			symbol_tables.push_back(const_cast<symbol_table_type*>(obj->symbol_table(i)));
		}

		for (int i = 0; i < obj->symbol_table_count(); ++i)
		{
			auto tbl = obj->symbol_table(i);
			assert(tbl);
			for (int j = 0; j < tbl->symbol_count(); ++j)
			{
				auto sym = tbl->symbol(j);
				assert(sym);
			}
		}

		auto GOT = obj->symbol("_GLOBAL_OFFSET_TABLE_");
		if (GOT)
		{
			void** linker_callback = (void**)(GOT->value() + 2*sizeof(addr_t));
			void** linker_callback_arg = (void**)(GOT->value() + sizeof(addr_t));

			auto arg = new LinkerCallbackArg_t;
			arg->object = obj;
			arg->GOT = const_cast<typename ELFSymbols::symbol_type*>(GOT);
			*linker_callback_arg = arg;
			*linker_callback = (void*)&link_callback_handler;
		}
		else
		{
			assert(false);
		}

		std::cout << "obj: " << obj << std::endl;
	}

	void* ELFSymbols::resolve_symbol(LinkerCallbackArg_t* arg, addr_t index)
	{
		assert(arg);
		assert(arg->object);
		assert(index > 0);
		assert(arg->GOT);

		relocation_type* reloc = nullptr;
		
		int searched = 0;

		for (int i = 0; i < arg->object->relocation_table_count() && reloc == nullptr; ++i)
		{
			auto tbl = arg->object->relocation_table(i);
			assert(tbl);

			for (int j = 0; j < tbl->count(); ++j)
			{
				++searched;
				auto r = tbl->reloc(j);
				assert(r);
				
				if (r->target_loc() == (void*)(arg->GOT->value() + (index / 2) + 3*sizeof(addr_t)))
				{
					reloc = const_cast<relocation_type*>(r);
					break;
				}
				else
				{
					std::cout << "Not it: '" << r->symbol()->name() << "   " << (void*)((addr_t)r->target_loc() - arg->GOT->value()) << std::endl;
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
			if (object == arg->object)
			{
				continue;
			}

			

			auto sym = object->symbol(name);
			if (sym)
			{
				++searched;
				if (sym->raw_value() != 0)
				{
					std::cout << "Found it!\n" << std::endl;
					std::cout << "Setting " << (void*)reloc->target_loc() << std::endl;
					*(void**)reloc->target_loc() = (void*)sym->value();
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
			sleep(5);
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

		
		for (int i = 1; i < boot::mboot->raw()->u.elf_sec.num; ++i)
		{
			sec_table[i].offset = sec_table[i].address;
		}

		auto syms = new typename ELFSymbols::object_type(
					nullptr,
					0,
					0,
					boot::mboot->raw()->u.elf_sec.addr,
					boot::mboot->raw()->u.elf_sec.num,
					ELF_KIND_EXEC,
					boot::mboot->raw()->u.elf_sec.shndx
		);

		ELF_kernel_symbols->link_object(syms);
	}
}