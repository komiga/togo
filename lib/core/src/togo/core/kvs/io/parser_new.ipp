#line 2 "togo/core/kvs/io/parser_new.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/fixed_allocator.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/parser/parser.hpp>
#include <togo/core/parser/parse_state.hpp>
#include <togo/core/kvs/kvs.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace togo {

struct KVSParseState {
	unsigned current_node;
};

namespace {

constexpr static ParseResult::Type const
rtype_kvs = ParseResult::type_user_base,
rtype_string_quote = static_cast<ParseResult::Type>(rtype_kvs + 1)
;

inline KVS& parser_node(ParseState& s, unsigned i) {
	auto const& r = result(s, i);
	TOGO_DEBUG_ASSERTE(r.type == rtype_kvs);
	return *reinterpret_cast<KVS*>(r.p);
}

inline KVS& parser_node_current(ParseState& s) {
	return parser_node(s, static_cast<KVSParseState*>(s.userdata)->current_node);
}

inline f64 result_to_f64(ParseState& s, ParsePosition const& from, unsigned i) {
	auto const& r = result(s, from, i);
	return r.type == ParseResult::type_f64 ? r.f : static_cast<f64>(r.i);
}

inline char escape_char(char const c) {
	switch (c) {
	case 't': return '\t';
	case 'n': return '\n';
	case 'r': return '\r';
	default: return c;
	}
}

void string_copy_unescape(char* dst, unsigned capacity, unsigned num_escapes, StringRef src) {
	TOGO_DEBUG_ASSERTE((src.size - num_escapes) < capacity);
	auto e = end(src);
	for (auto p = begin(src); p < e; ++dst) {
		char c = *p++;
		if (c == '\\') {
			c = escape_char(*p++);
		}
		*dst = c;
	}
	*dst = '\0';
}

FixedAllocator<1536> p_storage;

#define TOGO_PARSER_DEF(name_, ...) \
	Parser const name_{"KVS." #name_, __VA_ARGS__};

TOGO_PARSER_DEF(p_comment_block,
Func{[](Parser const*, ParseState& s, ParsePosition const&) {
	if (s.p >= s.e || *s.p++ != '/') {
		return fail(s);
	}
	if (s.p >= s.e || *s.p++ != '*') {
		return fail(s);
	}

	signed expect = ~0;
	unsigned depth = 1;
	for (; s.p < s.e; ++s.p) {
		signed c = *s.p;
		if (c == expect) {
			expect = ~0;
			if (c == '/') {
				if (--depth == 0) {
					++s.p;
					break;
				}
			} else/* if (c == '*')*/ {
				++depth;
			}
		} else if (c == '/') {
			expect = '*';
		} else if (c == '*') {
			expect = '/';
		} else {
			expect = ~0;
		}
	}
	if (depth > 0) {
		return fail(s, "unclosed block comment (at depth %u)", depth);
	}
	return ok(s);
}})

TOGO_PARSER_DEF(p_comment_single,
Func{[](Parser const*, ParseState& s, ParsePosition const&) {
	if (s.p >= s.e || *s.p++ != '/') {
		return fail(s);
	}
	if (s.p >= s.e || *s.p++ != '/') {
		return fail(s, "expected second slash starting a comment");
	}

	for (; s.p < s.e; ++s.p) {
		if (*s.p == '\n') {
			break;
		}
	}
	return ok(s);
}})

TOGO_PARSER_DEF(p_comment,
PMod::test,
Any{p_storage,
	p_comment_block,
	p_comment_single
})

TOGO_PARSER_DEF(p_separator,
PMod::test,
Func{[](Parser const*, ParseState& s, ParsePosition const& from) {
	while (s.p < s.e) {
		signed c = *s.p;
		if (!(false
			|| c == ' '
			|| c == '\t'
			|| c == '\n'
			|| c == ','
			|| c == ';'
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

TOGO_PARSER_DEF(p_fluff,
PMod::repeat_or_none,
Any{p_storage,
	PDef::whitespace,
	p_separator,
	p_comment
})

TOGO_PARSER_DEF(p_value_f64,
PMod::flatten,
Close{p_storage,
Func{[](Parser const*, ParseState& s, ParsePosition const&) {
	enum : unsigned {
		PART_DECIMAL  = 1 << 0,
		PART_EXPONENT = 1 << 1,
	};
	unsigned parts = 0;
	parser::parse_do(PDef::sign_maybe, s);
	parser::parse_do(PDef::digits_dec, s);
	if (*s.p == '.') {
		++s.p;
		parts |= PART_DECIMAL;
		if (!parser::parse_do(PDef::digits_dec, s)) {
			return fail(s, "missing numeral part after decimal point");
		}
	}
	if (*s.p == 'e' || *s.p == 'E') {
		++s.p;
		parts |= PART_EXPONENT;
		parser::parse_do(PDef::sign_maybe, s);
		if (!parser::parse_do(PDef::digits_dec, s)) {
			return fail(s, "missing numeral part after exponent");
		}
	}
	if (parts == 0) {
		return fail(s, "floating-point number must have at least a decimal point or an exponent");
	}
	return ok(s);
}},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto& r = back(s.results);
	return ok_replace(s, from, {parser::parse_f64({r.s.b, r.s.e})});
}
})

TOGO_PARSER_DEF(p_value_string_identifier,
Func{[](Parser const*, ParseState& s, ParsePosition const& from) {
	if (s.p >= s.e) {
		return fail(s);
	}
	signed c = *s.p++;
	if (!(false
		|| ('a' <= c && c <= 'z')
		|| ('A' <= c && c <= 'Z')
		|| ('0' <= c && c <= '9')
		|| c == '_'
		// UTF-8 lead byte
		|| c >= 0xC0
	)) {
		return fail(s);
	}

	for (; s.p < s.e; ++s.p) {
		c = *s.p;
		if (!(false
			|| ('a' <= c && c <= 'z')
			|| ('A' <= c && c <= 'Z')
			|| ('0' <= c && c <= '9')
			|| c == '_'
			|| c == '-'
			// part of UTF-8 code unit sequence
			|| c >= 0x80
		)) {
			break;
		}
	}
	return ok(s, {from.p, s.p});
}})

TOGO_PARSER_DEF(p_value_string_quote,
Func{[](Parser const*, ParseState& s, ParsePosition const& from) {
	if (s.p >= s.e || *s.p++ != '\"') {
		return fail(s);
	}

	unsigned num_escapes = 0;
	bool escaped = false;
	for (; s.p < s.e; ++s.p) {
		signed c = *s.p;
		if (c == '\n') {
			return fail(s, "unexpected newline in quoted string");
		} else if (escaped) {
			++num_escapes;
			escaped = false;
		} else if (c == '\\') {
			escaped = true;
		} else if (c == '\"') {
			ParseResult r{from.p + 1, s.p++};
			r.type = rtype_string_quote;
			push(s, rvalue_ref(r));
			push(s, {num_escapes});
			return ok(s);
		}
	}
	return fail(s, "expected \" to end quoted string");
}})

TOGO_PARSER_DEF(p_value_string_block,
Func{[](Parser const*, ParseState& s, ParsePosition const& from) {
	if (s.p >= s.e || *s.p++ != '`') {
		return fail(s);
	}
	if (s.p >= s.e || *s.p++ != '`') {
		return fail(s, "expected second ` in block string opener");
	}
	if (s.p >= s.e || *s.p++ != '`') {
		return fail(s, "expected third ` in block string opener");
	}

	unsigned num_ticks = 0;
	for (; s.p < s.e; ++s.p) {
		signed c = *s.p;
		if (c == '`') {
			if (++num_ticks == 3) {
				++s.p;
				return ok(s, {from.p + 3, s.p - 3});
			}
		} else {
			num_ticks = 0;
		}
	}
	return fail(s, "expected ``` to end block string");
}})

TOGO_PARSER_DEF(p_value_vector,
Close{p_storage,
Bounded{p_storage,
	'(', ')', Parser{
	PMod::repeat,
	Any{p_storage,
		p_fluff,
		p_value_f64,
		PDef::s64_any
	}}
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto& k = parser_node_current(s);
	unsigned nr = num_results(s, from);
	switch (nr) {
	case 0:
		return fail(s, "vector cannot be empty");

	case 1:
		kvs::vec1(k, Vec1{
			result_to_f64(s, from, 0)
		});
		break;
	case 2:
		kvs::vec2(k, Vec2{
			result_to_f64(s, from, 0),
			result_to_f64(s, from, 1)
		});
		break;
	case 3:
		kvs::vec3(k, Vec3{
			result_to_f64(s, from, 0),
			result_to_f64(s, from, 1),
			result_to_f64(s, from, 2)
		});
		break;
	case 4:
		kvs::vec4(k, Vec4{
			result_to_f64(s, from, 0),
			result_to_f64(s, from, 1),
			result_to_f64(s, from, 2),
			result_to_f64(s, from, 3)
		});
		break;

	default:
		return fail(s, "vector must have only 1 to 4 dimensions, but input has %u dimensions", nr);
	}
	return ok_replace(s, from);
}})

extern Parser const p_value_array;
extern Parser const p_value_node;

TOGO_PARSER_DEF(p_instance_value,
Close{p_storage,
Any{p_storage,
	PDef::null,
	PDef::boolean,
	p_value_f64,
	PDef::s64_any,
	p_value_string_identifier,
	p_value_string_quote,
	p_value_string_block,
	p_value_vector,
	p_value_array,
	p_value_node
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	unsigned nr = num_results(s, from);
	if (nr == 0) {
		return ok(s);
	}
	// TOGO_DEBUG_ASSERTE(nr == 1);

	auto& k = parser_node_current(s);
	auto const& r = result(s, from, 0);
	switch (r.type) {
	case ParseResult::type_null: kvs::nullify(k); break;
	case ParseResult::type_bool: kvs::boolean(k, r.b); break;
	case ParseResult::type_s64: kvs::integer(k, r.i); break;
	case ParseResult::type_f64: kvs::decimal(k, r.f); break;
	case ParseResult::type_slice: kvs::string(k, StringRef{r.s.b, r.s.e}); break;
	case rtype_string_quote: {
		unsigned num_escapes = result(s, from, 1).u;
		StringRef slice_str{r.s.b, r.s.e};
		if (num_escapes == 0 || slice_str.empty()) {
			kvs::string(k, slice_str);
		} else {
			kvs::set_type(k, KVSType::string);
			k._value.string.size = slice_str.size - num_escapes;
			k._value.string.capacity = k._value.string.size + 1;
			k._value.string.data = TOGO_ALLOCATE_N(
				memory::default_allocator(),
				char, k._value.string.capacity
			);
			string_copy_unescape(k._value.string.data, k._value.string.capacity, num_escapes, slice_str);
		}
	}	break;

	default:
		TOGO_ASSERT(false, "unexpected result type");
	}
	return ok_replace(s, from);
}})

inline ParseResultCode pf_instance_open(Parser const*, ParseState& s, ParsePosition const&) {
	auto& k = parser_node_current(s);
	++static_cast<KVSParseState*>(s.userdata)->current_node;
	return ok(s, {rtype_kvs, &kvs::push_back(k, KVS{})});
}

inline ParseResultCode pf_instance_close(Parser const*, ParseState& s, ParsePosition const& from) {
	--static_cast<KVSParseState*>(s.userdata)->current_node;
	auto& k = parser_node_current(s);
	if (s.result_code != ParseResultCode::ok) {
		kvs::pop_back(k);
	}
	set_num_results(s, from);
	return s.result_code;
}

TOGO_PARSER_DEF(p_instance,
CloseAndFlush{p_storage,
	pf_instance_close,
Open{
	pf_instance_open,
	p_instance_value,
}})

TOGO_PARSER_DEF(p_name,
Close{p_storage,
All{p_storage,
	Any{p_storage,
		p_value_string_identifier,
		p_value_string_quote,
		p_value_string_block,
	},
	PDef::space_maybe,
	Parser{PMod::test, Char{'='}}
},
[](Parser const*, ParseState& s, ParsePosition const& from) {
	auto& r = result(s, from, 0);
	auto& k = parser_node_current(s);
	if (r.type == ParseResult::type_slice) {
		auto name = result(s, from, 0).s;
		kvs::set_name(k, {name.b, name.e});
	} else if (r.type == rtype_string_quote) {
		unsigned num_escapes = result(s, from, 1).u;
		StringRef slice_str{r.s.b, r.s.e};
		if (num_escapes == 0 || slice_str.empty()) {
			kvs::set_name(k, slice_str);
		} else {
			unsigned new_size = slice_str.size - num_escapes;
			if (new_size > k._name_size) {
				TOGO_DEALLOCATE(memory::default_allocator(), k._name);
				k._name_size = new_size;
				k._name = TOGO_ALLOCATE_N(
					memory::default_allocator(),
					char, k._name_size + 1
				);
			}
			string_copy_unescape(k._name, k._name_size + 1, num_escapes, slice_str);
			k._name_hash = kvs::hash_name(kvs::name_ref(k));
		}
	} else {
		TOGO_DEBUG_ASSERT(false, "expected identifier or quoted string");
	}
	return ok_replace(s, from);
}})

TOGO_PARSER_DEF(p_instance_named,
CloseAndFlush{p_storage,
	pf_instance_close,
Open{p_storage,
	pf_instance_open,
All{p_storage,
	p_name,
	PDef::space_maybe,
	p_instance_value
}}})

TOGO_PARSER_DEF(p_items_array,
PMod::repeat_or_none,
All{p_storage,
	p_fluff,
	p_instance
	// Parser{PMod::maybe, Ref{p_instance}}
})

TOGO_PARSER_DEF(p_value_array,
Open{p_storage,
Bounded{p_storage,
	'[', ']',
All{p_storage,
	p_fluff,
	p_items_array
}},
[](Parser const*, ParseState& s, ParsePosition const&) {
	kvs::set_type(parser_node_current(s), KVSType::array);
	return ok(s);
}})

TOGO_PARSER_DEF(p_items_node,
PMod::repeat_or_none,
All{p_storage,
	p_fluff,
	p_instance_named
	// Parser{PMod::maybe, Ref{p_instance_named}}
})

TOGO_PARSER_DEF(p_value_node,
Open{p_storage,
Bounded{
	'{', '}',
	p_items_node,
},
[](Parser const*, ParseState& s, ParsePosition const&) {
	kvs::set_type(parser_node_current(s), KVSType::node);
	return ok(s);
}})

} // anonymous namespace

TOGO_PARSER_DEF(p_root,
All{p_storage,
	Head{},
	p_fluff,
	p_items_node,
	Tail{}
})

#undef TOGO_PARSER_DEF

} // namespace togo
