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

#include <cstdlib>

namespace togo {

namespace parser {

#if defined(TOGO_DEBUG)
bool s_debug_trace = false;
#endif

namespace {

static FixedParserAllocator<18, 16, 10> pdef_storage;

} // anonymous namespace

#define TOGO_PDEF(name_, ...) \
Parser const PDef :: name_{"PDef." #name_, __VA_ARGS__};

TOGO_PDEF(whitespace, PMod::test, Func{
[](Parser const*, ParseState& s, ParsePosition const& from) {
	while (s.p < s.e) {
		signed c = *s.p;
		if (!(false
			|| c == ' '
			|| c == '\t'
			|| c == '\n'
			|| c == '\r'
		)) {
			break;
		}
		++s.p;
	}
	if (from.p < s.p) {
		return ok(s);
	}
	return fail(s);
}
})

TOGO_PDEF(whitespace_maybe, PMod::maybe, Ref{PDef::whitespace})

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

TOGO_PDEF(sign, Any{pdef_storage,
	Char{'-'},
	Char{'+'}
})
TOGO_PDEF(sign_maybe, PMod::maybe, Ref{PDef::sign})

TOGO_PDEF(digit_dec, CharRange{'0', '9'})
TOGO_PDEF(digits_dec, PMod::repeat, Ref{PDef::digit_dec})

TOGO_PDEF(digit_hex, Any{pdef_storage,
PDef::digit_dec,
CharRange{'a', 'f'},
CharRange{'A', 'F'}
})
TOGO_PDEF(digits_hex, PMod::repeat, Ref{PDef::digit_hex})

TOGO_PDEF(digit_oct, CharRange{'0', '7'})
TOGO_PDEF(digits_oct, PMod::repeat, Ref{PDef::digit_oct})

TOGO_PDEF(u64_dec, PMod::flatten, Close{
PDef::digits_dec,
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto& r = back(s.results);
	auto e = r.s.e;
	u64 value = 0;
	for (auto p = r.s.b; p < e; ++p) {
		value = (value << 1) + (value << 3) /*mul 10*/ + (*p - '0');
	}
	return ok_replace(s, from, {value});
}
})

TOGO_PDEF(u64_hex, PMod::flatten, Close{pdef_storage,
All{pdef_storage,
	Any{pdef_storage, String{"0x"}, String{"0X"}},
	PDef::digits_hex
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto const& r = back(s.results);
	u64 value = 0;
	auto p = r.s.b + 2;
	auto e = r.s.e;
	for (; p < e && *p == '0'; ++p) {}
	for (; p < e; ++p) {
		unsigned d = *p;
		if (d <= '9') {
			d -= '0';
		} else if (d <= 'F') {
			d = d - 'A' + 10;
		} else {
			d = d - 'a' + 10;
		}
		value = (value << 4) /*mul 16*/ + d;
	}
	return ok_replace(s, from, {value});
}
})

TOGO_PDEF(u64_oct, PMod::flatten, Close{pdef_storage,
All{pdef_storage,
	Char{'0'},
	Parser{PMod::maybe, Ref{PDef::digits_oct}}
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto const& r = back(s.results);
	u64 value = 0;
	auto p = r.s.b + 1;
	auto e = r.s.e;
	for (; p < e && *p == '0'; ++p) {}
	for (; p < e; ++p) {
		value = (value * 8) + (*p - '0');
	}
	return ok_replace(s, from, {value});
}
})

TOGO_PDEF(u64_any, Any{pdef_storage,
PDef::u64_hex,
PDef::u64_oct,
PDef::u64_dec
})

TOGO_PDEF(s64_dec, Close{pdef_storage,
All{pdef_storage,
	PDef::sign_maybe,
	PDef::u64_dec
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	bool sign = false;
	if ((array::size(s.results) - from.i) == 2) {
		sign = s.results[from.i].c == '-';
	}
	s64 value = static_cast<s64>(back(s.results).u);
	return ok_replace(s, from, {sign ? -value : value});
}
})

TOGO_PDEF(s64_any, Close{pdef_storage,
All{pdef_storage,
	PDef::sign_maybe,
	Any{pdef_storage,
		PDef::u64_hex,
		PDef::u64_oct,
		PDef::s64_dec
	}
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	bool sign = false;
	if ((array::size(s.results) - from.i) == 2) {
		sign = s.results[from.i].c == '-';
	}
	s64 value = static_cast<s64>(back(s.results).u);
	return ok_replace(s, from, {sign ? -value : value});
}
})

namespace {

inline static f64 parse_f64(StringRef str) {
	FixedArray<char, 64> cstr;
	string::copy(cstr, str);
	char* end = nullptr;
	f64 value = std::strtod(begin(cstr), &end);
	TOGO_DEBUG_ASSERTE(end == &back(cstr));
	return value;
}

} // anonymous namespace

TOGO_PDEF(f64_basic, PMod::flatten, Close{pdef_storage,
All{pdef_storage,
	PDef::sign_maybe,
	PDef::digits_dec,
	Char{'.'},
	PDef::digits_dec
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto& r = back(s.results);
	return ok_replace(s, from, {parse_f64({r.s.b, r.s.e})});
}
})

TOGO_PDEF(f64_exp, PMod::flatten, Close{pdef_storage,
All{pdef_storage,
	PDef::f64_basic,
	Parser{PMod::maybe, All{pdef_storage,
		Any{pdef_storage, Char{'e'}, Char{'E'}},
		PDef::sign_maybe,
		PDef::digits_dec
	}}
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto& r = back(s.results);
	return ok_replace(s, from, {parse_f64({r.s.b, r.s.e})});
}
})

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

	"user",
};
static_assert(array_extent(s_value_type_name) == (ParseResult::type_user_base + 1), "");

#define PARSE_TRACE_PRINTF(f_, ...) do { \
	if (s_debug_trace) { TOGO_LOGF(f_, __VA_ARGS__); } \
} while (false)

#define PARSE_TRACE(f_) \
	PARSE_TRACE_PRINTF("parse trace: %*.d" f_, s_debug_trace_depth * 2, 0)
#define PARSE_TRACEF(f_, ...) \
	PARSE_TRACE_PRINTF("parse trace: %*.d" f_, s_debug_trace_depth * 2, 0, __VA_ARGS__)

#define PARSE_TRACE_EXIT_STUFF() do { \
	bool _any = array::any(s.results); \
	auto _vt = _any ? array::back(s.results).type : ParseResult::type_null; \
	_vt = (_vt >= ParseResult::type_user_base) ? ParseResult::type_user_base : _vt; \
	PARSE_TRACEF( \
		"= %s (%lu%s%s)", \
		s_result_code_name[unsigned_cast(rc)], \
		array::size(s.results), \
		_any ? ", " : "", \
		_any ? s_value_type_name[_vt] : "" \
	); \
} while (false)

#define PARSE_DEBUG_PRINT_DECL(p_) do { \
	auto& _p = p_; \
	StringRef _n = _p.name; \
	StringRef _t = parser::type_name(_p); \
	auto _o = parser::modifiers(_p); \
	if (_n.any()) { TOGO_LOGF("%.*s = ", _n.size, _n.data); } \
	TOGO_LOGF("%.*s", _t.size, _t.data); \
	if (_o != PMod::none) { \
		TOGO_LOGF("%s%s%s%s%s", \
			enum_bool(_o & PMod::maybe) ? ":m" : "", \
			enum_bool(_o & PMod::test) ? ":t" : "", \
			enum_bool(_o & PMod::flatten) ? ":f" : "", \
			enum_bool(_o & (PMod::repeat | PMod::repeat_or_none)) ? ":r" : "", \
			enum_bool(_o & PMod::repeat_or_none) ? "n" : "" \
		); \
	} \
} while (false)

static void debug_print_shallow(Parser const& p) {
	PARSE_DEBUG_PRINT_DECL(p);
	switch (type(p)) {
	case ParserType::Undefined:
	case ParserType::Nothing:
	case ParserType::Empty:
	case ParserType::Head:
	case ParserType::Tail:
		TOGO_LOG("\n");
		break;

	case ParserType::Char: TOGO_LOGF("{'%c'}\n", p.s.Char.c); break;
	case ParserType::CharRange: TOGO_LOGF("{'%c' - '%c'}\n", p.s.CharRange.b, p.s.CharRange.e); break;
	case ParserType::String: TOGO_LOGF("{`%.*s`}\n", p.s.String.s.size, p.s.String.s.data); break;

	case ParserType::Bounded: {
		auto& d = p.s.Bounded;
		TOGO_LOGF("{'%c', '%c', ", d.opener, d.closer);
		PARSE_DEBUG_PRINT_DECL(*d.p);
		TOGO_LOG("}\n");
	}	break;

	case ParserType::Any: 
	case ParserType::All: {
		TOGO_LOG("{");
		auto& d = p.s.Any;
		for (unsigned i = 0; i < d.num;) {
			PARSE_DEBUG_PRINT_DECL(*d.p[i]);
			if (++i < d.num) {
				TOGO_LOG(", ");
			}
		}
		TOGO_LOG("}\n");
	}	break;

	case ParserType::Ref: {
		auto& d = p.s.Ref;
		TOGO_LOG("{");
		PARSE_DEBUG_PRINT_DECL(*d.p);
		TOGO_LOG("}\n");
	}	break;

	case ParserType::Func: {
		auto& d = p.s.Func;
		TOGO_LOGF("{0x%08lx, 0x%08lx}\n",
			reinterpret_cast<std::uintptr_t>(d.f),
			reinterpret_cast<std::uintptr_t>(d.userdata)
		);
	}	break;

	case ParserType::Close: {
		auto& d = p.s.Close;
		TOGO_LOGF("{0x%08lx, ", reinterpret_cast<std::uintptr_t>(d.f));
		PARSE_DEBUG_PRINT_DECL(*d.p);
		TOGO_LOG("}\n");
	}	break;
	}
}

#else

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

static Parser const s_parser_begin{Head{}};
static Parser const s_parser_end{Tail{}};

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
	} else if (enum_bool(mods & PMod::repeat_or_none)) {
		suppress_errors(s);
		parser::parse_impl(p, s, from, type, (mods & ~PMod::repeat_or_none) | PMod::repeat);
		unsuppress_errors(s);
		PARSE_RESULT(ok(s));
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

	case ParserType::Head:
		if (s.b == s.p) {
			PARSE_RESULT(ok(s));
		}
		PARSE_RESULT(fail(s, "expected beginning of input"));

	case ParserType::Tail:
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

	case ParserType::Bounded: {
		auto const& d = p.s.Bounded;
		if (*s.p != d.opener) {
			PARSE_RESULT(fail(s, "expected bound opener: '%c'", d.opener));
		}
		++s.p;
		auto rc = parser::parse_do(*d.p, s);
		if (rc != ParseResultCode::ok) {
			PARSE_RESULT(rc);
		}
		if (*s.p != d.closer) {
			PARSE_RESULT(fail(s, "expected bound closer: '%c'", d.closer));
		}
		++s.p;
		PARSE_RESULT(ok(s));
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

	case ParserType::Func: {
		auto const& d = p.s.Func;
		TOGO_DEBUG_ASSERTE(d.f);
		PARSE_RESULT(d.f(&p, s, from));
	}

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
	case ParserType::Head:
	case ParserType::Tail:
	case ParserType::Char:
	case ParserType::CharRange:
	case ParserType::String:
	case ParserType::Bounded:
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

	case ParserType::Func:
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
		"Head",
		"Tail",

		"Char",
		"CharRange",
		"String",
		"Bounded",

		"Any",
		"All",

		"Ref",

		"Func",

		"Close",
	};
	static_assert(array_extent(s_type_name) == parser::c_num_types, "");
	TOGO_DEBUG_ASSERTE(unsigned_cast(type) < parser::c_num_types);
	return s_type_name[unsigned_cast(type)];
}

/// Run parser.
ParseResultCode parser::parse_do(Parser const& p, ParseState& s) {
	auto type = parser::type(p);
	auto mods = parser::modifiers(p);

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
	if (
		s.p == s.e &&
		type > ParserType::Tail && type < ParserType::Any &&
		!enum_bool(mods & (PMod::none
			| PMod::maybe
			| PMod::repeat
			| PMod::repeat_or_none
		))
	) {
		rc = fail(s, "no more input");
	} else {
		rc = parse_impl(p, s, from, type, parser::modifiers(p), true);
	}

#if defined(TOGO_DEBUG)
	if (s_debug_trace) {
		PARSE_TRACE_EXIT_STUFF();
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
		PARSE_TRACE_EXIT_STUFF();
		if (!rc && s.error) {
			TOGO_LOGF("  =>  %u:%u %.*s",
				s.error->line, s.error->column,
				static_cast<unsigned>(fixed_array::size(s.error->message)),
				begin(s.error->message)
			);
		}
		TOGO_LOG("\n");
		PARSE_TRACE("=> {");
		for (unsigned i = 0; i < size(s.results);) {
			auto const& r = s.results[i];
			if (r.type >= ParseResult::type_user_base) {
				TOGO_LOGF("user{%u}", r.type);
			} else {
				TOGO_LOGF("%s = ", s_value_type_name[back(s.results).type]);
				switch (r.type) {
				case ParseResult::type_null:
					TOGO_LOG("null");
					break;

				case ParseResult::type_bool:
					TOGO_LOGF("%s", r.b ? "true" : "false");
					break;

				case ParseResult::type_char:
					TOGO_LOGF("0x%02x '%c'", r.c, r.c);
					break;

				case ParseResult::type_s64:
					TOGO_LOGF("%ld", r.i);
					break;

				case ParseResult::type_u64:
					TOGO_LOGF("%lu", r.u);
					break;

				case ParseResult::type_f64:
					TOGO_LOGF("%.06lf", r.f);
					break;

				case ParseResult::type_pointer:
					TOGO_LOGF("{0x%08lx, ", reinterpret_cast<std::uintptr_t>(r.p));
					break;

				case ParseResult::type_slice:
					TOGO_LOGF(
						"%4lu %4lu `%.*s`",
						r.s.b - s.b,
						r.s.e - s.b,
						static_cast<unsigned>(r.s.e - r.s.b),
						r.s.b
					);
					break;

				case ParseResult::type_user_base:
					TOGO_DEBUG_ASSERTE(false);
					break;
				}
			}
			if (++i < size(s.results)) {
				TOGO_LOG(", ");
			}
		}
		TOGO_LOG("}\n");
	}
#endif

	return rc == ParseResultCode::ok;
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