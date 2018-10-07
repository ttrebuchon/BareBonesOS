#include "Tests.hh"
#define _TEST_SRC
#include <Utils/experimental/reflect>
#include <Utils/experimental/Serialize/serialize>
#include <Support/sqlite/serializer.hh>
#include <Support/sqlite/context.hh>
#include <sqlite3/sqlite3.h>

#include <Support/sqlite/serialize.hh>

using namespace Utils::experimental;

extern const char* sqlite_out_db;

#if __cplusplus >= 201703L

static_assert(Support::SQLite::has_model<int>::value);
static_assert(Support::SQLite::is_primitive<int>::value);

#endif


template <class T>
void debug_print_type()
{
	std::cout << __func__ << std::endl;
}


class DebugContext : public serialize::Context
{
	protected:
	
	virtual bool save_values(const reflect::Model* model, Utils::list<serialize::ValueSet*>& v) override
	{
		
		std::clog << model->name().c_str() << std::endl;
		const reflect::Model* keyModel;
		auto k = model->get_key(&keyModel);
		assert(k);
		assert(keyModel);
		auto kstr = k->name();
		std::clog << "\t" << kstr.c_str() << " - " << k->property_typeinfo()->info().name() << "\n";
		for (auto base : model->bases())
		{
			assert(base.linked);
			assert(base.model);
			//auto vals = 
		}
		
		for (auto vals : v)
		{
			for (auto prop : *model)
			{
				auto val = vals->value(prop->name());
				if (val)
				{
					std::clog << prop->name().c_str() << " - - " << prop->property_typeinfo()->info().name() << std::endl;
				}
			}
			
			for (const auto& base : model->bases())
			{
				assert(base.linked);
				auto bVals = vals->base(base.model->name());
				assert(bVals);
				/*if (!save_values(base.model, bVals))
				{
					return false;
				}*/
			}
		}
		
		
		
		//auto v = k->
		return true;
	}
	
};



class cout_prong : public serialize::Prong<>
{
	public:
	
	virtual const reflect::Type* get_type() const override
	{
		return reflect::Type::Get<void>();
	}
	
	virtual bool delete_pointer() override
	{
		return false;
	}
};


class LogPlug : public serialize::Plug<>
{
	public:
	using typename serialize::Plug<>::key_type;
	Utils::map<key_type, serialize::Prong<>*> prongs;
	
	LogPlug()
	{
		_prong = new cout_prong;
	}
	
	serialize::Prong<>* _prong;
	
	virtual serialize::Prong<>* prong(const key_type& k) const override
	{
		return _prong;
	}
};

class test_class
{
	public:
	int x;
	double y;
	int* z;
	
	test_class()
	{
		++_counter;
	}
	
	~test_class()
	{
		--_counter;
	}
	
	private:
	static int _counter;
	
	public:
	static int counter()
	{ return _counter; }
};

int test_class::_counter = 0;

class const_test_class
{
	public:
	const int x;
	const double y;
	int* const z;
	const_test_class(int x, double y, int* z = nullptr) : x(x), y(y), z(z)
	{}
};

class poly_test_class_base
{
	private:
	static int _counter;
	bool move_copied = false;
	public:
	long i, j;
	
	poly_test_class_base()
	{
		++_counter;
	}
	
	poly_test_class_base(const poly_test_class_base& ptc) : i(ptc.i), j(ptc.j)
	{
		++_counter;
	}
	
	poly_test_class_base(poly_test_class_base&& ptc) : i(ptc.i), j(ptc.j)
	{
		ptc.move_copied = true;
		++_counter;
	}
	
	virtual ~poly_test_class_base()
	{
		if (!move_copied)
		{
			--_counter;
		}
	}
	
	virtual void foo() = 0;
	
	
	static int counter()
	{
		return _counter;
	}
};

int poly_test_class_base::_counter = 0;

class poly_test_class : public poly_test_class_base
{
	public:
	long k;
	
	virtual ~poly_test_class() = default;
	
	virtual void foo() {}
};

class poly_test_class2 : public poly_test_class_base
{
	public:
	double z;
	
	virtual ~poly_test_class2() = default;
	
	virtual void foo() {}
};

class test_entity_id : public test_class
{
	public:
	int id;
	
	
};

class test_comp_class
{
	private:
	static int _counter;
	public:
	int id;
	poly_test_class_base* b1;
	test_class* c1;
	
	test_comp_class()
	{
		++_counter;
	}
	
	~test_comp_class()
	{
		--_counter;
	}
	
	
	static int counter()
	{
		return _counter;
	}
};

int test_comp_class::_counter = 0;

class test_self_ref_class
{
	public:
	int id;
	test_comp_class* tcc;
	test_self_ref_class* self;
};

class poly_test_class3 : public poly_test_class
{
	public:
	poly_test_class_base* other_ptc;
	
	virtual ~poly_test_class3() = default;
	
};



template <class L, class R>
struct is_not_base_of : public Utils::integral_constant<bool, !Utils::is_base_of<L, R>::value>
{};

template <class L, class R>
struct is_base_or_derived_of : public Utils::integral_constant<bool, Utils::is_base_of<L, R>::value || Utils::is_base_of<R, L>::value>
{};

template <class L, class R>
struct sort_abstract : public Utils::integral_constant<bool, Utils::is_abstract<L>::value || !Utils::is_abstract<R>::value>
{};

template <class>
struct sort_abstract_select;

template <template <class...> class Seq, class... Types>
struct sort_abstract_select<Seq<Types...>> : public Support::Meta::type_sequence_sort<sort_abstract, Seq<Types...>>
{};

template <class T>
struct select_identity
{
	typedef T type;
};

void second_test();

TEST(reflect)
{
	/*auto t = new reflect::MemberProperty<test_class, int>("", &test_class::x);
	
	assert(t->is_fundamental());
	
	int x1 = 1;
	test_class obj;
	t->set(obj, x1);
	ASSERTEQ(obj.x, 1);
	obj.x = 4;
	t->get(obj, x1);
	ASSERTEQ(x1, 4);
	
	delete t;
	
	reflect::TypedProperty<test_class, int>* iprop;
	
	iprop = new reflect::MemberProperty<test_class, int>("", &test_class::x);
	
	assert(iprop->is_fundamental());
	
	x1 = 1;
	iprop->set(obj, x1);
	ASSERTEQ(obj.x, 1);
	obj.x = 4;
	iprop->get(obj, x1);
	ASSERTEQ(x1, 4);
	
	
	
	
	
	reflect::TypedProperty<test_class, double>* dprop;
	
	dprop = new reflect::MemberProperty<test_class, double>("", &test_class::y);
	
	assert(dprop->is_fundamental());
	
	double y1 = 1;
	dprop->set(obj, y1);
	ASSERTEQ(obj.y, 1);
	obj.y = 4;
	dprop->get(obj, y1);
	ASSERTEQ(y1, 4);
	
	
	delete dprop;
	
	dprop = reflect::getProperty("", &test_class::y);
	
	assert(dprop->is_fundamental());
	
	y1 = 1;
	dprop->set(obj, y1);
	ASSERTEQ(obj.y, 1);
	obj.y = 4;
	dprop->get(obj, y1);
	ASSERTEQ(y1, 4);
	
	delete dprop;
	
	
	reflect::GenericProperty* gprop = reflect::getProperty("", &test_class::y);
	assert(gprop->object_typeinfo()->info() == typeid(test_class));
	
	assert(gprop->is_fundamental());
	
	delete gprop;
	
	
	
	const_test_class cobj(1, 4.2f);
	auto cdprop = reflect::getProperty("", &const_test_class::y);
	
	assert(cdprop->is_fundamental());
	
	cdprop->get(cobj, y1);
	ASSERTEQ(y1, 4.2f);
	
	
	class Pseudo
	{
		int x;
		int y;
		int z;
		double q;
		test_class obj;
		
		public:
		
		static void Model(reflect::ModelBuilder<Pseudo>& m)
		{
			m
			.add("x", &Pseudo::x)
			.add("y", &Pseudo::y)
			.add("z", &Pseudo::z)
			.add("q", &Pseudo::q)
			.add("obj", &Pseudo::obj);
			
			
		}
	};
	
	reflect::TypeSystem ts;
	
	ts.addModel(Pseudo::Model);
	
	
	typedef void(*test_class_builder)(reflect::PropertyModel<test_class>&);
	
	reflect::ModelBuilder_fn<test_class> test_class_model_builder = [](auto& m)
	{
		m.add("x", &test_class::x)
		.add("y", &test_class::y)
		.entity(false);
		
	};
	
	assert(!ts.is_satisfied());
	
	ts.addModel(test_class_model_builder);
	
	assert(ts.is_satisfied());
	
	assert(typeid(int) != typeid(int*));
	
	auto pdprop = reflect::getProperty("", &test_class::z);
	
	assert(pdprop->is_pointer());
	
	auto cpdprop = reflect::getProperty("", &const_test_class::z);
	
	assert(cpdprop->is_pointer());
	assert(cpdprop->is_const());
	assert(!pdprop->is_const());
	assert(!cpdprop->is_volatile());
	assert(!pdprop->is_volatile());
	assert(!cpdprop->is_reference());
	assert(!pdprop->is_reference());
	
	assert(cpdprop->property_typeinfo() != pdprop->property_typeinfo());
	assert(cpdprop->object_typeinfo() != pdprop->object_typeinfo());
	
	std::cout << typeid(int*).name() << std::endl;
	std::cout << typeid(int).name() << std::endl;
	std::cout << typeid(const int*).name() << std::endl;
	std::cout << typeid(const int).name() << std::endl;
	std::cout << typeid(int&).name() << std::endl;
	std::cout << typeid(const int&).name() << std::endl;
	std::cout << typeid(const float).name() << std::endl;
	std::cout << typeid(float).name() << std::endl;
	assert(typeid(int const) == typeid(int));
	
	
	
	delete pdprop;
	delete cpdprop;
	
	
	auto bv1 = new serialize::BasicValue<int>();
	ASSERTEQ(bv1->value(), 0);
	
	auto bv2 = new serialize::BasicValue<int*>();
	ASSERTEQ(bv2->value(), nullptr);
	bv2->value() = nullptr;
	assert(bv2->value_typeinfo() == typeid((int*)nullptr));
	assert(bv2->value_typeinfo() != bv2->info());
	
	bv2->value() = new int(4);
	bool del_result = bv2->delete_pointer();
	assert(del_result);
	ASSERTEQ(bv2->value(), nullptr);
	
	delete bv2;
	delete bv1;
	
	
	
	auto bv3 = new serialize::BasicValue<const int>(4);
	ASSERTEQ(bv3->value(), 4);
	
	bv3->value() = 1;
	assert(bv3->value_typeinfo() == bv3->info());
	
	delete bv3;
	
	
	reflect::ModelBuilder_fn<test_entity_id> test_entity_id_builder = [](auto& b)
	{
		b
		.name("test_entity_id")
		.key("id", &test_entity_id::id)
		.template 
		inherits_from<test_class>()
		.entity();
	};
	
	ts.addModel(test_entity_id_builder);
	
	assert(ts.is_satisfied());
	
	auto itype = reflect::Type::Get<int>();
	assert(itype != nullptr);
	auto itype2 = reflect::Type::Get<int>();
	assert(itype2 != nullptr);
	assert(itype == itype2);
	auto itype3 = reflect::Type::Get<int*>();
	assert(itype3);
	assert((reflect::Type*)itype3 != itype);
	auto itype4 = itype3->under();
	assert(itype4);
	assert(itype4 == itype);
	
	reflect::ModelBuilder_fn<test_comp_class> test_comp_class_builder = [](auto& b)
	{
		b
		.name("test_comp_class")
		.key("id", &test_comp_class::id)
		.add("b1", &test_comp_class::b1)
		.add("c1", &test_comp_class::c1)
		.entity();
	};
	
	ts.addModel(test_comp_class_builder);
	
	assert(!ts.is_satisfied());
	
	
	reflect::ModelBuilder_fn<poly_test_class> poly_test_class_builder = [](auto& b)
	{
		b
		.name("poly_test_class")
		.add("k", &poly_test_class::k)
		.template
		inherits_from<poly_test_class_base>();
	};
	
	ts.addModel(poly_test_class_builder);
	
	assert(!ts.is_satisfied());
	reflect::ModelBuilder_fn<poly_test_class_base> poly_test_class_base_builder = [](auto& b)
	{
		b
		.name("poly_test_class_base")
		.key("i", &poly_test_class_base::i)
		.add("j", &poly_test_class_base::j);
	};
	
	ts.addModel(poly_test_class_base_builder);
	
	assert(ts.is_satisfied());
	
	#ifdef __EXCEPTIONS
	try
	{
		ts.addModel(poly_test_class_base_builder);
		assert(false);
	}
	catch (...)
	{
		
	}
	#endif
	
	
	reflect::ModelBuilder_fn<poly_test_class2> poly_test_class2_builder = [](auto& b)
	{
		b
		.name("poly_test_class2")
		.add("z", &poly_test_class2::z)
		.template
		inherits_from<poly_test_class_base>();
	};
	
	ts.addModel(poly_test_class2_builder);
	assert(ts.is_satisfied());
	
	
	bool linkResult = ts.link();
	assert(linkResult);
	
	
	
	auto ptc_model = ts.get_model<poly_test_class>();
	assert(ptc_model);
	assert(!ptc_model->is_abstract());
	assert(ptc_model->is_polymorphic());
	assert(ptc_model->is_entity());
	assert(ptc_model->get_type());
	assert(ptc_model->get_type()->is_class());
	
	poly_test_class* ptc_ex = new poly_test_class();
	ptc_ex->i = 0;
	ptc_ex->j = 1;
	ptc_ex->k = 2;
	
	auto ptc_vals = serialize::ObjectValueSet<poly_test_class>::Gather(ptc_model, ptc_ex);
	
	ptc_ex->i = 3;
	ptc_ex->j = 4;
	ptc_ex->k = 5;
	
	ptc_vals->restore(ptc_model, ptc_ex);
	ASSERTEQ(ptc_ex->i, 0);
	ASSERTEQ(ptc_ex->j, 1);
	ASSERTEQ(ptc_ex->k, 2);
	
	
	
	delete ptc_vals;
	
	
	delete ptc_ex;
	
	
	
	
	auto tcc_ex = new test_comp_class;
	auto tcc_model = ts.get_model<test_comp_class>();
	
	
	
	
	auto tcc_vals = serialize::ObjectValueSet<test_comp_class>::Gather(tcc_model, tcc_ex);
	
	static_assert(Utils::is_base_of<int, int>::value);
	static_assert(!Utils::is_base_of<int, double>::value);
	
	
	
	DebugContext context;
	
	context.addModel(test_comp_class_builder);
	
	context.addModel(poly_test_class_builder);
	context.addModel(poly_test_class2_builder);
	
	context.addModel(poly_test_class_base_builder);
	
	context.addModel(test_class_model_builder);
	
	assert(context.types().is_satisfied());
	
	
	
	context.link();
	
	
	
	serialize::Collection<poly_test_class_base> ptc_coll(context);
	
	auto ptc_ent = ptc_coll.create<poly_test_class>();
	assert(ptc_ent);
	
	ptc_ent->k = 4;
	
	auto ptc2_ent = ptc_coll.create<poly_test_class2>();
	assert(ptc2_ent);
	
	
	
	serialize::Entity* ent = dynamic_cast<serialize::Entity*>(ptc_ent);
	assert(ent);
	
	auto ent_vals = ent->get_values();
	assert(ent_vals);
	
	ptc_ent->k = 0;
	
	ent->set_values(ent_vals);
	ASSERTEQ(ptc_ent->k, 4);
	
	std::clog << "Saving collection..." << std::endl;
	
	ptc_coll.save();
	
	std::clog << "Saved." << std::endl;*/
	
	/*auto tc_ex = new test_class;
	
	serialize::Plug<test_class> tcp;
	auto tcpp = serialize::Plug<test_class>::From_Model(context.types().get_model<test_class>(), tc_ex);
	assert(tcpp);
	
	const serialize::Adapter<test_class>* adapt;
	__try
	{
		adapt = context.adapter<test_class>();
	}
	__catch(...)
	{
		adapt = nullptr;
	}
	
	assert(adapt);
	
	auto lplug = new LogPlug;
	
	
	
	adapt->connect(lplug, tcpp);*/
	
	/*
	
	//static_assert(Support::SQLite::dfuncer<test_class>::dfunc(&test_class::x).value);
	
	//::Support::SQLite::MemberBuilder<test_class>::get("Hello", &test_class::x);
	
	//std::cout << typeid(decltype(::Support::SQLite::MemberBuilder<test_class>::get("Hello", &test_class::x))).name() << std::endl;
	
	*/
	#if __cplusplus >= 201703L
	{
		#ifndef SQLITE_VFS_NATIVE_FS
		auto db_fs = QA::MountTempFilesystem("/", "DBs", "images/SQLiteDisk.img", 8192*1024);
		assert(db_fs);
		#endif
		
		
	static_assert(Utils::is_same<decltype(&test_class::x), typename Support::SQLite::Member_Pointer<int, test_class>::type>::value);
	
	
	
	Support::SQLite::Member_Ptr<&test_class::x>::Member("x");
	
	constexpr auto mvyvf = Support::SQLite::Member_Ptr<&test_class::x>::Member("x");
	
	test_class tc_ex;
	tc_ex.x = 10;
	
	Utils::unordered_map<Utils::string, int> tc_map;
	
	tc_map["x"] = 4;
	
	mvyvf.store(&tc_ex, tc_map);
	assert(tc_map["x"] == 10);
	
	Utils::make_tuple(4, Support::SQLite::Member_Ptr<&test_class::x>::Member("x"));
	
	second_test();
	
	#ifndef SQLITE_VFS_NATIVE_FS
	bool umount_res = QA::UnmountTempFilesystem("/", "DBs", db_fs);
	assert(umount_res);
	#endif
	}
	#endif
	
	
	std::clog << __func__ << " Exercising memory...\n";
	
	char* c1 = new char;
	delete c1;
	for (int i = 1; i < 16384; ++i)
	{
		c1 = new char[i];
		for (int j = 0; j < i; ++j)
		{
			c1[j] = i;
		}
		delete[] c1;
	}
	
	std::clog << "test_comp_class::counter = " << test_comp_class::counter() << std::endl;
	assert(test_comp_class::counter() == 0);
	
	std::clog << "poly_test_class_base::counter = " << poly_test_class_base::counter() << std::endl;
	assert(poly_test_class_base::counter() == 0);
	
	std::clog << "test_class::counter = " << test_class::counter() << std::endl;
	/*assert(test_class::counter() == 0);*/
	
	std::clog << __func__ << " Returning...\n";
	assert(false);
}
















#if __cplusplus >= 201703L

struct Storage
{
	template <class N, class T>
	void store(N name, T value)
	{
		std::cout << name << " = " << value << std::endl;
	}
};


template <class Type, class Storage, void(*Access)(Storage&, const char*, Type)>
struct GenericStorage
{
	Storage storage;
	void store(const char* name, Type value)
	{
		Access(storage, name, value);
	}
};

template <class MType, class VType>
void map_access(MType& m, const char* name, VType v)
{
	m[name] = v;
}


namespace Support::SQLite
{
	template <>
	class model<test_class>
	{
		
		public:
		
		constexpr static auto members = Utils::make_tuple(MEMBER(y)/* Member_Ptr<&test_class::y>::Member("y")*/);
		
		constexpr static auto Name = "test_class";
		
		constexpr static auto key = Member_Ptr<&test_class::x>::Member("x");
	};
	
	static_assert(Utils::is_same<typename model_has_key<test_class>::key_type, int>::value);
	
	template <>
	class model<poly_test_class_base>
	{
		public:
		
		constexpr static auto Name = "poly_test_class_base";
		
		constexpr static auto Members = Utils::make_tuple(MEMBER(j));
		
		constexpr static auto Key = MEMBER(i);
	};
	
	
	template <>
	class model<test_comp_class>
	{
		public:
		
		constexpr static auto Name = "test_comp_class";
		
		constexpr static auto Members = Utils::make_tuple(MEMBER(c1), MEMBER(b1));
		
		constexpr static auto Key = MEMBER(id);
	};
	
	template <>
	class model<poly_test_class>
	{
		public:
		
		constexpr static auto Name = "poly_test_class";
		
		constexpr static auto Members = Utils::make_tuple(MEMBER(k));
		
		constexpr static auto Bases = base_types<poly_test_class_base>();
		
		
	};
	
	template <>
	class model<test_self_ref_class>
	{
		public:
		
		constexpr static auto Name = "test_self_ref_class";
		
		constexpr static auto Members = Utils::make_tuple(MEMBER(tcc), MEMBER(self));
		
		constexpr static auto Key = MEMBER(id);
	};
	
	template <>
	class model<poly_test_class2>
	{
		public:
		
		constexpr static auto name = "poly_test_class2";
		constexpr static auto members = Utils::make_tuple(MEMBER(z));
		
		constexpr static auto bases = base_types<poly_test_class_base>();
	};
	
	template <>
	class model<poly_test_class3>
	{
		public:
		
		constexpr static auto name = "poly_test_class3";
		constexpr static auto members = Utils::make_tuple(MEMBER(other_ptc));
		
		constexpr static auto bases = base_types<poly_test_class>();
	};
	
	
	
	static_assert(Meta::type_sequence_from<typename detail::BuiltModel<poly_test_class>::Bases>::type::size == 1);


void testt()
{
	auto m = get_members<test_class>(model<test_class>::members);
	
	typedef detail::BuiltModel<test_class> model_t;
	
	std::cout << model_t::name << std::endl;
	
	
	
	test_class tc_ex;
	tc_ex.x = 10;
	tc_ex.y = 4;
	
	typedef GenericStorage<int, Utils::unordered_map<Utils::string, int>, map_access<Utils::unordered_map<Utils::string, int>, int>> MapStorage;
	
	MapStorage tc_map;
	
	tc_map.storage["x"] = 4;
	
	
	
	auto store = detail::BuiltModel<test_class>::store<MapStorage>;
	detail::BuiltModel<test_class>::store<MapStorage>(&tc_ex, tc_map);
	
	store(&tc_ex, tc_map);
	
	assert(tc_map.storage["y"] == 4);
	
	Storage strg;
	detail::BuiltModel<test_class>::store<Storage>(&tc_ex, strg);
	
	assert(detail::BuiltModel<test_class>::get_key(&tc_ex) == &tc_ex.x);
	assert(detail::BuiltModel<test_class>::get_key((const test_class*)&tc_ex) == &tc_ex.x);
	
	
	
	
	
	
	
	SQLiteContext* sqlcontext = new SQLiteContext(sqlite_out_db);
	
	Context<SQLiteContext*, test_class, test_comp_class, poly_test_class, poly_test_class_base, poly_test_class2, poly_test_class3> context(sqlcontext, true);
	
	assert(false);
	
	assert(context.owns_implementation());
	
	sqlcontext->register_type<test_comp_class>();
	sqlcontext->register_type<test_class>();
	
	sqlcontext->register_type<poly_test_class_base>();
	sqlcontext->register_type<poly_test_class>();
	
	assert(sqlcontext->table_exists("test_class"));
	assert(sqlcontext->table_exists("test_comp_class"));
	assert(sqlcontext->get_table_key("test_class") == "x");
	assert(sqlcontext->get_table_key("test_comp_class") == "id");
	
	bool res;
	
	tc_ex.x = 2;
	tc_ex.y = 4;
	
	res = sqlcontext->store(&tc_ex);
	if (!res)
	{
		#ifdef DEBUG_TRACK_QUERY
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
		#endif
		std::cout << "Error Message: '" << sqlcontext->error_msg() << std::endl;
	}
	assert(res);
	assert(false);
	
	
	test_comp_class tcc_ex;
	tcc_ex.id = 1;
	tcc_ex.c1 = nullptr;
	tcc_ex.b1 = nullptr;
	
	res = sqlcontext->store(&tcc_ex);
	if (!res)
	{
		#ifdef DEBUG_TRACK_QUERY
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
		#endif
		std::cout << "Error Message: '" << sqlcontext->error_msg() << std::endl;
	}
	assert(res);
	
	tcc_ex.c1 = &tc_ex;
	res = sqlcontext->store(&tcc_ex);
	assert(res);
	
	tcc_ex.c1 = nullptr;
	res = sqlcontext->store(&tcc_ex);
	assert(res);
	
	poly_test_class* ptc_ex = new poly_test_class;
	ptc_ex->k = 40;
	
	poly_test_class_base* ptcb_ex = ptc_ex;
	
	ptcb_ex->i = 2;
	ptcb_ex->j = 10;
	
	res = sqlcontext->store(ptcb_ex);
	assert(res);
	
	
	tcc_ex.b1 = ptc_ex;
	tcc_ex.c1 = &tc_ex;
	res = sqlcontext->store(&tcc_ex);
	assert(res);
	
	res = sqlcontext->store(ptc_ex);
	assert(res);
	
	
	
	tcc_ex.b1 = nullptr;
	res = sqlcontext->store(&tcc_ex);
	assert(res);
	
	
	tcc_ex.b1 = ptc_ex;
	res = sqlcontext->store(&tcc_ex);
	assert(res);
	
	
	#ifdef DEBUG_TRACK_QUERY
	{
		std::cout << "------------------------------------\n";
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
	}
	#endif
	
	
	
	ptc_ex->i = 3;
	
	
	
	std::cout << tcc_ex.id << "\n" << tcc_ex.b1->i << "\n" << tcc_ex.c1->x << std::endl;
	res = sqlcontext->store(&tcc_ex);
	if (!res)
	{
		#ifdef DEBUG_TRACK_QUERY
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
		#endif
		std::cout << "Error Message: '" << sqlcontext->error_msg() << std::endl;
	}
	assert(res);
	
	
	
	
	
	
	#ifdef DEBUG_TRACK_QUERY
	{
		std::cout << "------------------------------------\n";
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
	}
	#endif
	
	
	
	
	
	auto& tc_set = context.set<test_class>();
	assert(&tc_set == &context.set<const test_class>());
	
	
	auto tc_ex_ptr = new test_class;
	tc_ex_ptr->x = 10;
	tc_ex_ptr->y = 24.8;
	
	tc_set.attach(tc_ex_ptr);
	int rescount = tc_set.save();
	ASSERTEQ(rescount, 1);
	ASSERTEQ(tc_set.size(), 1);
	//ASSERTEQ(tc_set.size(true), 2);
	
	
	
	
	
	#ifdef DEBUG_TRACK_QUERY
	{
		std::cout << "------------------------------------\n";
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
	}
	#endif
	
	
	assert(!sqlcontext->load((test_class*)nullptr));
	
	//debug_print_type<Meta::type_sequence<test_class, poly_test_class, poly_test_class_base, int>::sort<Utils::is_base_of>>();
	
	debug_print_type<detail::BuiltModel<poly_test_class3>::All_Bases>();
	
	debug_print_type<detail::BuiltModel<poly_test_class3>::All_Bases::intersect<Meta::type_sequence<int, poly_test_class_base>>>();
	
	static_assert(Utils::is_base_of<poly_test_class_base, poly_test_class>::value);
	static_assert(Meta::type_sequence<int, poly_test_class, poly_test_class_base, poly_test_class2, int>::any<Utils::is_polymorphic>);
	static_assert(Meta::type_sequence<int, int, poly_test_class2, int>::any<Utils::is_polymorphic>);
	static_assert(Meta::type_sequence<int, int, poly_test_class2>::any<Utils::is_polymorphic>);
	static_assert(Meta::type_sequence<poly_test_class2>::any<Utils::is_polymorphic>);
	static_assert(Meta::type_sequence<poly_test_class2, int, double, test_class>::any<Utils::is_polymorphic>);
	static_assert(!Meta::type_sequence<int, double, test_class>::any<Utils::is_polymorphic>);
	
	static_assert(Meta::type_sequence<int, poly_test_class, poly_test_class_base, poly_test_class2, int>::index_of_type_index(typeid(int)) == 0);
	
	//assert(false);
	
	__try
	{
		std::cout << "Set Local Count: " << tc_set.size() << std::endl;
		ASSERTEQ(tc_set.size(), 1);
		auto loaded_tc = sqlcontext->load<test_class>(2);
		assert(loaded_tc);
		
		bool res = tc_set.lapse(loaded_tc);
		assert(res);
		assert(tc_set.owns(loaded_tc));
		res = tc_set.persist(loaded_tc);
		assert(!tc_set.owns(loaded_tc));
		
		std::cout << "x: '" << loaded_tc->x << "'\ny: '" << loaded_tc->y << "'" << std::endl;
		ASSERTEQ(tc_set.size(), 2);
		
		auto loaded_tc_2 = sqlcontext->load<test_class>(2);
		ASSERTEQ(loaded_tc, loaded_tc_2);
		ASSERTEQ(tc_set.size(), 2);
		
		auto loaded_tc_3 = tc_set(2);
		ASSERTEQ(loaded_tc, loaded_tc_3);
		ASSERTEQ(tc_set.size(), 2);
		
		
		
		auto& ptcb_set = context.set<poly_test_class_base>();
		
		std::clog << ptcb_set.size() << std::endl;
		auto& ptc_set = context.set<poly_test_class>();
		assert(ptc_set.size() == 0);
		
		auto ptc_ex = new poly_test_class;
		ptc_ex->i = 4;
		
		ptc_set.push_back(ptc_ex, true);
		ptc_set.apply_changes();
		assert(ptc_set.size() == 1);
		assert(ptc_set(4) == ptc_ex);
		
		//auto ptcb_ex_2 = sqlcontext->load<poly_test_class_base>(4);
		//assert(dynamic_cast<poly_test_class*>(ptcb_ex_2));
		auto ptcb_ex = ptcb_set(4);
		assert(ptcb_ex->i == ptc_ex->i);
		//assert(ptcb_ex_2->i == ptc_ex->i);
		assert(ptcb_ex == ptc_ex);
		
		ptcb_set.save();
		tc_set.save();
		
		res = ptc_set.unload(ptc_ex);
		assert(res);
		res = tc_set.unload(loaded_tc);
		assert(res);
		
		assert(tc_set(2, false) == nullptr);
		
		//delete ptc_ex;
	}
	__catch(Utils::exception& ex)
	{
		#ifdef DEBUG_TRACK_QUERY
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		#endif
		std::cout << ex.what() << std::endl;
		throw;
	}
	
	#ifdef DEBUG_TRACK_QUERY
	{
		std::cout << "------------------------------------\n";
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
	}
	#endif
	
	{
	Utils::unordered_set<int> uints;
	uints.insert(4);
	assert(uints.count(4) == 1);
	assert(uints.count(5) == 0);
	uints.insert(5);
	assert(uints.count(5) == 1);
	assert(uints.size() == 2);
	
	Utils::unordered_set<void*> ptrss;
	ptrss.insert(&tc_set);
	assert(ptrss.count(&tc_set) == 1);
	assert(ptrss.count(nullptr) == 0);
	ptrss.insert(nullptr);
	assert(ptrss.count(nullptr) == 1);
	assert(ptrss.size() == 2);
	
	
	
	for (auto ptr : ptrss)
	{
		assert(ptr == nullptr || ptr == &tc_set);
	}
	}
	
	
	int existing = tc_set.size();
	ASSERTEQ(tc_set.size(), 1);
	
	int nCount = 10;//000;
	
	auto& tcc_set = context.set<test_comp_class>();
	for (int i = 100; i < nCount + 100 - existing; ++i)
	{
		if (i % 10000 == 0)
		{
			std::cout << "Applying." << std::endl;
			//tc_set.apply_changes();
			tcc_set.apply_changes();
			std::cout << "Applied." << std::endl;
		}
		
		
		
		auto tc = tc_set.create(i);
		tc->y = ((double)i * 5)/2;
		
		auto tcc = tcc_set.create(i/2);
		tcc->c1 = tc;
		tcc->b1 = nullptr;
	}
	
	//ASSERTEQ(tc_set.push(), 2);
	ASSERTEQ(tc_set.size(), nCount);
	//ASSERTEQ(tc_set.save(), nCount);
	
	std::cout << "Final applying..." << std::endl;
	//tc_set.apply_changes();
	tcc_set.apply_changes();
	
	std::cout << "Saved." << std::endl;
	
	//tc_set.save();
	
	#ifdef DEBUG_TRACK_QUERY
	{
		sqlcontext->clear_queries();
	}
	#endif
	
	
	#ifdef DEBUG_TRACK_QUERY
	{
		std::cout << "------------------------------------\n";
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
	}
	#endif
	
	// <int, int, double>
	typedef Meta::type_sequence<int, int, double> S1;
	
	// <int, double>
	typedef typename S1::distinct S2;
	static_assert(S2::size == 2);
	
	// <int>
	typedef typename S2::remove<double> S3;
	static_assert(S3::size == 1);
	static_assert(S3::indexof<int> == 0);
	
	// <double>
	typedef typename S2::remove<int> S4;
	static_assert(S4::size == 1);
	static_assert(S4::indexof<double> == 0);
	
	// <double>
	typedef typename S1::remove_all<int> S5;
	static_assert(S5::size == 1);
	static_assert(S5::indexof<double> == 0);
	
	// <int, double>
	typedef typename S1::remove<int> S1_1;
	// <double>
	typedef typename S1_1::remove<int> S1_2;
	// < >
	typedef typename S1_2::remove<double> S1_3;
	
	static_assert(S1_3::size == 0);
	static_assert(S1_2::size == 1);
	static_assert(S1_1::size == 2);
	
	static_assert(S1_3::same_types<S1_3>);
	static_assert(S1_2::same_types<S1_2>);
	static_assert(S1_1::same_types<S1_1>);
	static_assert(S1::same_types<S1>);
	
	// <int, double, int>
	typedef typename S2::concat<S3> S6;
	static_assert(S6::same_types<S1>);
	
	
	// <int, double> != <int, int, double>
	static_assert(!S6::distinct::same_types<S1>);
	
	// <int, double> == <int, double>
	static_assert(S6::distinct::same_types<S1::distinct>);
	
	
	typedef typename  detail::all_related_entity_types<test_self_ref_class>::type S7;
	
	debug_print_type<S7>();
	
	
	
	
	sqlcontext->register_type<test_self_ref_class>();
	
	
	
	#ifdef DEBUG_TRACK_QUERY
	{
		std::cout << "------------------------------------\n";
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
	}
	#endif
	
	
	
	{
	
	auto tsrc_ex = new test_self_ref_class;
	tsrc_ex->self = tsrc_ex;
	tsrc_ex->tcc = new test_comp_class;
	tsrc_ex->tcc->c1 = new test_class;
	tsrc_ex->tcc->b1 = nullptr;
	
	const int traverse_count = 10000;
	
	auto tmp = tsrc_ex;
	for (int i = 0; i < traverse_count; ++i)
	{
		tmp->self = new test_self_ref_class;
		tmp = tmp->self;
		tmp->tcc = new test_comp_class;
		tmp->tcc->b1 = nullptr;
		tmp->tcc->c1 = tsrc_ex->tcc->c1;
		tmp->self = new test_self_ref_class;
		tmp->self->tcc = tmp->tcc;
		tmp = tmp->self;
	}
	
	tmp->self = tsrc_ex;
	
	
	Traverser trav;
	std::clog << "\n\n\n\nTraversing!\n";
	auto sets1 = trav.traverse(tsrc_ex);
	std::clog << "Traversed!\n\n\n\n\n";
	
	{
		assert(sets1);
		auto& set = sets1->set<test_self_ref_class>();
		assert(!set.set.count(nullptr));
		assert(set.set.size() == 2*traverse_count + 1);
		
	}
	
	{
		assert(sets1);
		auto& set = sets1->set<test_comp_class>();
		assert(!set.set.count(nullptr));
		assert(set.set.size() == traverse_count + 1);
		
	}
	
	{
		assert(sets1);
		auto& set = sets1->set<poly_test_class_base>();
		assert(!set.set.count(nullptr));
		assert(set.set.size() == 0);
	}
	
	{
		assert(sets1);
		auto& set = sets1->set<test_class>();
		assert(!set.set.count(nullptr));
		assert(set.set.size() == 1);
	}
	
	Utils::function<void(test_self_ref_class*)> deleter;
	deleter = [&deleter, tsrc_ex](auto ptr)
	{
		if (ptr->self)
		{
			if (ptr->self == ptr || ptr->self == tsrc_ex)
			{
				return;
			}
			if (ptr->self->tcc == ptr->tcc)
			{
				ptr->self->tcc = nullptr;
			}
			deleter(ptr->self);
			delete ptr->self;
			ptr->self = nullptr;
		}
		
		if (ptr->tcc)
		{
			delete ptr->tcc;
			ptr->tcc = nullptr;
		}
	};
	
	deleter(tsrc_ex->self);
	
	delete tsrc_ex->tcc->c1;
	delete tsrc_ex->tcc;
	delete tsrc_ex;
	
	}
	
	
	
	
	
	
	
	
	
	// Cleanup
	
	delete tc_ex_ptr;
	delete ptc_ex;
	
	#ifdef DEBUG_TRACK_QUERY
	{
		std::cout << "------------------------------------\n";
		auto str = sqlcontext->dump_queries();
		std::cout << str.c_str() << std::endl;
		std::cout << "------------------------------------\n";
		sqlcontext->clear_queries();
	}
	#endif
	
	std::clog << "Exiting test...\n";
}

}

void second_test()
{
	Support::SQLite::testt();
}

static_assert(Support::SQLite::has_model<test_class>::value);

#endif