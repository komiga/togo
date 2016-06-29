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

static FixedParserAllocator<10, 11, 8> pdef_storage;

} // anonymous namespace

#define TOGO_PDEF(name_, ...) \
Parser const PDef :: name_{"PDef." #name_, __VA_ARGS__};

TOGO_PDEF(null, PMod::test, Close{pdef_storage,
String{"null"},
[](Parser const*, ParseState& s, ParsePosition const&) {
	return ok(s, {null_tag{}});
}
})

TOGO_PDEF(boolean, Any{pdef_storage,
Parser{PMod::test, Close{pdef_storage,
	String{"true"},
	[](Parser const*, ParseState& s, ParsePosition const&) {
		return ok(s, {true});
	}
}},
Parser{PMod::test, Close{pdef_storage,
	String{"false"},
	[](Parser const*, ParseState& s, ParsePosition const&) {
		return ok(s, {false});
	}
}}
})

TOGO_PDEF(digit_dec, CharRange{'0', '9'});
TOGO_PDEF(digits_dec, PMod::repeat, Ref{PDef::digit_dec});

TOGO_PDEF(digit_hex, Any{pdef_storage,
PDef::digit_dec,
CharRange{'a', 'f'},
CharRange{'A', 'F'}
});
TOGO_PDEF(digits_hex, PMod::repeat, Ref{PDef::digit_hex});

TOGO_PDEF(digit_oct, CharRange{'0', '7'});
TOGO_PDEF(digits_oct, PMod::repeat, Ref{PDef::digit_oct});

TOGO_PDEF(u64_dec, PMod::flatten, Close{
PDef::digits_dec,
[](Parser const*, ParseState& s, ParsePosition const&) {
	auto const& r = back(s.results);
	auto e = r.v.s.e;
	u64 value = 0;
	for (auto p = r.v.s.b; p < e; ++p) {
		value = (value << 1) + (value << 3) /*mul 10*/ + (*p - '0');
	}
	return ok(s, {value});
}
});

TOGO_PDEF(u64_hex, PMod::flatten, Close{pdef_storage,
All{pdef_storage,
	Any{pdef_storage, String{"0x"}, String{"0X"}},
	PDef::digits_hex
},
[](Parser const*, ParseState& s, ParsePosition const&) {
	auto const& r = back(s.results);
	u64 value = 0;
	auto p = r.v.s.b + 2;
	auto e = r.v.s.e;
	for (; p < e && *p == '0'; ++p) {}
	for (; p < e; ++p) {
		char c = *p;
		if (c <= '9') {
			c -= '0';
		} else if (c <= 'F') {
			c -= 'A';
		} else {
			c -= 'a';
		}
		value = (value << 4) /*mul 16*/ + c;
	}
	return ok(s, {value});
}
});

TOGO_PDEF(u64_oct, PMod::flatten, Close{pdef_storage,
All{pdef_storage,
	Char{'0'},
	PDef::digits_oct
},
[](Parser const*, ParseState& s, ParsePosition const&) {
	auto const& r = back(s.results);
	u64 value = 0;
	auto p = r.v.s.b + 1;
	auto e = r.v.s.e;
	for (; p < e && *p == '0'; ++p) {}
	for (; p < e; ++p) {
		value = (value * 7) + (*p - '0');
	}
	return ok(s, {value});
}
});

TOGO_PDEF(u64_any, Any{pdef_storage,
PDef::u64_hex,
PDef::u64_oct,
PDef::u64_dec
});

TOGO_PDEF(s64_dec, Close{pdef_storage,
All{pdef_storage,
	Parser{PMod::maybe, Any{pdef_storage,
		Char{'-'}, Char{'+'}
	}},
	PDef::u64_dec
},
[](Parser const*, ParseState& s, ParsePosition const& pos) {
	auto const& r = back(s.results);
	bool sign = false;
	if ((array::size(s.results) - pos.i) == 2) {
		sign = s.results[pos.i].v.c == '-';
	}
	s64 value = sign ? -r.v.u : r.v.u;
	return ok(s, {value});
}
});

TOGO_PDEF(s64_any, Close{pdef_storage,
Any{pdef_storage,
	PDef::u64_hex,
	PDef::u64_oct,
	PDef::s64_dec
},
[](Parser const*, ParseState& s, ParsePosition const&) {
	auto& r = back(s.results);
	r.type = ParseResult::type_s64;
	return ok(s);
}
});

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

static char const* const s_value_type_name[]{
	"null",
	"bool",
	"char",
	"s64",
	"u64",
	"f64",
	"pointer",
	"slice",
};
static_assert(array_extent(s_value_type_name) == ParseResult::type_user_base, "");

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

//p_.name.any() ? p_.name : "<no-name>";

#define PARSE_DEBUG_PRINT_DECL(p_) do { \
	auto& _p = p_; \
	StringRef _n = _p.name; \
	StringRef _t = parser::type_name(_p); \
	auto _o = parser::modifiers(_p); \
	if (_n.any()) { TOGO_LOGF("%.*s = ", _n.size, _n.data); } \
	TOGO_LOGF("%.*s", _t.size, _t.data); \
	if (_o != PMod::none) { \
		TOGO_LOGF("%s%s%s%s", \
			enum_bool(_o & PMod::maybe) ? ":m" : "", \
			enum_bool(_o & PMod::test) ? ":t" : "", \
			enum_bool(_o & PMod::flatten) ? ":f" : "", \
			enum_bool(_o & PMod::repeat) ? ":r" : "" \
		); \
	} \
} while (false)

static void debug_print_shallow(Parser const& p) {
	PARSE_DEBUG_PRINT_DECL(p);
	switch (type(p)) {
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
			PARSE_DEBUG_PRINT_DECL(*d.p[i]);
			TOGO_LOG((++i < d.num) ? ", " : "");
		}
		TOGO_LOG("}\n");
	}	break;

	case ParserType::Ref: {
		auto& d = p.s.Ref;
		TOGO_LOG("{");
		PARSE_DEBUG_PRINT_DECL(*d.p);
		TOGO_LOG("}\n");
	}	break;

	case ParserType::Close: {
		auto& d = p.s.Close;
		TOGO_LOGF("{0x%08lx, ", reinterpret_cast<std::uintptr_t>(d.p));
		PARSE_DEBUG_PRINT_DECL(*d.p);
		TOGO_LOG("}\n");
	}	break;
	}
}

#else

// #define PARSE_TRACE_PRINT(f_) (void(0))
#define PARSE_TRACE_PRINTF(f_, ...) (void(0))
#define PARSE_TRACE(f_) (void(0))
#define PARSE_TRACEF(f_, ...) (void(0))

#endif

#define PARSE_RESULT(expr_) do { \
	_result_rc = expr_; \
	if (top_level && _result_rc == ParseResultCode::ok) { \
		goto l_finalize; \
	} \
	return _result_rc; \
} while (false)

static Parser const s_parser_begin{Begin{}};
static Parser const s_parser_end{End{}};

static ParseResultCode parse_impl(
	Parser const& p,
	ParseState& s,
	ParsePosition const& from,
	ParserType const type,
	ParserModifier const mods,
	bool const top_level = false
) {
	ParseResultCode _result_rc;

	if (mods == PMod::none) {
		// pass
	} else if (enum_bool(mods & PMod::maybe)) {
		suppress_errors(s);
		auto rc = parser::parse_impl(p, s, from, type, mods & ~PMod::maybe);
		unsuppress_errors(s);
		if (!!rc) {
			TOGO_DEBUG_ASSERTE(rc != ParseResultCode::no_match || (from.p == s.p && from.i == s.results._size));
			PARSE_RESULT(rc);
		}
		set_position(s, from);
		PARSE_RESULT(no_match(s));
	} else if (enum_bool(mods & PMod::test)) {
		suppress_results(s);
		auto rc = parser::parse_impl(p, s, from, type, mods & ~PMod::test);
		unsuppress_results(s);
		PARSE_RESULT(rc);
	} else if (enum_bool(mods & PMod::flatten)) {
		suppress_results(s);
		auto rc = parser::parse_impl(p, s, from, type, mods & ~PMod::flatten);
		unsuppress_results(s);
		if (rc == ParseResultCode::ok) {
			PARSE_RESULT(ok(s, {from.p, s.p}));
		}
		PARSE_RESULT(rc);
	} else if (enum_bool(mods & PMod::repeat)) {
		auto const mods_repeat = mods & ~PMod::repeat;
		auto rc = parser::parse_impl(p, s, from, type, mods_repeat);
		if (rc == ParseResultCode::ok) {
			suppress_errors(s);
			do {
				rc = parser::parse_impl(p, s, from, type, mods_repeat);
			} while (rc == ParseResultCode::ok);
			unsuppress_errors(s);
			PARSE_RESULT(ok(s));
		}
		PARSE_RESULT(fail(s));
	}

	switch (type) {
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

	case ParserType::Ref:
		PARSE_RESULT(parser::parse_do(*p.s.Ref.p, s));

	case ParserType::Close: {
		auto const& d = p.s.Close;
		TOGO_DEBUG_ASSERTE(d.f);
		if (d.p && parser::parse_do(*d.p, s) != ParseResultCode::ok) {
			PARSE_RESULT(fail_expected_sub_match(s, "Close"));
		}
		PARSE_RESULT(ok(s));
	}
	}
	TOGO_DEBUG_ASSERTE(false);
	PARSE_RESULT(fail(s));

l_finalize:
	if (type == ParserType::Close) {
		if (!s.suppress_results) {
			auto const& d = p.s.Close;
			_result_rc = d.f(d.p, s, from);
		}
	}
	return _result_rc;
}

} // anonymous namespace
} // namespace parser

#if defined(TOGO_DEBUG)

IGEN_PRIVATE
void parser::debug_print_tree(Parser const& p, unsigned tab IGEN_DEFAULT(0)) {
	TOGO_LOGF("%*.d+ ", tab * 2, 0);
	debug_print_shallow(p);
	++tab;
	switch (type(p)) {
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

	case ParserType::Ref:
		parser::debug_print_tree(*p.s.Ref.p, tab);
		break;

	case ParserType::Close:
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

		"Ref",

		"Close",
	};
	static_assert(array_extent(s_type_name) == parser::c_num_types, "");
	TOGO_DEBUG_ASSERTE(unsigned_cast(type) < parser::c_num_types);
	return s_type_name[unsigned_cast(type)];
}

/// Run parser.
ParseResultCode parser::parse_do(Parser const& p, ParseState& s) {
	auto type = parser::type(p);

#if defined(TOGO_DEBUG)
	if (s_debug_trace) {
		PARSE_TRACE("+ ");
		debug_print_shallow(p);
		++s_debug_trace_depth;
		if (type < ParserType::Any) {
			char c = s.p < s.e ? *s.p : '\0';
			PARSE_TRACEF("? %4lu %4lu  0x%02x '%c'\n", s.p - s.b, s.e - s.p, c, c);
		}
	}
#endif

	auto const from = position(s);
	ParseResultCode rc;
	if (s.p == s.e && type > ParserType::End && type < ParserType::Any) {
		rc = fail(s, "no more input");
	} else {
		rc = parse_impl(p, s, from, type, parser::modifiers(p), true);
	}

#if defined(TOGO_DEBUG)
	if (s_debug_trace) {
		PARSE_TRACEF(
			"= %s (%lu, %s)",
			s_result_code_name[unsigned_cast(rc)],
			array::size(s.results),
			array::any(s.results) ? s_value_type_name[back(s.results).type] : ""
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
			"= %s (%lu, %s)",
			s_result_code_name[unsigned_cast(rc)],
			array::size(s.results),
			array::any(s.results) ? s_value_type_name[back(s.results).type] : ""
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
