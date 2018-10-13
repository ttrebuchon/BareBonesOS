#include "command.hh"
#include "parsing.hh"
#include <boost/spirit/include/qi.hpp>
#include <string>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/utility/identity_type.hpp>

#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <list>
#include <Utils/string>
#include <Utils/list>


using namespace boost::spirit;

namespace Kernel
{
	typedef std::char_traits<char> ctraits;
	
	
	template <class T, class A>
	using parser_container = std::vector<T, typename A::template rebind<T>::other>;
	template <class A>
	using parser_string = std::basic_string<char, ctraits, typename A::template rebind<char>::other>;
	template <class A, class T>
	using a_rebind = typename A::template rebind<T>::other;
	
	template <class A>
	using utils_parser_string = Utils::basic_string<char, Utils::Char_Traits<char>, a_rebind<A, char>>;
	
	
	
	template <class A>
	struct command_t
	{
		typedef std::basic_string<char, ctraits, typename A::template rebind<char>::other> string_type;
		
		typedef typename A::template rebind<string_type>::other string_alllcator_type;
		
		string_type command;
		parser_container<string_type, A> args;
		
		
		command_t(const A& a) : command(a_rebind<A, char>(a)), args(a_rebind<A, string_type>(a))
		{
			
		}
		
		command_t() : command_t(A())
		{
			
		}
		
		command_t(const command_t& cmd) : command(cmd.command), args(cmd.args)
		{
			
		}
	};
	
	template <class A>
	struct command_continuation_t
	{
		typedef std::basic_string<char, ctraits, typename A::template rebind<char>::other> string_type;
		
		string_type type;
		command_t<A> cmd;
		
		command_continuation_t(const A& a) : type(a), cmd(a)
		{
			
		}
		
		command_continuation_t() : command_continuation_t(A())
		{
			
		}
		
		
		
		command_continuation_t(const string_type& type, const command_t<A>& cmd) : type(type), cmd(cmd)
		{
			
		}
	};
	
	template <class A>
	struct parsed_command_t
	{
		command_t<A> cmd;
		
		parser_container<command_continuation_t<A>, A> continuations;
		
		parsed_command_t(const A& a) : cmd(a), continuations(a_rebind<A, command_continuation_t<A>>(a))
		{
			
		}
		
		parsed_command_t() : parsed_command_t(A())
		{
			
		}
	};
}

BOOST_FUSION_ADAPT_TPL_STRUCT(
(A),
(Kernel::command_t) (A),
(typename Kernel::command_t<A>::string_type, command)
(typename BOOST_IDENTITY_TYPE((Kernel::parser_container<typename Kernel::command_t<A>::string_type, A>)), args)
)

BOOST_FUSION_ADAPT_TPL_STRUCT(
(A),
(Kernel::command_continuation_t) (A),
(typename Kernel::command_continuation_t<A>::string_type, type)
(Kernel::command_t<A>, cmd)
)

BOOST_FUSION_ADAPT_TPL_STRUCT(
(A),
(Kernel::parsed_command_t) (A),
(Kernel::command_t<A>, cmd)
(typename BOOST_IDENTITY_TYPE((Kernel::parser_container<Kernel::command_continuation_t<A>, A>)), continuations)
)

namespace Kernel
{
	
	
	template <class It, class Alloc, class string_type>
	struct parser_helpers
	{
		Alloc alloc;
		qi::rule<It, void()> spaces;
		
		parser_helpers(const Alloc& a) : alloc(a)
		{
			spaces = (*lit(' '));
		}
		
		
		qi::rule<It, string_type()> make_quote_rule(char c, bool include_quotes)
    	{
    		using qi::lexeme;
    		using ascii::char_;
    		using qi::string;
    		
    		qi::rule<It, string_type()> r;
    		if (!include_quotes)
    		{
    			r = lexeme[lit(c) >> +(char_ - c) >> lit(c)];
    		}
    		else
    		{
    			string_type str;
    			str += c;
    			r = lexeme[string(str) >> +(char_ - c) >> string(str)];
    		}
    		return r;
    	}
    	
    	
		
	};
	
    
    template <class It, class A>
    struct cmd_parser : qi::grammar<It, command_t<A>()>
    {
    	typedef typename A::template rebind<char>::other char_alloc_type;
    	typedef std::basic_string<char, ctraits, char_alloc_type> string_type;
    	typedef command_t<A> command_type;
    	A alloc;
    	char_alloc_type char_alloc;
    	
    	qi::rule<It, command_type()> start;
    	qi::rule<It, string_type()> base_cmd;
    	qi::rule<It, string_type()> arg;
    	qi::rule<It, std::vector<string_type, a_rebind<A, string_type>>()> args;
    	qi::rule<It, command_type()> command;
    	qi::rule<It, string_type()> quoted_string;
    	qi::rule<It, string_type()> single_quoted_string;
    	qi::rule<It, string_type()> single_word;
    	qi::rule<It, char()> reserved_chars;
    	
    	
    	cmd_parser(const A& alloc) : cmd_parser::base_type(start), alloc(alloc), char_alloc(alloc)
    	{
    		using qi::int_;
    		using qi::double_;
    		using qi::lit;
    		using qi::lexeme;
    		using ascii::char_;
    		using boost::phoenix::at_c;
    		using boost::phoenix::push_back;
    		
    		quoted_string %= lexeme['"' >> *(char_ - '"') >> '"'];
    		//quoted_string %= make_quote_rule('"', char_alloc);
    		single_quoted_string = make_quote_rule('\'', char_alloc);
    		
    		init_reserved();
    		init_single_word();
    		init_base_cmd();
    		init_arg();
    		
    		command %= 
    			base_cmd
    			>>
    			*(*lit(' ') >> arg);
    		
    		start %= command;
    	}
    	
    	
    	
    	void init_reserved()
    	{
    		using ascii::char_;
    		using qi::lexeme;
    		using qi::lit;
    		using qi::string;
    		
    		reserved_chars %= (lit('|') | '>' | '<' | '\\' | '&');
    	}
    	
    	void init_single_word()
    	{
    		using ascii::char_;
    		using qi::lexeme;
    		using qi::lit;
    		
    		auto disallowed = (reserved_chars | ' ' | '"' | '\'');
    		
    		
    		//single_word %= /*lexeme[*/' ' >> +(char_ - ' ' - reserved_chars)/*]*/;
    		single_word %= lexeme[+(char_ - disallowed)];
    	}
    	
    	
    	void init_base_cmd()
    	{
    		using qi::int_;
    		using qi::double_;
    		using qi::lit;
    		using qi::lexeme;
    		using ascii::char_;
    		
    		base_cmd %= (single_word | quoted_string | single_quoted_string);
    	}
    	
    	void init_arg()
    	{
    		using qi::int_;
    		using qi::double_;
    		using qi::lit;
    		using qi::lexeme;
    		using ascii::char_;
    		
    		
    		
    		arg %= (quoted_string | single_quoted_string | single_word);
    		
    		args %= *(arg);
    	}
    	
    	static qi::rule<It, std::string()> make_quote_rule(char c, char_alloc_type& ca)
    	{
    		using qi::lexeme;
    		using ascii::char_;
    		
    		qi::rule<It, std::string()> r;
    		r = lexeme[lit(c) >> *(char_ - c) >> lit(c)];
    		return r;
    	}
    };
    
    
    
    template <class It, class A>
    struct input_parser : qi::grammar<It, parsed_command_t<A>()>
    {
    	typedef std::basic_string<char, ctraits, typename A::template rebind<char>::other> string_type;
    	typedef command_t<A> command_type;
    	typedef parsed_command_t<A> parsed_type;
    	typedef command_continuation_t<A> cont_type;
    	
    	input_parser(const A& a) : input_parser::base_type(start), command(a)
    	{
    		using qi::int_;
    		using qi::double_;
    		using qi::lit;
    		using qi::string;
    		using qi::lexeme;
    		using ascii::char_;
    		using boost::phoenix::at_c;
    		using boost::phoenix::push_back;
    		
    		spaces = (*lit(' '));
    		
    		
    		cont_symbol %= *((string("|") | string("&")));
    		
    		cont = (spaces >> cont_symbol >> spaces >> command);
    		
    		conts %= *(cont);
    		
    		
    		start = (*lit(' ') >> command >> *lit(' ') >> conts);
    	}
    	
    	input_parser(const input_parser& other) : input_parser(other.command.alloc)
    	{
    		TRACE("COPIED");
    	}
    	
    	
    	qi::rule<It, parsed_type()> start;
    	cmd_parser<It, A> command;
    	
    	qi::rule<It, cont_type()> pipe;
    	qi::rule<It, std::vector<cont_type>()> conts;
    	qi::rule<It, string_type()> cont_symbol;
    	qi::rule<It, cont_type()> cont;
    	qi::rule<It, void()> spaces;
    };
    
    
    
    
    
    
    
    template <class A>
    using string_alloc_type = std::basic_string<char, ctraits, A>;
    
    
    
    
    template <class A>
	string_alloc_type<A> string_cmd(const command_t<A>& cmd)
	{
		string_alloc_type<A> str = cmd.command + "\nArgs = \n{";
			
		for (const auto& a : cmd.args)
		{
			str += " [" + a + "]";
		}
		str += " }\n\n";
		
		return str;
	}
	
	template <class A>
	void print_cmd(const command_t<A>& cmd)
	{
		TRACE_VAL(string_cmd(cmd));
	}
	
	template <class A>
	parser_string<A> unpack_cmd(const command_t<A>& cmd)
	{
		parser_string<A> str(a_rebind<A, char>(cmd.command.get_allocator()));
		str += "\"";
		str += cmd.command.c_str();
		str += "\"";
		
		for (const auto& arg : cmd.args)
		{
			str += " \"";
			str += arg.c_str();
			str += "\"";
		}
		
		return str;
	}
	
	template <class A>
	parser_string<A> unpack_continuation(const command_continuation_t<A>& cont)
	{
		parser_string<A> str = cont.type + " ";
		str += unpack_cmd<A>(cont.cmd);
		return str;
	}
	
	template <class A>
	parser_string<A> unpack_parsed_command(const parsed_command_t<A>& cmd)
	{
		parser_string<A> str = unpack_cmd<A>(cmd.cmd);
		for (const auto& cont : cmd.continuations)
		{
			str += " " + unpack_continuation<A>(cont);
		}
		return str;
	}
    
    
    
    
    
    
    
    
	
	
	template <class It, class A>
	bool parse_ns(It begin, It end, const A& alloc, parsed_command_t<A>& parsed, bool* returned_true = nullptr)
	{
		using qi::double_;
		using qi::phrase_parse;
		using boost::phoenix::ref;
		using qi::_1;
		using ascii::space;
		bool res = false;
		
		typedef typename A::template rebind<char>::other char_allocator;
		
		
		//parsed_command_t<A> parsed(alloc);
		input_parser<It, A> p(alloc);
		
		res = qi::parse(begin, end, p, parsed);
		
		/*if (!res || (begin != end))
		{
			if (!res)
			{
				TRACE("Parse failed.");
			}
			else
			{
				TRACE("Parse is incomplete.");
				goto print;
			}
			return false;
		}
		
		print:
		
		TRACE_VAL(unpack_parsed_command(parsed));*/
		
		
		if (returned_true != nullptr)
		{
			*returned_true = res;
		}
		return res && (begin == end);
	}
	
	namespace detail::shell
	{
	
	bool shell_input_parse(const char* start, const char* end, const Kernel::Memory::virtual_allocator<uint8_t>& _alloc, Command* cmd)
	{
		typedef std::allocator<void> alloc_type;
		typedef Memory::virtual_allocator<uint8_t> allocator_type;
		typedef Command command_type;
		alloc_type a;
		allocator_type alloc(_alloc);
		parsed_command_t<alloc_type> parsed;
		
		auto res = parse_ns(start, end, a, parsed);
		if (res && cmd)
		{
			cmd->command = parsed.cmd.command.c_str();
			for (const auto& arg : parsed.cmd.args)
			{
				cmd->args.emplace_back(arg.c_str());
			}
			
			
			
			typename allocator_type::template rebind<CommandContinuation>::other cont_alloc(alloc);
			if (!parsed.continuations.empty())
			{
				auto it = cmd;
				
				auto deleter = [=](detail::shell::CommandContinuation* cont)
				{
					cont->~CommandContinuation();
					auto cont_alloc2 = cont_alloc;
					cont_alloc2.deallocate(cont, 1);
				};
				
				for (auto& cont : parsed.continuations)
				{
					command_type new_cmd(alloc);
					new_cmd.command = cont.cmd.command.c_str();
					for (const auto& arg : cont.cmd.args)
					{
						new_cmd.args.emplace_back(arg.c_str());
					}
					
					auto new_cont = cont_alloc.allocate(1);
					new (new_cont) detail::shell::CommandContinuation(typename command_type::string_type(cont.type.c_str()), new_cmd);
					
					it->continuation = Utils::shared_ptr<detail::shell::CommandContinuation>(new_cont, deleter, alloc);
					it = &new_cont->command;
					
					
				}
			}
		}
		return res;
	}
	
	}
	
	
	
	
	
	template <class It>
	struct path_string_parser : qi::grammar<It, std::vector<std::string>()>
    {
    	typedef std::string string_type;
    	typedef std::vector<string_type> value_type;
    	
    	parser_helpers<It, typename value_type::allocator_type, string_type> helpers;
    	qi::rule<It, value_type()> start;
    	qi::rule<It, string_type()> segment;
    	qi::rule<It, string_type()> special_chars;
    	qi::rule<It, string_type()> escape_seqs;
    	qi::rule<It, string_type()> valid_chars;
    	qi::rule<It, char()> quote_chars;
    	qi::rule<It, void()> separator;
    	qi::rule<It, string_type()> single_quote, double_quote;
    	
		path_string_parser() : qi::grammar<It, value_type()>(start), helpers(typename value_type::allocator_type())
		{
			using qi::string;
			using ascii::char_;
			using qi::lit;
			
			separator = (lit(';'));
			
			quote_chars %= (string("'") | string("\""));
			
			special_chars %= (string(";") | string("\"") | string("'"));
			
			single_quote = helpers.make_quote_rule('\'', true);
			double_quote = helpers.make_quote_rule('"', true);
			
			
			//escape_seqs %= (string("\\") >> special_chars);
			
			//segment %= *(*(char_ - ';' - '\'' - '"') | double_quote | single_quote);
			segment %= +(+(char_ - special_chars) | single_quote | double_quote);
			
			start %= (segment % +char_(';'));
			
		}
	};
	
	template <class It>
	bool parse_path(It begin, It end, auto& parsed)
	{
		path_string_parser<It> parser;
		
		bool res = qi::parse(begin, end, parser, parsed);
		
		return res;
	}
	
	
	
	
	namespace detail::shell
	{
		bool path_string_parse(const char* start, const char* end, const Kernel::Memory::virtual_allocator<uint8_t>& alloc, Utils::vector<utils_string_type, typename Kernel::Memory::virtual_allocator<utils_string_type>>& parts)
		{
			bool res = false;
			std::vector<std::string> nparts;
			
			res = parse_path(start, end, nparts);
			TRACE("PARSED.");
			if (res)
			{
				for (const auto& str : nparts)
				{
					parts.emplace_back(str.c_str());
				}
			}
			
			return res;
		}
	}
	
	
	
	
}