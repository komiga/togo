#line 2 "togo/kvs_io.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/memory.hpp>
#include <togo/temp_allocator.hpp>
#include <togo/array.hpp>
#include <togo/io_types.hpp>
#include <togo/io.hpp>
#include <togo/kvs.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace togo {

namespace {

enum : signed {
	PC_EOF = ~0,
};

enum : unsigned {
	PF_NONE				= 0,
	PF_ERROR			= 1 << 0,
	PF_VALUE_NAMELESS	= 1 << 1,
	PF_NAME				= 1 << 2,
	PF_ASSIGN			= 1 << 3,
	PF_ESCAPED			= 1 << 4,
	PF_CARRY			= 1 << 5,
};

enum ParserStage : unsigned {
	PS_NAME,
	PS_ASSIGN,
	PS_VALUE,
};

enum ParserValueType : unsigned {
	PV_NONE,
	PV_NULL,
	PV_INTEGER,
	PV_DECIMAL,
	PV_TRUE,
	PV_FALSE,
	PV_STRING,
};

struct Parser {
	KVS& root;
	IReader& stream;
	ParserInfo& info;
	Array<KVS*> stack;
	Array<char> buffer;
	unsigned line;
	unsigned column;
	signed c;
	unsigned flags;
	ParserStage stage;
	ParserValueType value_type;
};

static char escape_char(char const c) {
	switch (c) {
	case 't': return '\t';
	case 'n': return '\n';
	case 'r': return '\r';
	default: return c;
	}
}

template<bool>
struct parse_s64_adaptor;

template<>
struct parse_s64_adaptor<true> {
	inline static s64 parse(char const* const cstr, unsigned const base) {
		return std::strtol(cstr, nullptr, base);
	}
};

template<>
struct parse_s64_adaptor<false> {
	inline static s64 parse(char const* const cstr, unsigned const base) {
		return std::strtoll(cstr, nullptr, base);
	}
};

inline static s64 parse_s64(char const* const cstr, unsigned const base) {
	return parse_s64_adaptor<sizeof(long) == sizeof(s64)>::parse(cstr, base);
}

template<bool>
struct parse_f64_adaptor;

template<>
struct parse_f64_adaptor<true> {
	inline static f64 parse(char const* const cstr) {
		return std::strtod(cstr, nullptr);
	}
};

template<>
struct parse_f64_adaptor<false> {
	inline static f64 parse(char const* const cstr) {
		return std::strtold(cstr, nullptr);
	}
};

inline static f64 parse_f64(char const* const cstr) {
	return parse_f64_adaptor<sizeof(double) >= sizeof(f64)>::parse(cstr);
}

static bool parser_error(
	Parser& p,
	char const* const format,
	...
) {
	va_list va;
	va_start(va, format);
	std::vsnprintf(p.info.message, array_extent(&ParserInfo::message), format, va);
	va_end(va);
	p.info.line = p.line;
	p.info.column = p.column;
	p.flags |= PF_ERROR;
	return false;
}

inline static bool parser_error_expected(Parser& p, char const* const what) {
	parser_error(p, "expected %s, got '%c'", what, p.c);
	return false;
}

inline static bool parser_error_unexpected(Parser& p, char const* const what) {
	parser_error(p, "unexpected %s: '%c'", what, p.c);
	return false;
}

inline static bool parser_error_stream(Parser& p, char const* const what) {
	if (io::status(p.stream).fail()) {
		parser_error(p, "%s: stream read failure", what);
	}
	return false;
}

inline static bool parser_is_identifier_lead(Parser const& p) {
	return
		 p.c == '_' ||
		(p.c >= 'A' && p.c <= 'Z') ||
		(p.c >= 'a' && p.c <= 'z')
	;
}

/*inline static bool parser_is_value_completer(Parser const& p) {
	return
		p.c == '\t' ||
		p.c == '\n' ||
		p.c == ' ' ||
		p.c == ',' ||
		p.c == ';'
	;
}*/

inline static bool parser_is_completer(Parser const& p) {
	return
		p.c == '\n' ||
		p.c == ',' ||
		p.c == ';'
	;
}

inline static bool parser_is_number_lead(Parser const& p) {
	return
		(p.c >= '0' && p.c <= '9') ||
		p.c == '-' ||
		p.c == '+' ||
		p.c == '.'
	;
}

static void parser_buffer_add(Parser& p) {
	if (p.flags & PF_ESCAPED) {
		p.flags &= ~PF_ESCAPED;
		array::push_back(p.buffer, escape_char(static_cast<char>(p.c)));
	} else {
		array::push_back(p.buffer, static_cast<char>(p.c));
	}
}

inline static void parser_buffer_clear(Parser& p) {
	array::clear(p.buffer);
}

static StringRef parser_buffer_ref(Parser const& p) {
	return StringRef{
		array::begin(p.buffer),
		static_cast<unsigned>(array::size(p.buffer))
	};
}

inline static KVS& parser_top(Parser& p) {
	return *array::back(p.stack);
}

inline static void parser_pop(Parser& p) {
	array::pop_back(p.stack);
	if (kvs::is_array(parser_top(p))) {
		p.stage = PS_VALUE;
		p.flags |= PF_VALUE_NAMELESS;
	} else {
		p.stage = PS_NAME;
		p.flags &= ~PF_VALUE_NAMELESS;
	}
}

inline static void parser_push(Parser& p, KVS& kvs) {
	array::push_back(p.stack, &kvs);
}

inline static void parser_push_new(Parser& p, bool const named) {
	KVS& top = parser_top(p);
	if (!kvs::space(top)) {
		kvs::grow(top);
	}
	kvs::resize(top, kvs::size(top) + 1);
	if (named && array::any(p.buffer)) {
		kvs::set_name(kvs::back(top), parser_buffer_ref(p));
		parser_buffer_clear(p);
	}
	parser_push(p, kvs::back(top));
}

inline static bool parser_set_value(Parser& p) {
	TOGO_DEBUG_ASSERTE(p.value_type != PV_NONE);
	switch (p.value_type) {
	case PV_NONE: break;
	case PV_NULL:
		kvs::nullify(parser_top(p));
		break;

	case PV_INTEGER:
		array::push_back(p.buffer, '\0');
		kvs::integer(parser_top(p), parse_s64(array::begin(p.buffer), 10));
		break;

	case PV_DECIMAL:
		array::push_back(p.buffer, '\0');
		kvs::decimal(parser_top(p), parse_f64(array::begin(p.buffer)));
		break;

	case PV_TRUE: // fall-through
	case PV_FALSE:
		kvs::boolean(parser_top(p), p.value_type == PV_TRUE);
		break;

	case PV_STRING:
		kvs::string(parser_top(p), parser_buffer_ref(p));
		break;
	}
	parser_buffer_clear(p);
	p.stage = (p.flags & PF_VALUE_NAMELESS) ? PS_VALUE : PS_NAME;
	p.value_type = PV_NONE;
	p.flags &= ~(PF_NAME | PF_ASSIGN);
	return true;
}

static bool parser_next(Parser& p) {
	if (p.flags & PF_CARRY) {
		p.flags &= ~PF_CARRY;
		return true;
	}
	char c = '\0';
	do {} while (io::read_value(p.stream, c) && c == '\r');
	p.c = c;
	IOStatus const& status = io::status(p.stream);
	if (status.eof()) {
		p.c = PC_EOF;
	} else if (status.fail()) {
		return false;
	}
	if (p.c == '\n') {
		++p.line;
		p.column = 0;
	} else {
		++p.column;
	}
	return true;
}

static bool parser_skip_whitespace(Parser& p, bool const newline) {
	while (p.c == ' ' || p.c == '\t' || (newline && p.c == '\n')) {
		if (!parser_next(p)) {
			return false;
		}
	}
	return true;
}

static bool parser_read_number(Parser& p) {
	enum : unsigned {
		PART_SIGN				= 1 << 0,
		PART_NUMERAL			= 1 << 1,
		PART_DECIMAL			= 1 << 2,
		PART_DECIMAL_NUMERAL	= 1 << 3,
		PART_EXPONENT			= 1 << 4,
		PART_EXPONENT_SIGN		= 1 << 5,
		PART_EXPONENT_NUMERAL	= 1 << 6,
	};
	unsigned parts = 0;
	do {
		switch (p.c) {
		// Completers
		case PC_EOF:
		case '\t':
		case '\n':
		case ' ':
		case ',': case ';':
		case '}': case ']':
			if (~parts & PART_NUMERAL) {
				return parser_error(p, "missing numeral part in number");
			} else if (parts & PART_DECIMAL && ~parts & PART_DECIMAL_NUMERAL) {
				return parser_error(p, "missing numeral part after decimal in number");
			} else if (parts & PART_EXPONENT && ~parts & PART_EXPONENT_NUMERAL) {
				return parser_error(p, "missing numeral part after number exponent");
			}
			if (parts & PART_DECIMAL || parts & PART_EXPONENT) {
				p.value_type = PV_DECIMAL;
			} else {
				p.value_type = PV_INTEGER;
			}
			p.flags |= PF_CARRY;
			return true;

		case '-': case '+':
			if (
				parts & PART_EXPONENT_SIGN ||
				(~parts & PART_EXPONENT && parts & PART_SIGN)
			) {
				return parser_error(p, "sign already specified for number part");
			} else if (parts & PART_EXPONENT_NUMERAL) {
				return parser_error(p, "unexpected non-leading sign in number exponent");
			}
			if (parts & PART_EXPONENT) {
				parts |= PART_EXPONENT_SIGN;
			} else {
				parts |= PART_SIGN;
			}
			break;

		case '.':
			if (parts & PART_EXPONENT) {
				return parser_error(p, "unexpected decimal point in number exponent");
			} else if (parts & PART_DECIMAL) {
				return parser_error(p, "decimal point in number specified twice");
			}
			parts |= PART_DECIMAL;
			break;

		case 'e': case 'E':
			if (parts & PART_EXPONENT) {
				return parser_error(p, "exponent in number specified twice");
			}
			parts |= PART_EXPONENT;
			break;

		default:
			if (p.c >= '0' || p.c <= '9') {
				if (parts & PART_EXPONENT) {
					parts |= PART_EXPONENT_NUMERAL;
				} else if (parts & PART_DECIMAL) {
					parts |= PART_NUMERAL | PART_DECIMAL_NUMERAL;
				} else {
					parts |= PART_NUMERAL;
				}
			} else {
				return parser_error_unexpected(p, "symbol in number");
			}
			break;
		}
		parser_buffer_add(p);
	} while (parser_next(p));
	return parser_error_stream(p, "in number");
}

static bool parser_read_string(Parser& p) {
	do {
		switch (p.c) {
		// Completers
		case PC_EOF:
		case '\t':
		case '\n':
		case ' ':
		case ',': case ';':
		case '=':
		case '}': case ']':
			p.flags |= PF_CARRY;
			p.value_type = PV_STRING;
			if (array::size(p.buffer) == 4) {
				if (std::memcmp(array::begin(p.buffer), "null", 4) == 0) {
					p.value_type = PV_NULL;
				} else if (std::memcmp(array::begin(p.buffer), "true", 4) == 0) {
					p.value_type = PV_TRUE;
				}
			} else if (array::size(p.buffer) == 5) {
				if (std::memcmp(array::begin(p.buffer), "false", 5) == 0) {
					p.value_type = PV_FALSE;
				}
			}
			return true;

		case '\\':
		case '{': case '[':
		case '+': case '-':
		case '\'':
		case '"':
		case '`':
			return parser_error_unexpected(p, "symbol in string");
		}
		parser_buffer_add(p);
	} while (parser_next(p));
	return parser_error_stream(p, "in unquoted string");
}

static bool parser_read_string_quote(Parser& p) {
	while (parser_next(p)) {
		switch (p.c) {
		case PC_EOF:
			return parser_error(p, "expected completer for double-quote bounded string, got EOF");

		case '"':
			if (~p.flags & PF_ESCAPED) {
				p.value_type = PV_STRING;
				return true;
			}
			break;

		case '\\':
			if (~p.flags & PF_ESCAPED) {
				p.flags |= PF_ESCAPED;
				continue;
			}
			break;

		case '\n':
			return parser_error(p, "unexpected newline in double-quote bounded string");
		}
		parser_buffer_add(p);
	}
	return parser_error_stream(p, "in double-quote bounded string");
}

static bool parser_read_string_block(Parser& p) {
	unsigned count = 1;
	while (count < 3 && parser_next(p)) {
		if (p.c != '`') {
			return parser_error(p, "incomplete lead block-quote for expected block-quote bounded string");
		}
		++count;
	}
	if (!io::status(p.stream)) {
		return false;
	}
	count = 0;
	while (parser_next(p)) {
		if (p.c == PC_EOF) {
			return parser_error(p, "expected completer for block-quote bounded string, got EOF");
		} else if (p.c == '`') {
			if (++count == 3) {
				p.value_type = PV_STRING;
				array::resize(p.buffer, array::size(p.buffer) - 2);
				return true;
			}
		}
		parser_buffer_add(p);
	}
	return parser_error_stream(p, "in block-quote bounded string");
}

static void parser_stage_name(Parser& p) {
	if (!parser_skip_whitespace(p, true)) {
		return;
	}
	switch (p.c) {
	case PC_EOF:
		break;

	case '}':
		if (array::size(p.stack) == 1) {
			parser_error(p, "unbalanced '}' at root level");
		} else if (kvs::is_type(parser_top(p), KVSType::node)) {
			parser_pop(p);
		} else {
			parser_error_expected(p, "identifier");
		}
		break;

	case ']':
		parser_error(p, "unbalanced ']'");
		break;

	case '"':
		if (parser_read_string_quote(p)) {
			goto l_push_new;
		}
		break;

	default:
		if (parser_is_identifier_lead(p)) {
			if (parser_read_string(p)) {
				if (parser_is_completer(p)) {
					parser_error_unexpected(p, "completer after name");
				} else {
					goto l_push_new;
				}
			}
		} else {
			parser_error_expected(p, "identifier");
		}
		break;
	}
	return;

l_push_new:
	parser_push_new(p, true);
	p.stage = PS_ASSIGN;
	p.flags |= PF_NAME;
}

static void parser_stage_assign(Parser& p) {
	if (!parser_skip_whitespace(p, false)) {
		return;
	}
	if (p.c == PC_EOF) {
		parser_error(p, "expected equality sign, got EOF");
	} else if (p.c == '=') {
		p.stage = PS_VALUE;
		p.flags |= PF_ASSIGN;
	} else {
		parser_error_expected(p, "equality sign");
	}
}

static void parser_stage_value(Parser& p) {
	if (!parser_skip_whitespace(p, p.flags & PF_VALUE_NAMELESS)) {
		return;
	}
	switch (p.c) {
	case PC_EOF:
		parser_error(p, "expected value, got EOF");
		break;

	case '\t':
	case ' ':
		break;

	case '\n':
		if (p.flags & PF_ASSIGN) {
			parser_error_expected(p, "value");
		}
		break;

	case '{':
		if (~p.flags & PF_ASSIGN) {
			parser_push_new(p, false);
		}
		kvs::set_type(parser_top(p), KVSType::node);
		p.stage = PS_NAME;
		p.flags &= ~(PF_NAME | PF_ASSIGN | PF_VALUE_NAMELESS);
		break;

	case '[':
		if (~p.flags & PF_ASSIGN) {
			parser_push_new(p, false);
		}
		kvs::set_type(parser_top(p), KVSType::array);
		p.flags |= PF_VALUE_NAMELESS;
		p.flags &= ~(PF_NAME | PF_ASSIGN);
		break;

	case ']':
		if (array::size(p.stack) == 1) {
			parser_error(p, "unbalanced ']' at root level");
		} else if (kvs::is_type(parser_top(p), KVSType::array)) {
			parser_pop(p);
		} else {
			parser_error_expected(p, "value");
		}
		break;

	case '"':
		if (parser_read_string_quote(p)) {
			goto l_set_value;
		}
		break;

	case '`':
		if (parser_read_string_block(p)) {
			goto l_set_value;
		}
		break;

	default:
		if (parser_is_completer(p)) {
			if (
				~p.flags & PF_VALUE_NAMELESS &&
				p.flags & PF_ASSIGN
			) {
				parser_error_expected(p, "value");
			}
		} else if (parser_is_identifier_lead(p)) {
			if (parser_read_string(p)) {
				goto l_set_value;
			}
		} else if (parser_is_number_lead(p)) {
			if (parser_read_number(p)) {
				goto l_set_value;
			}
		} else {
			parser_error_expected(p, "value");
		}
		break;
	}
	return;

l_set_value:
	if (~p.flags & PF_ASSIGN) {
		parser_push_new(p, false);
		parser_set_value(p);
	} else {
		parser_set_value(p);
	}
	parser_pop(p);
	return;
}

} // anonymous namespace

bool kvs::read(KVS& root, IReader& stream, ParserInfo& pinfo) {
	TempAllocator<4096> allocator{};
	Parser p{
		root, stream, pinfo,
		{allocator},
		{allocator},
		1, 0, PC_EOF,
		PF_NONE,
		PS_NAME,
		PV_NONE
	};
	array::reserve(p.stack, 32);
	array::reserve(p.buffer, 4096 - (1 + 32) * sizeof(void*));
	kvs::set_type(p.root, KVSType::node);
	kvs::clear(p.root);
	parser_push(p, p.root);
	while (parser_next(p)) {
		switch (p.stage) {
		case PS_NAME: parser_stage_name(p); break;
		case PS_ASSIGN: parser_stage_assign(p); break;
		case PS_VALUE: parser_stage_value(p); break;
		}
		if (p.flags & PF_ERROR) {
			return false;
		} else if (p.c == PC_EOF) {
			break;
		}
	}
	if (p.flags & PF_ERROR) {
		// do nothing
	} else if (io::status(p.stream).fail()) {
		parser_error(p, "stream read failure");
	} else if (p.c == PC_EOF) {
		if (p.flags & (PF_NAME | PF_ASSIGN)) {
			parser_error(p, "expected value, got EOF");
		} else if (array::size(p.stack) > 1) {
			parser_error(p, "%u unclosed collection(s) at EOF", array::size(p.stack) - 1);
		}
	}
	return ~p.flags & PF_ERROR;
}

namespace {

static constexpr char const TABS[]{
	"																"
};

#define RETURN_ERROR(x) if (!(x)) { return false; }

static bool write_tabs(IWriter& stream, unsigned tabs) {
	while (tabs > 0) {
		if (!io::write(stream, TABS, tabs)) {
			return false;
		}
		tabs -= min(tabs, array_extent(TABS));
	}
	return true;
}

static bool writef(IWriter& stream, char const* const format, ...) {
	char buffer[256];
	va_list va;
	va_start(va, format);
	signed const size = std::vsnprintf(buffer, array_extent(buffer), format, va);
	va_end(va);
	if (size < 0) {
		return false;
	}
	return io::write(stream, buffer, static_cast<unsigned>(size));
}

static bool kvs_write(
	KVS const& kvs,
	IWriter& stream,
	KVSType const parent_type,
	unsigned const tabs,
	bool const is_root
);

static bool kvs_write_collection(
	KVS const& kvs,
	IWriter& stream,
	char const* const open,
	char const close,
	unsigned const tabs,
	bool const is_root
) {
	if (!is_root) {
		RETURN_ERROR(io::write(stream, open, kvs::empty(kvs) ? 1 : 2));
	}
	for (unsigned i = 0; i < kvs::size(kvs); ++i) {
		RETURN_ERROR(
			kvs_write(kvs[i], stream, kvs::type(kvs), tabs + !is_root, false) &&
			io::write_value(stream, '\n')
		);
	}
	if (!is_root) {
		RETURN_ERROR(
			(kvs::empty(kvs) || write_tabs(stream, tabs)) &&
			io::write_value(stream, close)
		);
	}
	return true;
}

static bool kvs_write(
	KVS const& kvs,
	IWriter& stream,
	KVSType const parent_type,
	unsigned const tabs,
	bool const is_root
) {
	RETURN_ERROR(write_tabs(stream, tabs));
	if (!is_root && parent_type == KVSType::node) {
		RETURN_ERROR(
			io::write(stream, kvs::name(kvs), kvs::name_size(kvs)) &&
			io::write(stream, " = ", 3)
		);
	}
	switch (kvs::type(kvs)) {
	case KVSType::null:
		RETURN_ERROR(io::write(stream, "null", 4));
		break;

	case KVSType::integer:
		RETURN_ERROR(writef(stream, "%lld", kvs._value.integer));
		break;

	case KVSType::decimal:
		RETURN_ERROR(writef(stream, "%lf", kvs._value.decimal));
		break;

	case KVSType::boolean:
		if (kvs._value.boolean) {
			RETURN_ERROR(io::write(stream, "true", 4));
		} else {
			RETURN_ERROR(io::write(stream, "false", 5));
		}
		break;

	case KVSType::string:
		RETURN_ERROR(
			io::write_value(stream, '\"') &&
			io::write(stream, kvs::string(kvs), kvs::string_size(kvs)) &&
			io::write_value(stream, '\"')
		);
		break;

	case KVSType::vec1:
		RETURN_ERROR(writef(
			stream, "(%lf)",
			kvs._value.vec1.x
		));
		break;

	case KVSType::vec2:
		RETURN_ERROR(writef(
			stream, "(%f %f)",
			kvs._value.vec2.x,
			kvs._value.vec2.y
		));
		break;

	case KVSType::vec3:
		RETURN_ERROR(writef(
			stream, "(%f %f %f)",
			kvs._value.vec3.x,
			kvs._value.vec3.y,
			kvs._value.vec3.z
		));
		break;

	case KVSType::vec4:
		RETURN_ERROR(writef(
			stream, "(%f %f %f %f)",
			kvs._value.vec4.x,
			kvs._value.vec4.y,
			kvs._value.vec4.z,
			kvs._value.vec4.y
		));
		break;

	case KVSType::array:
		kvs_write_collection(kvs, stream, "[\n", ']', tabs, is_root);
		break;

	case KVSType::node:
		kvs_write_collection(kvs, stream, "{\n", '}', tabs, is_root);
		break;
	}
	return true;
}

#undef RETURN_ERROR

} // anonymous namespace

IOStatus kvs::write(KVS const& kvs, IWriter& stream) {
	kvs_write(kvs, stream, KVSType::node, 0, true);
	return io::status(stream);
}

} // namespace togo
