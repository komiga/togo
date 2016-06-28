#line 2 "togo/core/parser/parser.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/assert_allocator.hpp>
#include <togo/core/memory/fixed_allocator.hpp>
#include <togo/core/memory/temp_allocator.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/parser/types.hpp>
#include <togo/core/parser/parser.hpp>
#include <togo/core/parser/parse_state.hpp>

namespace togo {

namespace parser {

#if defined(TOGO_DEBUG)
bool s_debug_trace = false;
#endif

namespace {
	FixedParserAllocator<1, 2 + 2, 1 + 2> pdef_storage;
} // anonymous namespace

#define TOGO_PDEF(name_, ...) \
Parser const PDef :: name_{"PDef::" #name_, __VA_ARGS__};

TOGO_PDEF(null, CloseTest{pdef_storage,
	[](Parser const*, ParseState& s, ParsePosition const&) {
		return ok(s, {null_tag{}});
	},
	String{"null"}
})

TOGO_PDEF(boolean, Any{pdef_storage,
	CloseTest{pdef_storage, [](Parser const*, ParseState& s, ParsePosition const&) {
		return ok(s, {true});
	}, String{"true"}},
	CloseTest{pdef_storage, [](Parser const*, ParseState& s, ParsePosition const&) {
		return ok(s, {false});
	}, String{"false"}}
})

TOGO_PDEF(digit_dec, CharRange{'0', '9'});
TOGO_PDEF(digits_dec, Repeat{PDef::digit_dec});

TOGO_PDEF(digit_hex, Any{pdef_storage,
	digit_dec,
	CharRange{'a', 'f'},
	CharRange{'A', 'F'}
});
TOGO_PDEF(digits_hex, Repeat{PDef::digit_hex});

TOGO_PDEF(digit_oct, CharRange{'0', '7'});
TOGO_PDEF(digits_oct, Repeat{PDef::digit_oct});

TOGO_PDEF(u64_dec, Undefined{}/*CloseFlatten{pdef_storage, PDef::digits_dec}*/);
TOGO_PDEF(u64_hex, Undefined{});
TOGO_PDEF(u64_oct, Undefined{});

TOGO_PDEF(u64_any, Undefined{});
TOGO_PDEF(s64_dec, Undefined{});
TOGO_PDEF(s64_any, Undefined{});

#undef TOGO_PDEF

namespace {

#if defined(TOGO_DEBUG)
static thread_local unsigned s_debug_trace_depth = 0;
static thread_local bool s_debug_branch_show_error = false;

static char const* const s_result_code_name[]{
	"fail",
	"ok",
	"no_match",
};
static_assert(array_extent(s_result_code_name) == parse_state::c_num_result_codes, "");

/*#define PARSE_TRACE_PRINT(f_) do { \
	if (s_debug_trace) { TOGO_LOG(f_); } \
} while (false)*/

#define PARSE_TRACE_PRINTF(f_, ...) do { \
	if (s_debug_trace) { TOGO_LOGF(f_, __VA_ARGS__); } \
} while (false)

#define PARSE_TRACE(f_) \
	PARSE_TRACE_PRINTF("parse trace: %*.d" f_, s_debug_trace_depth * 2, 0)
#define PARSE_TRACEF(f_, ...) \
	PARSE_TRACE_PRINTF("parse trace: %*.d" f_, s_debug_trace_depth * 2, 0, __VA_ARGS__)

#define DECL_FORMAT "%.*s%s%.*s"

//p_.name.any() ? p_.name : "<no-name>";
#define PARSE_DEBUG_PRINT_DECL(f_, p_) do { \
	StringRef _n = (p_).name; \
	StringRef _t = parser::type_name(p_); \
	TOGO_LOGF(f_, \
		_n.size, _n.data, \
		_n.any() ? " = " : "", \
		_t.size, _t.data \
	); \
} while (false)

#define PARSE_DEBUG_PRINT_DECLF(f_, p_, ...) do { \
	StringRef _n = (p_).name; \
	StringRef _t = parser::type_name(p_); \
	TOGO_LOGF(f_, \
		_n.size, _n.data, _n.any() ? " = " : "", \
		_t.size, _t.data, \
		__VA_ARGS__ \
	); \
} while (false)

static void debug_print_shallow(Parser const& p) {
	PARSE_DEBUG_PRINT_DECL(DECL_FORMAT, p);
	switch (p.type) {
	case ParserType::Undefined:
	case ParserType::Nothing:
	case ParserType::Empty:
	case ParserType::Begin:
	case ParserType::End:
		TOGO_LOG("\n");
		break;

	case ParserType::Char: TOGO_LOGF("{'%c'}\n", p.s.Char.c); break;
	case ParserType::CharRange: TOGO_LOGF("{'%c' - '%c'}\n", p.s.CharRange.b, p.s.CharRange.e); break;
	case ParserType::String: TOGO_LOGF("{`%.*s`}\n", p.s.String.s.size, p.s.String.s.data); break;

	case ParserType::Any: 
	case ParserType::All: {
		TOGO_LOG("{");
		auto& d = p.s.Any;
		for (unsigned i = 0; i < d.num;) {
			PARSE_DEBUG_PRINT_DECLF(
				DECL_FORMAT "%s",
				*d.p[i],
				(++i < d.num) ? ", " : ""
			);
		}
		TOGO_LOG("}\n");
	}	break;

	case ParserType::Maybe:
	case ParserType::Repeat: {
		auto& d = p.s.Maybe;
		PARSE_DEBUG_PRINT_DECL("{" DECL_FORMAT "}\n", *d.p);
	}	break;

	case ParserType::Close:
	case ParserType::CloseTest: {
		auto& d = p.s.Close;
		TOGO_LOGF("{0x%08lx, ", reinterpret_cast<std::uintptr_t>(d.p));
		PARSE_DEBUG_PRINT_DECL(DECL_FORMAT "}\n", *d.p);
	}	break;
	}
}

#else

// #define PARSE_TRACE_PRINT(f_) (void(0))
#define PARSE_TRACE_PRINTF(f_, ...) (void(0))
#define PARSE_TRACE(f_) (void(0))
#define PARSE_TRACEF(f_, ...) (void(0))

#endif

#define PARSE_RESULT(expr_) return expr_

static Parser const s_parser_begin{Begin{}};
static Parser const s_parser_end{End{}};

static ParseResultCode parse_impl(ParseState& s, Parser const& p, ParsePosition const& from) {
	switch (p.type) {
	case ParserType::Undefined:
		TOGO_ASSERT(false, "tried to parse undefined parser");

	case ParserType::Nothing:
		PARSE_RESULT(ok(s));

	case ParserType::Empty:
		if (s.b == s.p && s.p == s.e) {
			PARSE_RESULT(ok(s));
		}
		PARSE_RESULT(fail(s, "expected empty input"));

	case ParserType::Begin:
		if (s.b == s.p) {
			PARSE_RESULT(ok(s));
		}
		PARSE_RESULT(fail(s, "expected beginning of input"));

	case ParserType::End:
		if (s.p == s.e) {
			PARSE_RESULT(ok(s));
		}
		PARSE_RESULT(fail(s, "expected end of input"));

	case ParserType::Char: {
		char c = *s.p;
		if (c == p.s.Char.c) {
			++s.p;
			PARSE_RESULT(ok(s, {c}));
		}
		PARSE_RESULT(fail(s, "expected '%c'", p.s.Char.c));
	}

	case ParserType::CharRange: {
		auto const& d = p.s.CharRange;
		TOGO_DEBUG_ASSERTE(d.b <= d.e);
		char c = *s.p;
		if (d.b <= c && c <= d.e) {
			++s.p;
			PARSE_RESULT(ok(s, {c}));
		}
		PARSE_RESULT(fail(s,
			"expected within range '%c' - '%c'",
			static_cast<char>(d.b),
			static_cast<char>(d.e)
		));
	}

	case ParserType::String: {
		auto const& d = p.s.String;
		if (string::compare_equal({s.p, min(d.s.size, static_cast<unsigned>(s.e - s.p))}, d.s)) {
			s.p += p.s.String.s.size;
			PARSE_RESULT(ok(s, {from.p, s.p}));
		}
		PARSE_RESULT(fail(s, "expected '%.*s'", d.s.size, d.s.data));
	}

	case ParserType::Any: {
		suppress_errors(s);
		auto const& d = p.s.Any;
		for (unsigned i = 0; i < d.num; ++i) {
			if (!!parser::parse_do(*d.p[i], s)) {
				unsuppress_errors(s);
				PARSE_RESULT(ok(s));
			}
			set_position(s, from);
		}
		unsuppress_errors(s);
		PARSE_RESULT(fail(s, "no match in Any"));
	}

	case ParserType::All: {
		auto const& d = p.s.All;
		for (unsigned i = 0; i < d.num; ++i) {
			if (!parser::parse_do(*d.p[i], s)) {
				PARSE_RESULT(fail(s));
			}
		}
		PARSE_RESULT(ok(s));
	}

	case ParserType::Maybe: {
		auto const& d = p.s.Maybe;
		suppress_errors(s);
		auto rc = parser::parse_do(*d.p, s);
		unsuppress_errors(s);
		if (!!rc) {
			TOGO_DEBUG_ASSERTE(rc != ParseResultCode::no_match || (from.p == s.p && from.i == s.results._size));
			PARSE_RESULT(rc);
		}
		set_position(s, from);
		PARSE_RESULT(no_match(s));
	}

	case ParserType::Repeat: {
		auto const& d = p.s.Repeat;
		auto const& sub = *d.p;
		ParseResultCode rc = parser::parse_do(sub, s);
		if (rc == ParseResultCode::ok) {
			suppress_errors(s);
			do { rc = parser::parse_do(sub, s); } while (rc == ParseResultCode::ok);
			unsuppress_errors(s);
		} else {
			PARSE_RESULT(fail(s));
		}
		PARSE_RESULT(ok(s));
	}

	case ParserType::Close: {
		auto const& d = p.s.Close;
		TOGO_DEBUG_ASSERTE(d.f);
		if (d.p && parser::parse_do(*d.p, s) != ParseResultCode::ok) {
			PARSE_RESULT(fail_expected_sub_match(s, "Close"));
		}
		PARSE_RESULT(d.f(d.p, s, from));
	}

	case ParserType::CloseTest: {
		auto const& d = p.s.CloseTest;
		TOGO_DEBUG_ASSERTE(d.f);
		suppress_results(s);
		if (d.p && parser::parse_do(*d.p, s) != ParseResultCode::ok) {
			unsuppress_results(s);
			PARSE_RESULT(fail_expected_sub_match(s, "CloseTest"));
		}
		unsuppress_results(s);
		PARSE_RESULT(d.f(d.p, s, from));
	}
	}
	TOGO_DEBUG_ASSERTE(false);
	PARSE_RESULT(fail(s));
}

} // anonymous namespace
} // namespace parser

#if defined(TOGO_DEBUG)

IGEN_PRIVATE
void parser::debug_print_tree(Parser const& p, unsigned tab IGEN_DEFAULT(0)) {
	TOGO_LOGF("%*.d+ ", tab * 2, 0);
	debug_print_shallow(p);
	++tab;
	switch (p.type) {
	case ParserType::Undefined:
	case ParserType::Nothing:
	case ParserType::Empty:
	case ParserType::Begin:
	case ParserType::End:
	case ParserType::Char:
	case ParserType::CharRange:
	case ParserType::String:
		break;

	case ParserType::Any: 
	case ParserType::All:
		for (auto s : array_cref(p.s.Any.p, p.s.Any.num)) {
			parser::debug_print_tree(*s, tab);
		}
		break;

	case ParserType::Maybe:
	case ParserType::Repeat:
		parser::debug_print_tree(*p.s.Maybe.p, tab);
		break;

	case ParserType::Close:
	case ParserType::CloseTest:
		parser::debug_print_tree(*p.s.Close.p, tab);
		break;
	}
}

#endif

/// Name of parser type.
StringRef parser::type_name(ParserType type) {
	static StringRef const s_type_name[]{
		"Undefined",
		"Nothing",
		"Empty",
		"Begin",
		"End",

		"Char",
		"CharRange",
		"String",

		"Any",
		"All",

		"Maybe",
		"Repeat",

		"Close",
		"CloseTest",
	};
	static_assert(array_extent(s_type_name) == parser::c_num_types, "");
	TOGO_DEBUG_ASSERTE(unsigned_cast(type) < parser::c_num_types);
	return s_type_name[unsigned_cast(type)];
}

/// Run parser.
ParseResultCode parser::parse_do(Parser const& p, ParseState& s) {
#if defined(TOGO_DEBUG)
	if (s_debug_trace) {
		PARSE_TRACE("+ ");
		debug_print_shallow(p);
		++s_debug_trace_depth;
		if (p.type < ParserType::Any) {
			char c = s.p < s.e ? *s.p : '\0';
			PARSE_TRACEF("? %4lu %4lu  0x%02x '%c'\n", s.p - s.b, s.e - s.p, c, c);
		}
	}
#endif

	auto const from = position(s);
	ParseResultCode rc;
	if (s.p == s.e && p.type > ParserType::End && p.type < ParserType::Any) {
		rc = fail(s, "no more input");
	} else {
		rc = parse_impl(s, p, from);
	}

#if defined(TOGO_DEBUG)
	if (s_debug_trace) {
		PARSE_TRACEF(
			"= %s (%lu)",
			s_result_code_name[unsigned_cast(rc)],
			array::size(s.results)
		);
		if (!!rc) {
			s_debug_branch_show_error = false;
		}
		if (s_debug_branch_show_error) {
			TOGO_LOG("  ...\n");
		} else if (s.error && !rc) {
			s_debug_branch_show_error = true;
			TOGO_LOGF(
				"  =>  %u:%u %.*s\n",
				s.error->line, s.error->column,
				static_cast<unsigned>(fixed_array::size(s.error->message)),
				begin(s.error->message)
			);
		} else {
			TOGO_LOG("\n");
		}
		--s_debug_trace_depth;
	}
#endif

	if (s.error) {
		s.error->result_code = rc;
	}
	switch (rc) {
	case ParseResultCode::ok:
		break;

	case ParseResultCode::fail:
	case ParseResultCode::no_match:
		set_position(s, from);
		break;
	}
	return rc;
}

/// Run parser.
///
/// This will clear the results of the state before parsing.
bool parser::parse(Parser const& p, ParseState& s) {
#if defined(TOGO_DEBUG)
	if (s_debug_trace) {
		PARSE_TRACEF("parse(`%.*s`)\n", static_cast<signed>(s.e - s.b), s.b);
		s_debug_trace_depth = 1;
		s_debug_branch_show_error = false;
	}
#endif

	parse_state::clear_results(s);
	auto const rc = parser::parse_do(p, s);
	if (rc == ParseResultCode::ok) {
		update_text_position(s);
	}

#if defined(TOGO_DEBUG)
	if (s_debug_trace) {
		s_debug_trace_depth = 0;
		PARSE_TRACEF(
			"= %s (%lu)",
			s_result_code_name[unsigned_cast(rc)],
			array::size(s.results)
		);
		if (!rc && s.error) {
			TOGO_LOGF("  =>  %u:%u %.*s",
				s.error->line, s.error->column,
				static_cast<unsigned>(fixed_array::size(s.error->message)),
				begin(s.error->message)
			);
		}
		TOGO_LOG("\n\n");
	}
#endif

	return !!rc;
}

/// Run parser.
bool parser::parse(
	Parser const& p,
	ArrayRef<char const> data,
	ParseError* error IGEN_DEFAULT(nullptr),
	void* userdata IGEN_DEFAULT(nullptr)
) {
	TempAllocator<1024 * 2> a;
	ParseState s{a, error, userdata};
	parse_state::set_data_array(s, data);
	return parser::parse(p, s);
}

/// Try to match a parser.
bool parser::test(Parser const& p, ParseState& s) {
	suppress_results(s);
	bool success = parser::parse(p, s);
	unsuppress_results(s);
	return success;
}

/// Try to match a parser against the entire input.
bool parser::test_whole(Parser const& p, ParseState& s) {
	FixedParserAllocator<3> storage;
	Parser whole{All{storage, s_parser_begin, const_cast<Parser&>(p), s_parser_end}};
	return parser::test(whole, s);
}

/// Try to match a parser.
bool parser::test(
	Parser const& p,
	ArrayRef<char const> data,
	ParseError* error IGEN_DEFAULT(nullptr),
	void* userdata IGEN_DEFAULT(nullptr)
) {
	AssertAllocator a;
	ParseState s{a, error, userdata};
	parse_state::set_data_array(s, data);
	return parser::test(p, s);
}

/// Try to match a parser against the entire input.
bool parser::test_whole(
	Parser const& p,
	ArrayRef<char const> data,
	ParseError* error IGEN_DEFAULT(nullptr),
	void* userdata IGEN_DEFAULT(nullptr)
) {
	AssertAllocator a;
	ParseState s{a, error, userdata};
	parse_state::set_data_array(s, data);
	return parser::test_whole(p, s);
}

} // namespace togo
