
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/fixed_allocator.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/parser/parser.hpp>
#include <togo/core/parser/parse_state.hpp>

#include "./common.hpp"

using namespace togo;

static_assert(parser::sizeof_series(0) == 0, "");
static_assert(parser::sizeof_series(0, 0) == 0, "");
static_assert(parser::sizeof_series(1) == sizeof(Parser*), "");
static_assert(parser::sizeof_series(2) == sizeof(Parser*) * 2, "");
static_assert(parser::sizeof_series(2, 2) == sizeof(Parser*) * 4 + sizeof(Parser) * 2, "");

ParseResultCode f_func_nop(Parser const*, ParseState& s, ParsePosition const&) {
	return parse_state::ok(s);
}

signed main() {
	memory::init();

#if defined(TOGO_DEBUG)
	parser::s_debug_trace = true;
#endif

	FixedAllocator<1024 * 4> state_storage;
	ParseError error{};
	ParseState state{state_storage, &error};

{
	// ADD THOSE TESTS!!
	ParserType type = ParserType::Undefined;
	switch (type) {
	case ParserType::Undefined: break;
	case ParserType::Nothing: break;
	case ParserType::Empty: break;
	case ParserType::Head: break;
	case ParserType::Tail: break;
	case ParserType::Char: break;
	case ParserType::CharRange: break;
	case ParserType::String: break;
	case ParserType::Any: break;
	case ParserType::All: break;
	case ParserType::Ref: break;
	case ParserType::Func: break;
	case ParserType::Close: break;
	}
	ParserModifier mods = PMod::none;
	switch (mods) {
	case PMod::none: break;
	case PMod::maybe: break;
	case PMod::test: break;
	case PMod::flatten: break;
	case PMod::repeat: break;
	case PMod::repeat_or_none: break;
	}
}

{
	Parser const p{PMod::maybe, Nothing{}};
	TOGO_ASSERTE(modifiers(p) == PMod::maybe);
}

{
	Parser const p{PMod::test, Nothing{}};
	TOGO_ASSERTE(modifiers(p) == PMod::test);
}

{
	Parser const p{PMod::flatten, Nothing{}};
	TOGO_ASSERTE(modifiers(p) == PMod::flatten);
}

{
	Parser const p{PMod::repeat, Nothing{}};
	TOGO_ASSERTE(modifiers(p) == PMod::repeat);
}

{
	Parser const p{PMod::repeat_or_none, Nothing{}};
	TOGO_ASSERTE(modifiers(p) == PMod::repeat_or_none);
}

{
	Parser const p{};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::Undefined);
	TOGO_ASSERTE(modifiers(p) == PMod::none);
	TOGO_ASSERTE(p.name_hash == hash::IDENTITY32);
	TOGO_ASSERTE(p.name.empty() && !p.name.valid());
}

{
	Parser const p{"test"};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::Undefined);
	TOGO_ASSERTE(p.name_hash == "test"_hash32);
	TOGO_ASSERTE(p.name.any() && p.name.valid());
	TOGO_ASSERTE(string::compare_equal(p.name, "test"));
}

{
	Parser const p{Nothing{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::Nothing);
}

{
	Parser const p{Empty{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::Empty);
}

{
	Parser const p{Head{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::Head);
}

{
	Parser const p{Tail{}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::Tail);
}

{
	Parser const p{Char{'x'}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::Char);
	TOGO_ASSERTE(p.s.Char.c == 'x');
}

{
	Parser const p{CharRange{'a', 'z'}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::CharRange);
	TOGO_ASSERTE(p.s.CharRange.b == 'a' && p.s.CharRange.e == 'z');
}

{
	Parser const p{String{"abcdef"}};
	DEBUG_PRINT_PARSER(p);
	TOGO_ASSERTE(type(p) == ParserType::String);
	TOGO_ASSERTE(string::compare_equal(p.s.String.s, "abcdef"));
}

{
	FixedParserAllocator<1> a;
	Parser const p1{Char{'x'}};
	Parser const p2{Any{a, p1}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p2);

	TOGO_ASSERTE(type(p2) == ParserType::Any);
	TOGO_ASSERTE(p2.s.Any.num == 1);
	TOGO_ASSERTE(p2.s.Any.p[0] == &p1);
}

{
	FixedParserAllocator<1, 1> a;
	Parser const p1{Char{'x'}};
	Parser const p2{Any{a, p1, Parser{Char{'y'}}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p2);

	TOGO_ASSERTE(type(p2) == ParserType::Any);
	TOGO_ASSERTE(p2.s.Any.num == 2);
	TOGO_ASSERTE(p2.s.Any.p[0] == &p1);

	Parser const* p3 = p2.s.Any.p[1];
	TOGO_ASSERTE(type(*p3) == ParserType::Char);
	TOGO_ASSERTE(p3->s.Char.c == 'y');
}

{
	FixedParserAllocator<1, 1> a;
	Parser const p1{Char{'x'}};
	Parser const p2{All{a, p1, Parser{Char{'y'}}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p2);

	TOGO_ASSERTE(type(p2) == ParserType::All);
	TOGO_ASSERTE(p2.s.All.num == 2);
	TOGO_ASSERTE(p2.s.All.p[0] == &p1);

	Parser const* p3 = p2.s.All.p[1];
	TOGO_ASSERTE(type(*p3) == ParserType::Char);
	TOGO_ASSERTE(p3->s.Char.c == 'y');
}

{
	Parser const p{Func{f_func_nop, &error}};
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(type(p) == ParserType::Func);
	TOGO_ASSERTE(p.s.Func.f == f_func_nop);
	TOGO_ASSERTE(p.s.Func.userdata == &error);
}

{
	FixedParserAllocator<0, 0, 1> a;
	Parser const p{Close{a, f_func_nop, Char{'x'}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(type(p) == ParserType::Close);
	TOGO_ASSERTE(p.s.Close.f == f_func_nop);
	TOGO_ASSERTE(type(*p.s.Close.p) == ParserType::Char);
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
	FixedParserAllocator<0, 2> a;
	Parser const p{All{a,
		Parser{PMod::maybe, Char{'x'}},
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
	Parser const p{PMod::repeat, Char{'x'}};
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(TEST(p, "x"));
	TOGO_ASSERTE(TEST(p, "xxx"));
	TOGO_ASSERTE(TEST(p, "xxxz"));
	TOGO_ASSERTE(!TEST(p, "z"));
	TOGO_ASSERTE(!TEST(p, ""));
}

{
	Parser const p{PMod::repeat_or_none, Char{'x'}};
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(TEST_WHOLE(p, ""));
	TOGO_ASSERTE(TEST_WHOLE(p, "x"));
	TOGO_ASSERTE(TEST_WHOLE(p, "xxx"));
	TOGO_ASSERTE(TEST(p, "xxxz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xxxz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "z"));
}

{
	FixedParserAllocator<0, 2> a;
	Parser const p{All{a,
		Parser{PMod::repeat, Char{'x'}},
		Char{'y'}
	}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(TEST(p, "xy"));
	TOGO_ASSERTE(TEST(p, "xxy"));
	TOGO_ASSERTE(TEST(p, "xxxxxxxxxxxxxxxxxxxy"));
	TOGO_ASSERTE(TEST(p, "xyz"));
	TOGO_ASSERTE(!TEST(p, "yz"));
	TOGO_ASSERTE(!TEST(p, "y"));
	TOGO_ASSERTE(!TEST(p, "xz"));
	TOGO_ASSERTE(!TEST(p, "zxy"));
	TOGO_ASSERTE(!TEST(p, "x"));
	TOGO_ASSERTE(!TEST(p, "z"));
	TOGO_ASSERTE(!TEST(p, ""));
	TOGO_ASSERTE(!TEST(p, "xxz"));

	TOGO_ASSERTE(TEST_WHOLE(p, "xy"));
	TOGO_ASSERTE(TEST_WHOLE(p, "xxy"));
	TOGO_ASSERTE(TEST_WHOLE(p, "xxxxxxxxxxxxxxxxxxxy"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xyz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "yz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "y"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xz"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "zxy"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "x"));
	TOGO_ASSERTE(!TEST_WHOLE(p, "z"));
	TOGO_ASSERTE(!TEST_WHOLE(p, ""));
	TOGO_ASSERTE(!TEST_WHOLE(p, "xxz"));
}

{
	parser::parse_func_type* f = [](Parser const*, ParseState& s, ParsePosition const& from) {
		TOGO_ASSERTE(s.p == s.b && from.p == s.b && from.i == 0);
		s.p = s.e;
		return parse_state::ok(s, {from.p, s.p});
	};
	Parser const p{Func{f, &error}};
	DEBUG_PRINT_PARSER(p);

	PARSE_S(p, "xyz");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_slice);
	TOGO_ASSERTE(r.v.s.b == state.b && r.v.s.e == state.e);
}

{
	parser::parse_func_type* f = [](Parser const*, ParseState& s, ParsePosition const& from) {
		TOGO_ASSERTE(size(s.results) == 1);
		TOGO_ASSERTE(s.p == s.e && from.p == s.b && from.i == 0);
		auto& r = s.results[0];
		TOGO_ASSERTE(r.type == ParseResult::type_char && r.v.c == 'x');
		return parse_state::ok(s);
	};
	FixedParserAllocator<0, 0, 1> a;
	Parser const p{Close{a, f, Char{'x'}}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);
	DEBUG_PRINT_PARSER(p);

	TOGO_ASSERTE(PARSE(p, "x"));
}

{
	parser::parse_func_type* f = [](Parser const*, ParseState& s, ParsePosition const& from) {
		TOGO_ASSERTE(size(s.results) == 2);
		TOGO_ASSERTE(s.p == (s.b + 2) && from.p == s.b && from.i == 0);
		auto& r = s.results[0];
		TOGO_ASSERTE(r.type == ParseResult::type_char && r.v.c == 'x');
		return parse_state::ok(s);
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

{
	parser::parse_func_type* f = [](Parser const*, ParseState& s, ParsePosition const& from) {
		TOGO_ASSERTE(empty(s.results));
		TOGO_ASSERTE(s.p == (s.b + 2) && from.p == s.b && from.i == 0);
		return parse_state::ok(s);
	};
	FixedParserAllocator<0, 2, 1> a;
	Parser const p{"duo", PMod::test, Close{a, f, All{a,
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
