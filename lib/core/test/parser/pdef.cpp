
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

inline static bool float_equal_exact(f64 a, f64 b) {
	union {
		f64 f;
		u64 u;
	} ap{a}, bp{b};
	return ap.u == bp.u;
}

signed main() {
	memory::init();

#if defined(TOGO_DEBUG)
	parser::s_debug_trace = true;
#endif

	FixedAllocator<1024 * 4> state_storage;
	ParseError error{};
	ParseState state{state_storage, &error};

DEBUG_PRINT_PARSER(PDef::whitespace);
	TOGO_ASSERTE(TEST_WHOLE(PDef::whitespace, " "));
	TOGO_ASSERTE(TEST_WHOLE(PDef::whitespace, "  "));
	TOGO_ASSERTE(TEST_WHOLE(PDef::whitespace, "\t\n"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::whitespace, "\t\t"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::whitespace, "\t\n\r"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::whitespace, " \t\n\r"));
	TOGO_ASSERTE(TEST(PDef::whitespace, "   x"));
	TOGO_ASSERTE(!TEST_WHOLE(PDef::whitespace, "   x"));
	TOGO_ASSERTE(!TEST(PDef::whitespace, ""));

{
	FixedParserAllocator<2, 1> a;
	Parser const p{All{a,
		PDef::whitespace_maybe,
		Parser{PMod::repeat, CharRange{'a', 'z'}},
		PDef::whitespace_maybe
	}};
	TOGO_ASSERTE(a._put == a._buffer + a.BUFFER_SIZE);

	TOGO_ASSERTE(TEST_WHOLE(p, "x"));
	TOGO_ASSERTE(TEST_WHOLE(p, "x "));
	TOGO_ASSERTE(TEST_WHOLE(p, " x"));
	TOGO_ASSERTE(TEST_WHOLE(p, "\t\tx\r\n"));
	TOGO_ASSERTE(!TEST(p, ""));
}

DEBUG_PRINT_PARSER(PDef::null);
{
	PARSE_S(PDef::null, "null");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	TOGO_ASSERTE(state.p == state.e);
	auto& v = state.results[0];
	TOGO_ASSERTE(v.type == ParseResult::type_null);
}

DEBUG_PRINT_PARSER(PDef::boolean);
{
	PARSE_S(PDef::boolean, "true");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(state.p == state.e);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_bool);
	TOGO_ASSERTE(r.b == true);
}

{
	PARSE_S(PDef::boolean, "false");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(state.p == state.e);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_bool);
	TOGO_ASSERTE(r.b == false);
}

DEBUG_PRINT_PARSER(PDef::digit_dec);
DEBUG_PRINT_PARSER(PDef::digits_dec);
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_dec, "0"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_dec, "9"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_dec, "00"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_dec, "99"));

DEBUG_PRINT_PARSER(PDef::digit_hex);
DEBUG_PRINT_PARSER(PDef::digits_hex);
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_hex, "0"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_hex, "9"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_hex, "a"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_hex, "f"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_hex, "A"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_hex, "F"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_hex, "00aa"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_hex, "99ff"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_hex, "00AA"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_hex, "99FF"));

DEBUG_PRINT_PARSER(PDef::digit_oct);
DEBUG_PRINT_PARSER(PDef::digits_oct);
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_oct, "0"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_oct, "7"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_oct, "00"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_oct, "77"));

DEBUG_PRINT_PARSER(PDef::u64_any);
DEBUG_PRINT_PARSER(PDef::u64_dec);
{
	PARSE_S(PDef::u64_dec, "0");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 0);
}

{
	PARSE_S(PDef::u64_dec, "42");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 42);
}

{
	PARSE_S(PDef::u64_any, "42");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 42);
}

DEBUG_PRINT_PARSER(PDef::u64_hex);
{
	PARSE_S(PDef::u64_hex, "0x00");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 0x00);
}

{
	PARSE_S(PDef::u64_hex, "0x2a");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 0x2a);
}

{
	PARSE_S(PDef::u64_hex, "0x2A");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 0x2A);
}

{
	PARSE_S(PDef::u64_any, "0x2A");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 0x2A);
}

DEBUG_PRINT_PARSER(PDef::u64_oct);
{
	PARSE_S(PDef::u64_oct, "0");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 0);
}

{
	PARSE_S(PDef::u64_oct, "052");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 052);
}

{
	PARSE_S(PDef::u64_any, "052");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_u64);
	TOGO_ASSERTE(r.u == 052);
}

DEBUG_PRINT_PARSER(PDef::s64_dec);
{
	PARSE_S(PDef::s64_dec, "-42");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_s64);
	TOGO_ASSERTE(r.i == -42);
}

DEBUG_PRINT_PARSER(PDef::s64_any);
{
	PARSE_S(PDef::s64_any, "-42");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_s64);
	TOGO_ASSERTE(r.i == -42);
}

{
	PARSE_S(PDef::s64_any, "-0x2a");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_s64);
	TOGO_ASSERTE(r.i == -0x2a);
}

{
	PARSE_S(PDef::s64_any, "-052");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_s64);
	TOGO_ASSERTE(r.i == -052);
}

DEBUG_PRINT_PARSER(PDef::f64_basic);
{
	PARSE_S(PDef::f64_basic, "0.0");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_f64);
	TOGO_ASSERTE(float_equal_exact(r.f, 0.0));
}

{
	PARSE_S(PDef::f64_basic, "1.0");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_f64);
	TOGO_ASSERTE(float_equal_exact(r.f, 1.0));
}

{
	PARSE_S(PDef::f64_basic, "-42.42");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_f64);
	TOGO_ASSERTE(float_equal_exact(r.f, -42.42));
}

DEBUG_PRINT_PARSER(PDef::f64_exp);
{
	PARSE_S(PDef::f64_exp, "42.42e6");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_f64);
	TOGO_ASSERTE(float_equal_exact(r.f, 42.42e6));
}

{
	PARSE_S(PDef::f64_exp, "42.42e-6");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& r = state.results[0];
	TOGO_ASSERTE(r.type == ParseResult::type_f64);
	TOGO_ASSERTE(float_equal_exact(r.f, 42.42e-6));
}

	memory::shutdown();
	return 0;
}
