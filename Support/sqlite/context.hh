#if __cplusplus >= 201703L
#ifndef INCLUDED_SUPPORT_SQLITE_CONTEXT_HH
#define INCLUDED_SUPPORT_SQLITE_CONTEXT_HH

#include <Common.h>
#include <Utils/string>
#include <Utils/memory>
#include <Utils/list>
#include <Utils/unordered_set>
#include <Utils/utility>
#include <Support/MetaProgramming/Utilities.hh>
#include <Utils/typeindex>
#include <Support/events/EventHandler.hh>

#include "sqlite_type.hh"
#include "serializer.hh"
#include "schema.hh"
#include "DbSet.hh"
#include "entity_traversal.hh"
#include "EntityFactory.hh"
#include "Reader.hh"

#include <iostream>

extern "C" {
	
	struct sqlite3;
	struct sqlite3_stmt;
	
	int sqlite3_bind_parameter_index(sqlite3_stmt*, const char*);
	int sqlite3_bind_parameter_count(sqlite3_stmt*);
	const char* sqlite3_bind_parameter_name(sqlite3_stmt*, int);
}


namespace Support { namespace SQLite
{
	
	class SQLiteContext
	{
		protected:
		struct column_def
		{
			bool PK;
			Utils::string name;
			bool nullable;
			bool FK;
			sqlite_type value_type;
			Utils::string FK_table;
		};
		
		
		public:
		struct MetaTables
		{
			constexpr static auto& Inheritance = "__Meta_Inheritance";
		};
		
		
		protected:
		typedef Utils::tuple<int, Utils::string, sqlite_type, bool, Utils::string, bool> table_info_row;
		typedef Utils::shared_ptr<sqlite3_stmt> stmt_ptr;
		
		Utils::shared_ptr<sqlite3> db;
		Utils::map<Utils::string, schema> schemas;
		mutable Utils::map<Utils::type_index, Utils::shared_ptr<sqlite3_stmt>> size_statements;
		mutable Utils::map<Utils::type_index, events::EventHandler<void*, const char*, const void*>> loaded_entity_handlers;
		mutable Utils::map<Utils::type_index, Utils::function<void(void**, const char*, const void*)>> loading_entity_handlers;
		long trans_count = 0;
		Utils::map<Utils::string, Utils::type_index*> type_index_by_table;
		
		
		
		// Prepared Statements
		struct Prepared_Statements
		{
			stmt_ptr update_inheritance_1;
			stmt_ptr update_inheritance_2;
			stmt_ptr resolve_type;
		};
		
		Prepared_Statements prepared;
		
		
		
		
		
		void build_table(const Utils::string& name, Utils::list<column_def>& columns, const Utils::string& key_base = "", bool is_abstract = false);
		
		Utils::list<table_info_row> table_info(const Utils::string&) const;
		
		Utils::list<Utils::string> get_tables() const;
		
		void initialize_meta();
		
		void update_inheritance_hierarchy(const char* derived, const char* base, bool abstract);
		
		template <class Derived, class Base>
		void add_inheritance();
		
		template <class Derived>
		struct __inheritance_add_helper
		{
			template <class Base>
			struct inner
			{
				void operator()(SQLiteContext* c)
				{
					c->add_inheritance<Derived, Base>();
				}
			};
		};
		
		
		template <class T>
		void on_loaded_entity(T* t)
		{
			loaded_entity_handlers[typeid(T)]((void*)t, detail::BuiltModel<T>::name, model_has_key<T>::get_key(t));
		}
		
		
		
		
		static void sqlite_delete(sqlite3*) noexcept;
		static Utils::shared_ptr<sqlite3_stmt> Prepare(Utils::shared_ptr<sqlite3>, const Utils::string&);
		template <sqlite_type type>
		static bool Bind(Utils::shared_ptr<sqlite3_stmt>, int index, typename type_from_enum<type>::pass_type);
		static bool BindNull(Utils::shared_ptr<sqlite3_stmt>, int index);
		static bool Unbind(stmt_ptr);
		static bool ExecuteNonQuery(Utils::shared_ptr<sqlite3_stmt>);
		static bool ExecuteNonQuery(Utils::shared_ptr<sqlite3> db, const Utils::string& query, int* res = nullptr);
		static int ExecuteScalarInt(Utils::shared_ptr<sqlite3_stmt>);
		static bool StepQuery(Utils::shared_ptr<sqlite3_stmt>);
		static bool Reset(stmt_ptr);
		template <class Model>
		static void add_columns_subtype(SQLiteContext*, Utils::list<column_def>* columns, const Utils::string& supername, bool nullable);
		static int get_parameter_index(Utils::shared_ptr<sqlite3_stmt>, const char* name) noexcept;
		static int get_parameter_index(Utils::shared_ptr<sqlite3_stmt> stmt, const Utils::string& name) noexcept
		{
			return get_parameter_index(stmt, name.c_str());
		}
		static int get_column_index(Utils::shared_ptr<sqlite3_stmt>, const char* name);
		
		template <class Y>
		static bool get_stmt_value(Utils::shared_ptr<sqlite3_stmt> stmt, int index, Y& value)
		{
			typedef typename type_from_enum<enum_type<Y>::value>::type Y2;
			value = get_stmt_value<Y2>(stmt, index);
			return true;
		}
		
		template <class Y>
		static Y get_stmt_value(Utils::shared_ptr<sqlite3_stmt>, int index);
		
		public:
		
		SQLiteContext(const Utils::string& database);
		~SQLiteContext();
		
		bool table_exists(const Utils::string&) const;
		Utils::string get_table_key(const Utils::string& table) const;
		Utils::string dump_table(const Utils::string&) const noexcept;
		const char* error_msg() const noexcept;
		bool start_transaction()
		{
			trans_count++;
			if (trans_count == 1)
			{
				return ExecuteNonQuery(db, "BEGIN TRANSACTION;");
			}
			return true;
		}
		bool end_transaction()
		{
			if (trans_count == 1)
			{
				
				if (ExecuteNonQuery(db, "END TRANSACTION;"))
				{
					--trans_count;
					return true;
				}
				else
				{
					return false;
				}
			}
			else if (trans_count > 0)
			{
				--trans_count;
			}
			return true;
		}
		
		bool rollback()
		{
			if (trans_count == 1)
			{
				if (ExecuteNonQuery(db, "ROLLBACK"))
				{
					--trans_count;
					return true;
				}
				else
				{
					return false;
				}
			}
			else if (trans_count > 0)
			{
				--trans_count;
				return true;
			}
			return true;
		}
		
		template <class T>
		bool register_loaded_listener(auto fn)
		{
			Utils::function<void(void*, const char*, const void*)> new_fn = [fn](void* ent, const char* tname, const void* key_value)
			{
				fn((T*)ent, tname, (const typename detail::BuiltModel<T>::key_type*)key_value);
			};
			
			loaded_entity_handlers[typeid(T)] += new_fn;
			return true;
		}
		
		template <class T>
		bool register_loading_handler(const Utils::function<void(T**, const char*, const typename detail::BuiltModel<T>::key_type*)>& fn)
		{
			Utils::function<void(void**, const char*, const void*)> new_fn = [fn](void** ent, const char* tname, const void* key_value)
			{
				fn((T**)ent, tname, (const typename detail::BuiltModel<T>::key_type*)key_value);
			};
			
			loading_entity_handlers[typeid(T)] = new_fn;
			return true;
		}
		
		#ifdef DEBUG_TRACK_QUERY
		Utils::string dump_queries() const;
		void clear_queries();
		#endif
		
		
		enum class table_context_type : bool
		{
			store = false,
			load = true
		};
		
		
		template <class T>
		struct table_store_context
		{
			SQLiteContext& context;
			Utils::shared_ptr<sqlite3_stmt> store_stmt;
			
			
			table_store_context(SQLiteContext& c, Utils::shared_ptr<sqlite3_stmt> store_stmt) : context(c), store_stmt(store_stmt)
			{
				
			}
			
			template <class Y>
			auto store(Y* obj)
			{
				return context.store(obj);
			}
			
			template <class Y>
			void store(const char* name, Y value)
			{
				int index = get_parameter_index(store_stmt, name);
				assert(index > 0);
				
				bool res = Bind<enum_type<Y>::value>(store_stmt, index, value);
				assert(res);
			}
			
			void store(const char* name, nullptr_t)
			{
				int index = get_parameter_index(store_stmt, name);
				assert(index > 0);
				
				bool res = BindNull(store_stmt, index);
				assert(res);
			}
			
			bool execute() noexcept
			{
				auto res = context.start_transaction();
				if (!res)
				{
					return false;
				}
				assert(res);
				res = ExecuteNonQuery(store_stmt);
				if (res)
				{
					res = context.end_transaction();
				}
				
				if (!res)
				{
					context.rollback();
				}
				
				return res;
			}
		};
		
		
		
		template <class T, class Entities_Tuple_t = typename detail::all_related_entity_types<T>::type::template apply_transforms<Utils::add_pointer>::template apply_templates<Utils::unordered_set>::template apply_template<Utils::tuple>>
		struct table_bulk_store_context
		{
			private:
			typedef Entities_Tuple_t entity_set_tuple;
			
			template <class Y>
			struct value_type_extractor
			{
				typedef typename Y::value_type type;
			};
			
			typedef typename Meta::type_sequence_from<entity_set_tuple>::type::template apply_transforms<value_type_extractor>::template apply_transforms<Utils::remove_pointer> tuple_type_seq;
			
			typedef typename tuple_type_seq::template remove<T> tuple_type_seq_without_T;
			
			template <class Y>
			struct store_sub_invoker
			{
				void operator()(SQLiteContext& c, entity_set_tuple& tup) const
				{
					
					c.store<Y>(tup);
				}
			};
			
			
			public:
			SQLiteContext& context;
			Utils::shared_ptr<sqlite3_stmt> store_stmt;
			const int count;
			int index;
			entity_set_tuple related_entities;
			
			table_bulk_store_context(SQLiteContext& c, Utils::shared_ptr<sqlite3_stmt> store_stmt, int count) : context(c), store_stmt(store_stmt), count(count), index(0)
			{
				assert(count > 0);
			}
			
			template <class Y>
			auto store(Y* obj)
			{
				constexpr auto index = tuple_type_seq::template indexof<Y>::value;
				Utils::get<index>(related_entities).insert(obj);
				return true;
				return context.store(obj);
			}
			
			template <class Y>
			void store(const char* name, Y value)
			{
				int index = get_parameter_index(store_stmt, Utils::string(name) + Utils::to_string(index));
				assert(index > 0);
				
				bool res = Bind<enum_type<Y>::value>(store_stmt, index, value);
				assert(res);
			}
			
			void store(const char* name, nullptr_t)
			{
				int index = get_parameter_index(store_stmt, Utils::string(name) + Utils::to_string(index));
				assert(index > 0);
				
				bool res = BindNull(store_stmt, index);
				assert(res);
			}
			
			bool execute() noexcept
			{
				auto res = context.start_transaction();
				if (!res)
				{
					return false;
				}
				assert(res);
				
				
				tuple_type_seq_without_T::template for_each<store_sub_invoker>(context, related_entities);
				
				res = ExecuteNonQuery(store_stmt);
				if (res)
				{
					res = context.end_transaction();
				}
				
				if (!res)
				{
					context.rollback();
				}
				
				return res;
			}
		};
		
		
		
		
		
		
		
		
		template <class T>
		struct table_load_context
		{
			SQLiteContext& context;
			Utils::shared_ptr<sqlite3_stmt> load_stmt;
			
			table_load_context(SQLiteContext& c, Utils::shared_ptr<sqlite3_stmt> load_stmt) : context(c), load_stmt(load_stmt)
			{
				
			}
			
			template <class Y>
			auto load(const typename model_has_key<Y>::key_type& key)
			{
				return context.load<Y>(key);
			}
			
			template <class Y>
			void load(const char* name, Y& value)
			{
				int index = get_column_index(load_stmt, name);
				assert(index >= 0);
				
				auto res = get_stmt_value<Y>(load_stmt, index, value);
				assert(res);
			}
			
			bool execute(const typename model_has_key<T>::key_type& k) noexcept
			{
				constexpr static auto etype = enum_type<typename model_has_key<T>::key_type>::value;
				if (Bind<etype>(load_stmt, 1, k))
				{
					return StepQuery(load_stmt);
				}
				
				assert(false);
				
				return false;
			}
		};
		
		template <class T>
		struct table_context_factory
		{
			private:
			Utils::string store_query;
			Utils::string load_query;
			
			public:
			
			constexpr table_context_factory(SQLiteContext* base)
			{
				typedef detail::BuiltModel<T> Model;
				assert(base->schemas.count(Model::name) == 1);
				auto& schema = base->schemas.at(Model::name);
				bool comma = false;
				/*query = "INSERT OR REPLACE INTO [";
				query += Model::name;
				query += "] (";
				
				for (const auto& col : schema.columns)
				{
					if (comma)
					{
						query += ", ";
					}
					query += "[" + col + "]";
					comma = true;
				}
				query += ") VALUES ((SELECT [";
				query += schema.key_name;
				query += "] FROM [";
				query += Model::name;
				query += "] WHERE [";
				query += schema.key_name;
				query += "] = @";
				query += schema.key_name;
				query += ")";
				
				for (const auto& col : schema.columns)
				{
					query += ", @" + col;
				}
				
				query += ");";*/
				
				store_query = "INSERT OR REPLACE INTO [";
				store_query += Model::name;
				store_query += "] (";
				for (const auto& col : schema.columns)
				{
					if (comma)
					{
						store_query += ", ";
					}
					store_query += "[" + col + "]";
					comma = true;
				}
				
				store_query += ") VALUES (";
				comma = false;
				
				for (const auto& col : schema.columns)
				{
					if (comma)
					{
						store_query += ", ";
					}
					store_query += "@" + col;
					comma = true;
				}
				store_query += ");";
				
				
				comma = false;
				load_query = "SELECT * FROM [";
				load_query += Model::name;
				load_query += "] WHERE [";
				load_query += schema.key_name;
				load_query += "] = @";
				load_query += schema.key_name + " LIMIT 1";
			}
			
			table_store_context<T> storing(SQLiteContext* c) const
			{
				return table_store_context<T>(*c, Prepare(c->db, store_query));
			}
			
			table_load_context<T> loading(SQLiteContext* c) const
			{
				return table_load_context<T>(*c, Prepare(c->db, load_query));
			}
		};
		
		template <class T>
		table_context_factory<T>& get_factory()
		{
			#ifdef ENABLED_THREAD_LOCAL
			thread_local
			#endif
			static
			table_context_factory<T> factory(this);
			
			static_assert(sizeof(factory) == 16);
			
			return factory;
		}
		
		
		template <class T>
		bool store(T* object)
		{
			auto& factory = get_factory<T>();
			static_assert(is_entity<T>::value);
			typedef detail::BuiltModel<T> Model;
			
			auto table = factory.storing(this);
			
			constexpr auto str = Model::template store<table_store_context<T>>;
			
			str(object, table);
			
			return table.execute();
		}
		
		template <class T>
		bool store(const EntityProxy<T>* ent)
		{
			auto& factory = get_factory<T>();
			static_assert(is_entity<T>::value);
			typedef detail::BuiltModel<T> Model;
			
			auto table = factory.storing(this);
			
			ent->store_context(table);
			
			return table.execute();
		}
		
		template <class It>
		bool store(It start, It end)
		{
			auto count = Utils::distance(start, end);
			
			if (count < 100)
			{
				bool res = true;
				for (auto it = start; it != end && res; ++it)
				{
					res &= store(*it);
				}
				return res;
			}
			
			typedef typename Utils::remove_pointer<typename Utils::decay<decltype(*start)>::type>::type T;
			auto& factory = get_factory<T>();
			static_assert(is_entity<T>::value);
			typedef detail::BuiltModel<T> Model;
			
			auto table = factory.storing(this, count);
			
			constexpr auto str = Model::template store<table_bulk_store_context<T>>;
			
			for (auto it = start; it != end; ++it)
			{
				str(*it, table);
				++table.index;
			}
			
			return table.execute();
		}
		
		template <class Y, class... T>
		bool store(Utils::tuple<T...>& sets)
		{
			constexpr size_t index = Meta::type_sequence<T...>::template indexof<Y>::value;
			
			static_assert(index != Meta::type_sequence<T...>::npos);
			
			auto& set = Utils::get<index>(sets);
			size_t count = set.size();
			
			assert(NOT_IMPLEMENTED);
		}
		
		template <class T>
		T* load(const typename model_has_key<T>::key_type& k)
		{
			auto& factory = get_factory<T>();
			static_assert(is_entity<T>::value);
			typedef detail::BuiltModel<T> Model;
			
			auto table = factory.loading(this);
			
			if (!table.execute(k))
			{
				return nullptr;
			}
			
			constexpr auto ld = Model::template load<table_load_context<T>>;
			
			T* object = nullptr;
			if (loading_entity_handlers[typeid(T)])
			{
				void* _obj = nullptr;
				loading_entity_handlers[typeid(T)](&_obj, Model::name, &k);
				object = (T*)_obj;
			}
			if (!object)
			{
				/*auto factory = dynamic_cast<EntityFactory<T, SQLiteContext>*>(factories[Model::name]);
				if (!Utils::is_polymorphic<T>::value && factory)
				{
					
					object = factory->make();
				}
				else
				{
					assert(NOT_IMPLEMENTED);
				}
				//object = new T;
				*/
			}
			assert(object);
			
			ld(object, table);
			
			on_loaded_entity<T>(object);
			
			return object;
		}
		
		template <class T>
		bool load(const typename model_has_key<T>::key_type& k, T*& object)
		{
			auto& factory = get_factory<T>();
			static_assert(is_entity<T>::value);
			typedef detail::BuiltModel<T> Model;
			
			auto table = factory.loading(this);
			
			if (!table.execute(k))
			{
				return false;
			}
			
			constexpr auto ld = Model::template load<table_load_context<T>>;
			
			if (!object)
			{
				if (loading_entity_handlers[typeid(T)])
				{
					void* _obj = nullptr;
					loading_entity_handlers[typeid(T)](&_obj, Model::name, &k);
					object = (T*)_obj;
				}
				if (!object)
				{
					object = new T;
				}
			}
			
			ld(object, table);
			
			on_loaded_entity<T>(object);
			
			return true;
		}
		
		template <class T>
		bool load(T* object)
		{
			if (!object)
			{
				return false;
			}
			else
			{
				return load(*model_has_key<T>::get_key(object), object);
			}
		}
		
		template <class T>
		size_t size() const
		{
			static_assert(is_entity<T>::value);
			
			auto stmt = size_statements[typeid(T)];
			if (!stmt)
			{
				stmt = size_statements[typeid(T)] = Prepare(db, Utils::string("SELECT COUNT(*) FROM [") + detail::BuiltModel<T>::name + "];");
			}
			
			return ExecuteScalarInt(stmt);
		}
		
		
		
		template <class T>
		Utils::type_index resolve_type(const typename detail::BuiltModel<typename Utils::decay<T>::type>::key_type& key)
		{
			typedef typename detail::BuiltModel<typename Utils::decay<T>::type>::key_type key_type;
			if (!prepared.resolve_type)
			{
				prepared.resolve_type = Prepare(db, Utils::string("SELECT [derived] FROM [") + MetaTables::Inheritance + "] WHERE [base] = @BASE AND [distance] >= @DIST AND [abstract] = 0 ORDER BY [distance] DESC;");
			}
			assert(prepared.resolve_type);
			
			int base_index = get_parameter_index(prepared.resolve_type, "BASE");
			assert(base_index > 0);
			
			int dist_index = get_parameter_index(prepared.resolve_type, "DIST");
			assert(dist_index > 0);
			
			Bind<Text>(prepared.resolve_type, base_index, detail::BuiltModel<T>::name);
			Bind<Integer>(prepared.resolve_type, dist_index, 1);
			
			Utils::type_index* index = nullptr;
			
			{
			SQLiteReader<const char*> reader(prepared.resolve_type);
			
			while (reader.next())
			{
				auto row = reader.row();
				std::clog << Utils::get<0>(row) << "\n";
				
				// TODO
				if (!type_index_by_table[Utils::get<0>(row)])
				{
					continue;
				}
				
				auto stmt = Prepare(db, Utils::string("SELECT COUNT(*) FROM [") + Utils::get<0>(row) + "] WHERE [" + schemas[Utils::get<0>(row)].key_name + "] = @KEY;");
				Bind<enum_type<key_type>::value>(stmt, 1, key);
				if (ExecuteScalarInt(stmt) > 0)
				{
					index = type_index_by_table[Utils::get<0>(row)];
					break;
				}
				
			}
			
			}
			
			Reset(prepared.resolve_type);
			Unbind(prepared.resolve_type);
			
			if (index)
			{
				return *index;
			}
			else
			{
				return typeid(void);
			}
			
			assert(index);
			assert(NOT_IMPLEMENTED);
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		template <class T>
		void register_type()
		{
			typedef detail::BuiltModel<T> BM;
			TRACE("Called.");
			TRACE_VAL(BM::name);
			if (!type_index_by_table[BM::name])
			{
				type_index_by_table[BM::name] = new Utils::type_index(typeid(T));
			}
			if (table_exists(BM::name))
			{
				return;
			}
			Utils::list<column_def> columns;
			
			static_assert(has_model<T>::value);
			static_assert(is_entity<T>::value);
			
			static_assert(model_has_key<T>::value);
			
			constexpr auto evaluator = [](auto context, auto columns, const auto member)
			{
				typedef decltype(member) MT;
				
				typedef typename Utils::remove_pointer<typename MT::member_type>::type column_type;
				
				
				column_def def;
				def.PK = false;
				def.FK = false;
				def.name = member.name;
				def.nullable = Utils::is_pointer<typename MT::member_type>::value;
				if constexpr (is_entity<typename Utils::remove_pointer<typename MT::member_type>::type>::value)
				{
					def.FK = true;
					
					typedef typename Utils::remove_pointer<typename MT::member_type>::type FK_Ent_type;
					
					 def.FK_table = detail::BuiltModel<FK_Ent_type>::name;
					 
					 if constexpr (!Utils::is_same<FK_Ent_type, T>::value)
					 {
					 	context->template register_type<FK_Ent_type>();
					 }
				}
				else if constexpr (has_model<column_type>::value && ! is_primitive<column_type>::value)
				{
					typedef detail::BuiltModel<typename Utils::remove_pointer<typename MT::member_type>::type> SubModel;
					SQLiteContext::add_columns_subtype<SubModel>(context, columns, def.name, def.nullable);
					return;
				}
				def.value_type = enum_type<typename MT::member_type>::value;
				columns->push_back(def);
				
				// TODO: FK
			};
			
			TRACE("Evaluating for columns...");
			BM::members.eval_for_each(evaluator, this, &columns);
			
			Utils::string key_base = "";
			
			if constexpr (BM::has_key)
			{
				auto sz = columns.size();
				evaluator(this, &columns, model_has_key<T>::key);
				if constexpr (!Utils::is_same<typename BM::type, typename decltype(model_has_key<T>::key)::object_type>::value)
				{
					typedef typename decltype(model_has_key<T>::key)::object_type key_base_t;
					register_type<key_base_t>();
					key_base = detail::BuiltModel<key_base_t>::name;
				}
				if (sz > 0)
				{
				if (columns.size() > sz)
				{
				auto it = columns.begin();
				for (size_t i = 0; i < sz; ++i, ++it)
				{
					
				}
				auto end = columns.end();
				while (it != end)
				{
					(*it).PK = true;
					++it;
				}
				}
				}
				else
				{
					for (auto& col : columns)
					{
						col.PK = true;
					}
				}
				
			}
			
			build_table(BM::name, columns, key_base, Utils::is_abstract<T>::value);
			
			/*assert(factories.count(BM::name) == 0);
			if constexpr (!Utils::is_abstract<T>::value)
			{
				factories[BM::name] = new EntityFactory<T, SQLiteContext>;
			}*/
			
			
			typedef typename Meta::type_sequence_from<typename detail::BuiltModel<T>::Bases>::type base_seq;
			base_seq::template for_each<__inheritance_add_helper<T>::template inner>(this);
		}
		
		
	};
	
	namespace detail
	{
		template <class T1, class T2>
		struct type_sort_predicate_less : public Utils::integral_constant<bool, Utils::is_base_of<T1, T2>::value>
		{
			
		};
		
		
		
		template <class T>
		struct context_create;
		
		template <class T>
		struct context_create
		{
			typedef T result_type;
			typedef T& reference_type;
			
			template <class... Args>
			static constexpr result_type get(Args... args)
			{
				return T(Utils::forward<Args>(args)...);
			}
			
			__attribute__((__always_inline__))
			constexpr static bool release(T&) noexcept
			{
				return false;
			}
			
			__attribute__((__always_inline__))
			constexpr static reference_type to_reference(T& t) noexcept
			{
				return t;
			}
		};
		
		template <class T>
		struct context_create<T*>
		{
			typedef T* result_type;
			typedef T& reference_type;
			
			template <class... Args>
			static constexpr result_type get(Args... args)
			{
				return new T(Utils::forward<Args>(args)...);
			}
			
			__attribute__((__always_inline__))
			constexpr static bool release(T*& ptr) noexcept
			{
				if (ptr)
				{
					delete ptr;
					ptr = nullptr;
					return true;
				}
				else
				{
					return false;
				}
			}
			
			__attribute__((__always_inline__))
			constexpr static reference_type to_reference(T* t) noexcept
			{
				return *t;
			}
		};
		
		
		template <class T, auto fn>
		struct inline_delegate
		{
			T& t;
			
			constexpr inline_delegate(T& t) : t(t)
			{}
			
			auto operator()(auto... x) const
			{
				return (t.*fn)(x...);
			}
		};
		
		template <class L, class R>
		struct is_base_or_derived_of : public Utils::integral_constant<bool, Utils::is_base_of<L, R>::value || Utils::is_base_of<R, L>::value>
		{};
		
		template <class L, class R>
		struct sort_polymorphic : public Utils::integral_constant<bool, Utils::is_polymorphic<L>::value || !Utils::is_polymorphic<R>::value>
		{};
		
		template <class L, class R>
		struct sort_abstract : public Utils::integral_constant<bool, Utils::is_abstract<L>::value || !Utils::is_abstract<R>::value>
		{};
		
		template <class L, class R>
		struct sort_inherit : public Utils::integral_constant<bool, Utils::is_base_of<L, R>::value || !Utils::is_base_of<R, L>::value>
		{};
		
		
		
		template <class>
		struct sort_polymorphic_select;
		
		template <template <class...> class Seq, class... Types>
		struct sort_polymorphic_select<Seq<Types...>> : public Support::Meta::type_sequence_sort<sort_polymorphic, Seq<Types...>>
		{};
		
		template <class>
		struct sort_abstract_select;
		
		template <template <class...> class Seq, class... Types>
		struct sort_abstract_select<Seq<Types...>> : public Support::Meta::type_sequence_sort<sort_abstract, Seq<Types...>>
		{};
		
		template <template <class...> class Sort>
		struct sort_select
		{
			template <class T>
			struct inner : public Support::Meta::type_sequence_sort<Sort, T>
			{};
		};
		
		template <class T>
		struct select_identity
		{
			typedef T type;
		};
		
		
		/*template <class... Types>
		struct reorder_types
		{
			typedef typename Meta::type_sequence<typename Utils::decay<Types>::type...>::template group_by<is_base_or_derived_of>::template select<sort_polymorphic_select>::template select<sort_abstract_select>::template select_many<select_identity> type;
		};*/
		
		template <class... Types>
		struct reorder_types
		{
			typedef typename Meta::type_sequence<typename Utils::decay<Types>::type...>::template group_by<is_base_or_derived_of>::template select<sort_select<sort_polymorphic>::inner>::template select<sort_select<sort_inherit>::inner>::template select<sort_select<sort_abstract>::inner>::template select<sort_select<sort_inherit>::inner>::template select_many<select_identity> type;
		};
		
		
		
		template <class T>
		struct create_dbset_forward
		{
			auto operator()(auto&&... x)
			{
				return create_dbset<T>(x...);
			}
		};
	}
	
	template <class Impl, class Types>
	class _Context;
	
	template <class Impl, template <class...> class S, class... Types>
	class _Context<Impl, S<Types...>>
	{
		public:
		typedef typename Utils::remove_pointer<Impl>::type type;
		
		typedef typename Meta::type_sequence_from<typename detail::reorder_types<Types...>::type>::type entities_type;
		typedef typename entities_type::template select<detail::DbSet_Filter> set_types;
		typedef typename set_types::template apply_template<Utils::tuple> sets_tuple_type;
		
		static_assert(Meta::type_sequence_equal<entities_type, S<Types...>>::value);
		
		protected:
		Impl impl;
		sets_tuple_type _sets;
		bool owned;
		Utils::map<Utils::type_index, BaseEntityFactory<type>*> factories;
		
		
		constexpr type& get_imp() noexcept
		{
			if constexpr (Utils::is_pointer<Impl>::value)
			{
				return *impl;
			}
			else
			{
				return impl;
			}
		}
		
		template <class T>
		void entity_loaded(T* ent, const char* tname, const typename detail::BuiltModel<T>::key_type* key)
		{
			set<T>().attach(ent);
			assert(set<T>().count(ent) == 1);
		}
		
		template <class T>
		void entity_loading(T** ent, const char* tname, const typename detail::BuiltModel<T>::key_type* key)
		{
			assert(ent);
			assert(key);
			auto& set = this->set<T>();
			auto ptr = set(*key, false);
			if (!ptr)
			{
				std::clog << "tname = \"" << tname << "\"\n";
				if constexpr (Utils::is_polymorphic<T>::value)
				{
					auto tindex = get_imp().template resolve_type<T>(*key);
					if (factories.count(tindex))
					{
						auto factory = dynamic_cast<EntityFactory<T, type>*>(factories.at(tindex));
						assert(factory);
						ptr = factory->make_proxy(get_imp());
					}
				}
				else
				{
					ptr = set.create_local(*key);
				}
				assert(ptr);
			}
			*ent = ptr;
		}
		
		public:
		
		constexpr _Context(typename detail::context_create<Impl>::result_type c, bool owned = false) : impl(Utils::forward<typename detail::context_create<Impl>::result_type>(c)), _sets(create_dbset<Types>(detail::context_create<Impl>::to_reference(impl), &_sets)...), owned(!Utils::is_pointer<Impl>::value || owned)
		{
			TRACE("Members initialized.");
			auto& imp = get_imp();
			
			Utils::make_tuple(
			imp.template register_loaded_listener<Types>(detail::inline_delegate<_Context, &_Context::entity_loaded<Types>>(*this))...
			);
			
			Utils::make_tuple(
			imp.template
			register_loading_handler<Types>(detail::inline_delegate<_Context, &_Context::entity_loading<Types>>(*this))...
			);
			TRACE("Registering types...");
			Utils::make_tuple(register_type<Types>()...);
			TRACE("Types registered.");
		}
		
		template <class... Args>
		constexpr _Context(Args... args) : impl(Utils::move(detail::context_create<Impl>::get(Utils::forward<Args>(args)...))), _sets(create_dbset<Types>(detail::context_create<Impl>::to_reference(impl), &_sets)...), owned(true)
		{
			auto& imp = get_imp();
			
			Utils::make_tuple(
			imp.template register_loaded_listener<Types>(detail::inline_delegate<_Context, &_Context::entity_loaded<Types>>(*this))...
			);
			
			Utils::make_tuple(
			imp.template
			register_loading_handler<Types>(detail::inline_delegate<_Context, &_Context::entity_loading<Types>>(*this))...
			);
			
			Utils::make_tuple(register_type<Types>()...);
		}
		
		~_Context()
		{
			std::clog << __func__ << std::endl;
			if (owned)
			{
				std::clog << "Deleting...\n";
				detail::context_create<Impl>::release(impl);
			}
		}
		
		template <class T>
		DbSet<typename Utils::decay<T>::type>& set()
		{
			typedef typename Utils::decay<T>::type type;
			
			static_assert(entities_type::template contains<type>, "Context does not have a collection for given type");
			return Utils::get<entities_type::template indexof<type>>(_sets);
		}
		
		bool owns_implementation() const noexcept
		{
			return owned;
		}
		
		template <class T>
		bool register_type()
		{
			TRACE("Registering...");
			get_imp().template register_type<T>();
			TRACE("Registered type in implementation.");
			
			typedef detail::BuiltModel<T> Model;
			
			if constexpr (!Utils::is_abstract<T>::value)
			{
				if (factories.count(typeid(typename Model::type)) == 0)
				{
					factories[typeid(typename Model::type)] = new EntityFactory<T, type>;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return true;
			}
		}
		
		
		
	};
	
	
	/*template <class Impl, class... Types>
	class Context : public _Context<Impl, typename detail::reorder_types<Types...>::type>
	{
		public:
		typedef typename Utils::remove_pointer<Impl>::type type;
		//typedef Meta::type_sequence<typename Utils::decay<Types>::type...> entities_type;
		typedef typename detail::reorder_types<Types...>::type entities_type;
		typedef typename entities_type::template select<detail::DbSet_Filter> set_types;
		typedef typename set_types::template apply_template<Utils::tuple> sets_tuple_type;
		
		private:
		Impl impl;
		//Utils::tuple<DbSet<Types>...> _sets;
		sets_tuple_type _sets;
		bool owned;
		Utils::map<Utils::type_index, BaseEntityFactory<type>*> factories;
		
		
		template <class Y>
		struct derived_type_for_base
		{
			template <class T>
			struct inner
			{
				void operator()()
				{
					
				}
			};
		};
		
		
		
		
		
		
		constexpr type& get_imp() noexcept
		{
			if constexpr (Utils::is_pointer<Impl>::value)
			{
				return *impl;
			}
			else
			{
				return impl;
			}
		}
		
		template <class T>
		void entity_loaded(T* ent, const char* tname, const typename detail::BuiltModel<T>::key_type* key)
		{
			set<T>().attach(ent);
			assert(set<T>().count(ent) == 1);
		}
		
		template <class T>
		void entity_loading(T** ent, const char* tname, const typename detail::BuiltModel<T>::key_type* key)
		{
			assert(ent);
			assert(key);
			auto& set = this->set<T>();
			auto ptr = set(*key, false);
			if (!ptr)
			{
				std::clog << "tname = \"" << tname << "\"\n";
				if constexpr (Utils::is_polymorphic<T>::value)
				{
					auto tindex = get_imp().template resolve_type<T>(*key);
					if (factories.count(tindex))
					{
						auto factory = dynamic_cast<EntityFactory<T, type>*>(factories.at(tindex));
						assert(factory);
						ptr = factory->make_proxy(get_imp());
					}
				}
				else
				{
					ptr = set.create_local(*key);
				}
				assert(ptr);
			}
			*ent = ptr;
		}
		
		
		public:
		
		constexpr Context(typename detail::context_create<Impl>::result_type c, bool owned = false) : impl(Utils::forward<typename detail::context_create<Impl>::result_type>(c)), _sets(create_dbset<Types>(detail::context_create<Impl>::to_reference(impl))...), owned(!Utils::is_pointer<Impl>::value || owned)
		{
			auto& imp = get_imp();
			
			Utils::make_tuple(
			imp.template register_loaded_listener<Types>(detail::inline_delegate<Context, &Context::entity_loaded<Types>>(*this))...
			);
			
			Utils::make_tuple(
			imp.template
			register_loading_handler<Types>(detail::inline_delegate<Context, &Context::entity_loading<Types>>(*this))...
			);
			
			Utils::make_tuple(register_type<Types>()...);
		}
		
		template <class... Args>
		constexpr Context(Args... args) : impl(Utils::move(detail::context_create<Impl>::get(Utils::forward<Args>(args)...))), _sets(create_dbset<Types>(detail::context_create<Impl>::to_reference(impl))...), owned(true)
		{
			auto& imp = get_imp();
			
			Utils::make_tuple(
			imp.template register_loaded_listener<Types>(detail::inline_delegate<Context, &Context::entity_loaded<Types>>(*this))...
			);
			
			Utils::make_tuple(
			imp.template
			register_loading_handler<Types>(detail::inline_delegate<Context, &Context::entity_loading<Types>>(*this))...
			);
			
			Utils::make_tuple(register_type<Types>()...);
		}
		
		~Context()
		{
			std::clog << __func__ << std::endl;
			if (owned)
			{
				std::clog << "Deleting...\n";
				detail::context_create<Impl>::release(impl);
			}
		}
		
		template <class T>
		DbSet<typename Utils::decay<T>::type>& set()
		{
			typedef typename Utils::decay<T>::type type;
			
			static_assert(entities_type::template contains<type>, "Context does not have a collection for given type");
			return Utils::get<entities_type::template indexof<type>>(_sets);
		}
		
		bool owns_implementation() const noexcept
		{
			return owned;
		}
		
		template <class T>
		bool register_type()
		{
			get_imp().template register_type<T>();
			
			typedef detail::BuiltModel<T> Model;
			
			if constexpr (!Utils::is_abstract<T>::value)
			{
				if (factories.count(typeid(typename Model::type)) == 0)
				{
					factories[typeid(typename Model::type)] = new EntityFactory<T, type>;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return true;
			}
		}
		
	};*/
	
	
	template <class Impl, class... Types>
	class Context : public _Context<Impl, typename detail::reorder_types<Types...>::type>
	{
		private:
		typedef _Context<Impl, typename detail::reorder_types<Types...>::type> base_type;
		
		public:
		using typename base_type::type;
		using typename base_type::entities_type;
		using typename base_type::set_types;
		using typename base_type::sets_tuple_type;
		
		constexpr Context(typename detail::context_create<Impl>::result_type c, bool owned = false) : base_type(Utils::forward<typename detail::context_create<Impl>::result_type>(c), owned)
		{
			
		}
		
		template <class... Args>
		constexpr Context(Args&&... args) : base_type(Utils::forward<Args&&>(args)...)
		{
			
		}
		
		~Context()
		{
			
		}
		
		
		
	};
}
}


#endif
#endif