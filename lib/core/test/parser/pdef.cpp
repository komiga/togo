
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

signed main() {
	memory::init();

#if defined(TOGO_DEBUG)
	parser::s_debug_trace = true;
#endif

	FixedAllocator<1024 * 4> state_storage;
	ParseError error{};
	ParseState state{state_storage, &error};

	DEBUG_PRINT_PARSER(PDef::null);
	DEBUG_PRINT_PARSER(PDef::boolean);

	DEBUG_PRINT_PARSER(PDef::digit_dec);
	DEBUG_PRINT_PARSER(PDef::digits_dec);
	DEBUG_PRINT_PARSER(PDef::digit_hex);
	DEBUG_PRINT_PARSER(PDef::digits_hex);
	DEBUG_PRINT_PARSER(PDef::digit_oct);
	DEBUG_PRINT_PARSER(PDef::digits_oct);

	DEBUG_PRINT_PARSER(PDef::u64_dec);
	DEBUG_PRINT_PARSER(PDef::u64_hex);
	DEBUG_PRINT_PARSER(PDef::u64_oct);
	DEBUG_PRINT_PARSER(PDef::u64_any);

	DEBUG_PRINT_PARSER(PDef::s64_dec);
	DEBUG_PRINT_PARSER(PDef::s64_any);

{
	PARSE_S(PDef::null, "null");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& v = state.results[0];
	TOGO_ASSERTE(v.type == ParseResult::type_null);
}

{
	PARSE_S(PDef::boolean, "true");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& v = state.results[0];
	TOGO_ASSERTE(v.type == ParseResult::type_bool);
	TOGO_ASSERTE(v.v.b == true);
}

{
	PARSE_S(PDef::boolean, "false");
	TOGO_ASSERTE(!!error.result_code);
	TOGO_ASSERTE(size(state.results) == 1);
	auto& v = state.results[0];
	TOGO_ASSERTE(v.type == ParseResult::type_bool);
	TOGO_ASSERTE(v.v.b == false);
}

	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_dec, "0"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_dec, "9"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_dec, "00"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_dec, "99"));

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

	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_oct, "0"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digit_oct, "7"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_oct, "00"));
	TOGO_ASSERTE(TEST_WHOLE(PDef::digits_oct, "77"));

	memory::shutdown();
	return 0;
}
