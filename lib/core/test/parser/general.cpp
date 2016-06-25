
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/fixed_allocator.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/parser/parser.hpp>

using namespace togo;

static_assert(parser::sizeof_series(0) == 0, "");
static_assert(parser::sizeof_series(0, 0) == 0, "");
static_assert(parser::sizeof_series(1) == sizeof(Parser*), "");
static_assert(parser::sizeof_series(2) == sizeof(Parser*) * 2, "");
static_assert(parser::sizeof_series(2, 2) == sizeof(Parser*) * 4 + sizeof(Parser) * 2, "");

#if defined(TOGO_DEBUG)

#define DEBUG_PRINT_PARSER(p_) do { \
	parser::debug_print_tree(p_); \
	TOGO_LOG("\n"); \
} while (false)

#else
#define DEBUG_PRINT_PARSER(p_) (void(0))
#endif

#define PARSE(p, s) parse(p, s, &error, nullptr)
#define TEST(p, s) test(p, s, &error, nullptr)
#define TEST_WHOLE(p, s) test_whole(p, s, &error, nullptr)

void f_close_nop(ParseState&, Parser const*, char const*, unsigned) {}

signed main() {
	memory::init();

	ParseError error{};
#if defined(TOGO_DEBUG)
	parser::s_debug_trace = true;
#endif

{
	Parser const p{};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::Undefined);
	TOGO_ASSERTE(p.name_hash == hash::IDENTITY32);
	TOGO_ASSERTE(p.name.empty() && !p.name.valid());
}

{
	Parser const p{"test"};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::Undefined);
	TOGO_ASSERTE(p.name_hash == "test"_hash32);
	TOGO_ASSERTE(p.name.any() && p.name.valid());
	TOGO_ASSERTE(string::compare_equal(p.name, "test"));
}

{
	Parser const p{Nothing{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::Nothing);
}

{
	Parser const p{Empty{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::Empty);
}

{
	Parser const p{Begin{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::Begin);
}

{
	Parser const p{End{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::End);
}

{
	Parser const p{Char{'x'}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::Char);
	TOGO_ASSERTE(p.s.Char.c == 'x');
}

{
	Parser const p{CharRange{'a', 'z'}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::CharRange);
	TOGO_ASSERTE(p.s.CharRange.b == 'a' && p.s.CharRange.e == 'z');
}

{
	Parser const p{String{"abcdef"}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(p.type == ParserType::String);
	TOGO_ASSERTE(string::compare_equal(p.s.String.s, "abcdef"));
}

{
	FixedParserAllocator<1> a;
	Parser const p1{Char{'x'}};
	Parser const p2{Any{a, p1}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p2);

	TOGO_ASSERTE(p2.type == ParserType::Any);
	TOGO_ASSERTE(p2.s.Any.num == 1);
	TOGO_ASSERTE(p2.s.Any.p[0] == &p1);
}

{
	FixedParserAllocator<1, 1> a;
	Parser const p1{Char{'x'}};
	Parser const p2{Any{a, p1, Parser{Char{'y'}}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p2);

	TOGO_ASSERTE(p2.type == ParserType::Any);
	TOGO_ASSERTE(p2.s.Any.num == 2);
	TOGO_ASSERTE(p2.s.Any.p[0] == &p1);

	Parser const* p3 = p2.s.Any.p[1];
	TOGO_ASSERTE(p3->type == ParserType::Char);
	TOGO_ASSERTE(p3->s.Char.c == 'y');
}

{
	FixedParserAllocator<1, 1> a;
	Parser const p1{Char{'x'}};
	Parser const p2{All{a, p1, Parser{Char{'y'}}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p2);

	TOGO_ASSERTE(p2.type == ParserType::All);
	TOGO_ASSERTE(p2.s.All.num == 2);
	TOGO_ASSERTE(p2.s.All.p[0] == &p1);

	Parser const* p3 = p2.s.All.p[1];
	TOGO_ASSERTE(p3->type == ParserType::Char);
	TOGO_ASSERTE(p3->s.Char.c == 'y');
}

{
	Parser const p1{Char{'x'}};
	Parser const p2{Maybe{p1}};
	TOGO_ASSERTE(p2.type == ParserType::Maybe);
	TOGO_ASSERTE(p2.s.Maybe.p == &p1);
}

{
	FixedParserAllocator<0, 0, 1> a;
	Parser const p{Maybe{a, Char{'x'}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(p.type == ParserType::Maybe);
	TOGO_ASSERTE(p.s.Maybe.p->type == ParserType::Char);
	TOGO_ASSERTE(p.s.Maybe.p->s.Char.c == 'x');
}

{
	FixedParserAllocator<0, 0, 1> a;
	Parser const p{Close{a, f_close_nop, Char{'x'}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(p.type == ParserType::Close);
	TOGO_ASSERTE(p.s.Close.f == f_close_nop);
	TOGO_ASSERTE(p.s.Close.p->type == ParserType::Char);
	TOGO_ASSERTE(p.s.Close.p->s.Char.c == 'x');
}

{
	Parser const p{Char{'x'}};
	TOGO_ASSERTE(TEST(p, "x"));
	TOGO_ASSERTE(TEST(p, "xz"));
	TOGO_ASSERTE(!TEST(p, "y"));
	TOGO_ASSERTE(!TEST(p, ""));
	TOGO_ASSERTE(TEST_WHOLE(p, "x"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "z"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, ""));
}

{
	Parser const p{CharRange{'a', 'z'}};
	TOGO_ASSERTE(TEST(p, "a"));
	TOGO_ASSERTE(TEST(p, "z"));
	TOGO_ASSERTE(!TEST(p, "X"));
	TOGO_ASSERTE(!TEST(p, ""));
	TOGO_ASSERTE(TEST_WHOLE(p, "a"));
	TOGO_ASSERTE(TEST_WHOLE(p, "z"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "aZ"));
	TOGO_ASSERTE(!TEST_WHOLE(p, ""));
}

{
	Parser const p{CharRange{'a', 'z'}};
	TOGO_ASSERTE(TEST(p, "a"));
	TOGO_ASSERTE(TEST(p, "z"));
	TOGO_ASSERTE(!TEST(p, "A"));
	TOGO_ASSERTE(!TEST(p, "Z"));
}

{
	Parser const p{String{"abcdef"}};
	TOGO_ASSERTE(TEST(p, "abcdef"));
	TOGO_ASSERTE(TEST(p, "abcdefghijkl"));
	TOGO_ASSERTE(!TEST(p, "ghijkl"));
	TOGO_ASSERTE(!TEST(p, ""));
	TOGO_ASSERTE(TEST_WHOLE(p, "abcdef"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "abcdefghijkl"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "ghijkl"));
	TOGO_ASSERTE(!TEST_WHOLE(p, ""));
}

{
	FixedParserAllocator<0, 2> a;
	Parser const p{Any{a, Char{'x'}, Char{'y'}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(TEST(p, "x"));
	TOGO_ASSERTE(TEST(p, "xz"));
	TOGO_ASSERTE(TEST(p, "y"));
	TOGO_ASSERTE(TEST(p, "yz"));
	TOGO_ASSERTE(!TEST(p, "z"));
	TOGO_ASSERTE(!TEST(p, ""));

	TOGO_ASSERTE(TEST_WHOLE(p, "x"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xz"));
	TOGO_ASSERTE(TEST_WHOLE(p, "y"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "yz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "z"));
	TOGO_ASSERTE(!TEST_WHOLE(p, ""));
}

{
	FixedParserAllocator<0, 2> a;
	Parser const p{All{a, Char{'x'}, Char{'y'}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(TEST(p, "xy"));
	TOGO_ASSERTE(!TEST(p, "xx"));
	TOGO_ASSERTE(!TEST(p, "xz"));
	TOGO_ASSERTE(!TEST(p, "y"));
	TOGO_ASSERTE(!TEST(p, "z"));
	TOGO_ASSERTE(!TEST(p, ""));

	TOGO_ASSERTE(TEST_WHOLE(p, "xy"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xyz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xx"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "y"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "z"));
	TOGO_ASSERTE(!TEST_WHOLE(p, ""));
}

{
	FixedParserAllocator<0, 2, 1> a;
	Parser const p{All{a,
		Maybe{a, Char{'x'}},
		Char{'y'}
	}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(TEST(p, "y"));
	TOGO_ASSERTE(TEST(p, "xy"));
	TOGO_ASSERTE(TEST(p, "yz"));
	TOGO_ASSERTE(TEST(p, "xyz"));
	TOGO_ASSERTE(!TEST(p, "xz"));
	TOGO_ASSERTE(!TEST(p, "zxy"));
	TOGO_ASSERTE(!TEST(p, "x"));
	TOGO_ASSERTE(!TEST(p, "z"));
	TOGO_ASSERTE(!TEST(p, ""));

	TOGO_ASSERTE(TEST_WHOLE(p, "y"));
	TOGO_ASSERTE(TEST_WHOLE(p, "xy"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "yz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xyz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "zxy"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "x"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "z"));
	TOGO_ASSERTE(!TEST_WHOLE(p, ""));
}

{
	parser::close_func_type* f = [](ParseState& s, Parser const*, char const* fp, unsigned fi) {
		TOGO_ASSERTE(size(s.results) == 1);
		TOGO_ASSERTE(s.p == s.e && fp == s.b && fi == 0);
		auto& r = s.results[0];
		TOGO_ASSERTE(r.type == ParseResult::type_char && r.v.c == 'x');
	};
	FixedParserAllocator<0, 0, 1> a;
	Parser const p{Close{a, f, Char{'x'}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(PARSE(p, "x"));
}

{
	parser::close_func_type* f = [](ParseState& s, Parser const*, char const* fp, unsigned fi) {
		TOGO_ASSERTE(size(s.results) == 2);
		TOGO_ASSERTE(s.p == (s.b + 2) && fp == s.b && fi == 0);
		auto& r = s.results[0];
		TOGO_ASSERTE(r.type == ParseResult::type_char && r.v.c == 'x');
	};
	FixedParserAllocator<0, 2, 1> a;
	Parser const p{"duo", Close{a, f, All{a,
		Char{'x'},
		Char{'y'}
	}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(PARSE(p, "xy"));
	TOGO_ASSERTE(PARSE(p, "xyz"));
	TOGO_ASSERTE(!PARSE(p, "y"));
	TOGO_ASSERTE(!PARSE(p, ""));
}

	memory::shutdown();
	return 0;
}
