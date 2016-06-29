#line 2 "togo/core/parser/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Parser types.
@ingroup lib_core_types
@ingroup lib_core_parser
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/string/types.hpp>

namespace togo {
/**
	@addtogroup lib_core_parser
	@{
*/

// Forward declarations
namespace parser {

struct Parser;

} // namespace parser

namespace parse_state {

struct ParsePosition;
struct ParseState;
enum class ParseResultCode : unsigned;

} // namespace parse_state

namespace parser {

/// Parser type.
enum class ParserType : unsigned {
	Undefined,

	// intangible
	Nothing,
	Empty,
	Begin,
	End,

	// values
	Char,
	CharRange,
	String,

	// series
	Any,
	All,

	// branch
	Ref,

	// conditional_call
	Close,
};
static constexpr unsigned const c_num_types = unsigned_cast(ParserType::Close) + 1;

/// Parser modifiers.
///
/// These are executed in order of definition.
enum class ParserModifier : unsigned {
	/// No modifiers.
	none = 0,
	/// Report no_match instead of failure.
	maybe		= 1 << 0,
	/// Suppress results.
	test		= 1 << 1,
	/// Suppress results and produce a slice for the parsed segment.
	flatten		= 1 << 2,
	/// One or more.
	repeat		= 1 << 3,
};
static constexpr unsigned const c_last_option = unsigned_cast(ParserModifier::flatten);

using PMod = ParserModifier;

/// Parse function type.
using parse_func_type = parse_state::ParseResultCode (
	Parser const* p,
	parse_state::ParseState& s,
	parse_state::ParsePosition const& from
);

/// Parser data.
template<ParserType, typename = void> struct ParserData;

#define PARSER_TRAIT_TYPE(name_) \
template<ParserType T> struct name_ { \
	static constexpr bool const value = false

PARSER_TRAIT_TYPE(is_value)
	|| T == ParserType::Char
	|| T == ParserType::CharRange
	|| T == ParserType::String
;};

PARSER_TRAIT_TYPE(is_series)
	|| T == ParserType::All
	|| T == ParserType::Any
;};

PARSER_TRAIT_TYPE(is_branch)
	|| T == ParserType::Ref
;};

PARSER_TRAIT_TYPE(is_conditional_call)
	|| T == ParserType::Close
;};

#undef PARSER_TRAIT_TYPE

/// Undefined.
using Undefined = ParserData<ParserType::Undefined>;
/// Match nothing (always succeeds).
using Nothing = ParserData<ParserType::Nothing>;
/// Match an empty input.
using Empty = ParserData<ParserType::Empty>;
/// Match the beginning of the input.
using Begin = ParserData<ParserType::Begin>;
/// Match the end of the input.
using End = ParserData<ParserType::End>;

/// Match a single character.
using Char = ParserData<ParserType::Char>;
/// Match a character range.
using CharRange = ParserData<ParserType::CharRange>;
/// Match a string.
using String = ParserData<ParserType::String>;

/// Match any in a series.
using Any = ParserData<ParserType::Any>;
/// Match all in a series.
using All = ParserData<ParserType::All>;

/// Match another parser after modifiers.
using Ref = ParserData<ParserType::Ref>;

/// Match a parser and call a function if it succeeds.
using Close = ParserData<ParserType::Close>;

template<> struct ParserData<ParserType::Undefined> {};
template<> struct ParserData<ParserType::Nothing> {};
template<> struct ParserData<ParserType::Empty> {};
template<> struct ParserData<ParserType::Begin> {};
template<> struct ParserData<ParserType::End> {};

template<>
struct ParserData<ParserType::Char> {
	signed c;
};

template<>
struct ParserData<ParserType::CharRange> {
	signed b;
	signed e;
};

template<>
struct ParserData<ParserType::String> {
	StringRef s;
};

template<ParserType T>
struct ParserData<T, enable_if<is_series<T>::value>> {
	unsigned num;
	Parser const** p;

	template<class... P>
	ParserData(Allocator& a, P&&... p);
};

template<ParserType T>
struct ParserData<T, enable_if<is_branch<T>::value>> {
	Parser const* p;

	ParserData(Parser const& p);
	ParserData(Allocator& a, Parser&& p);
};

template<ParserType T>
struct ParserData<T, enable_if<is_conditional_call<T>::value>> {
	parse_func_type* f;
	Parser const* p;

	ParserData(parse_func_type* f);
	ParserData(parse_func_type* f, Parser const& p);
	ParserData(Parser const& p, parse_func_type* f);
	ParserData(Allocator& a, parse_func_type* f, Parser&& p);
	ParserData(Allocator& a, Parser&& p, parse_func_type* f);
};

/// Parser.
struct Parser {
	// {u16 type; u16 modifiers;}
	u32 properties;
	hash32 name_hash;
	StringRef name;

	union Storage {
		Char Char;
		CharRange CharRange;
		String String;

		Any Any;
		All All;

		Ref Ref;

		Close Close;

		Storage(no_init_tag) {}

		Storage(parser::Undefined&&) {}
		Storage(parser::Nothing&&) {}
		Storage(parser::Empty&&) {}
		Storage(parser::Begin&&) {}
		Storage(parser::End&&) {}

		Storage(parser::Char&& d) : Char(rvalue_ref(d)) {}
		Storage(parser::CharRange&& d) : CharRange(rvalue_ref(d)) {}
		Storage(parser::String&& d) : String(rvalue_ref(d)) {}

		Storage(parser::Any&& d) : Any(rvalue_ref(d)) {}
		Storage(parser::All&& d) : All(rvalue_ref(d)) {}

		Storage(parser::Ref&& d) : Ref(rvalue_ref(d)) {}

		Storage(parser::Close&& d) : Close(rvalue_ref(d)) {}
	} s;

	/// Construct named Undefined parser.
	Parser(StringRef name);

	/// Construct unnamed Undefined parser.
	Parser();

	/// Construct named parser with modifiers.
	template<ParserType T>
	Parser(StringRef name, ParserModifier mods, ParserData<T>&& d);

	/// Construct named parser.
	template<ParserType T>
	Parser(StringRef name, ParserData<T>&& d);

	/// Construct unnamed parser with modifiers.
	template<ParserType T>
	Parser(ParserModifier mods, ParserData<T>&& d);

	/// Construct unnamed parser.
	template<ParserType T>
	Parser(ParserData<T>&& d);
};

/// sizeof(Parser) * num.
inline constexpr unsigned sizeof_n(unsigned num = 1) {
	return sizeof(Parser) * num;
}

/// The allocation size of a series parser (e.g., Any).
inline constexpr unsigned sizeof_series(unsigned num_ref, unsigned num_inplace = 0) {
	return sizeof(Parser*) * (num_ref + num_inplace) + sizeof_n(num_inplace);
}

/// FixedAllocator helper.
template<
	unsigned num_ref,
	unsigned num_inplace = 0,
	unsigned num = 0
>
using FixedParserAllocator = FixedAllocator<0
	+ sizeof_series(num_ref, num_inplace)
	+ sizeof_n(num)
>;

/// Predefined parsers.
struct PDef {
	/// "null" => null
	static Parser const null;
	/// (true|false) => bool
	static Parser const boolean;

	/// [0-9] => char
	static Parser const digit_dec;
	/// <digit_dec>+ => slice
	static Parser const digits_dec;

	/// [0-9a-fA-F] => char
	static Parser const digit_hex;
	/// <digit_hex>+ => slice
	static Parser const digits_hex;

	/// [0-7] => char
	static Parser const digit_oct;
	/// [0-7]+ => slice
	static Parser const digits_oct;

	/// <digits>+([eE][\-+]?<digits>+)? => u64
	static Parser const u64_dec;
	/// 0[xX]<hex_digits>+ => u64
	static Parser const u64_hex;
	/// 0<oct_digits>+ => u64
	static Parser const u64_oct;
	/// (u64_dec|u64_hex|u64_oct) => u64
	static Parser const u64_any;

	/// <sign>?u64_dec => s64
	static Parser const s64_dec;
	/// (s64_dec|u64_hex|u64_oct) => s64
	static Parser const s64_any;
};

} // namespace parser

namespace parse_state {

/// Parse result code.
enum class ParseResultCode : unsigned {
	fail,
	ok,
	no_match,
};
static constexpr unsigned const c_num_result_codes = unsigned_cast(ParseResultCode::no_match) + 1;

/// Whether a result code is fail-y.
inline constexpr bool operator!(ParseResultCode rc) {
	return rc == ParseResultCode::fail;
}

/// Parse position.
struct ParsePosition {
	char const* p;
	u32_fast i;
};

struct Slice {
	char const* b;
	char const* e;
};

/// Parse result.
struct ParseResult {
	enum : unsigned {
		type_null,
		type_bool,
		type_char,
		type_s64,
		type_u64,
		type_f64,
		type_pointer,
		type_slice,

		type_user_base,
	};

	unsigned type;
	union Value {
		bool b;
		char c;
		s64 i;
		u64 u;
		f64 f;
		void const* p;
		Slice s;

		Value() = default;
		~Value() = default;

		Value(bool x) : b(x) {}
		Value(char x) : c(x) {}
		Value(s64 x) : i(x) {}
		Value(u64 x) : u(x) {}
		Value(f64 x) : f(x) {}
		Value(void const* x) : p(x) {}
		Value(char const* b, char const* e) : s{b, e} {}
	} v;

	ParseResult() = default;
	~ParseResult() = default;

	ParseResult(null_tag const) : type(type_null) {}
	ParseResult(unsigned type, Value&& x) : type(type), v(rvalue_ref(x)) {}
	ParseResult(bool x) : type(type_bool), v(x) {}
	ParseResult(char x) : type(type_char), v(x) {}
	ParseResult(s64 x) : type(type_s64), v(x) {}
	ParseResult(u64 x) : type(type_u64), v(x) {}
	ParseResult(f64 x) : type(type_f64), v(x) {}
	ParseResult(void const* x) : type(type_pointer), v(x) {}
	ParseResult(char const* b, char const* e) : type(type_slice), v{b, e} {}
};

/// Parse error.
struct ParseError {
	/// Line where the error occurred.
	unsigned line;
	/// Column where the error occurred.
	unsigned column;

	/// Result code.
	ParseResultCode result_code;
	/// Error message.
	FixedArray<char, 512> message;
};

/// Parse state.
struct ParseState {
	char const* p;
	char const* b;
	char const* e;
	char const* t;
	Array<ParseResult> results;
	unsigned suppress_results;
	unsigned suppress_errors;
	unsigned line;
	unsigned column;
	ParseError* error;
	void* userdata;

	ParseState(
		Allocator& allocator,
		ParseError* error = nullptr,
		void* userdata = nullptr
	)
		: p(nullptr)
		, b(nullptr)
		, e(nullptr)
		, t(nullptr)
		, results(allocator)
		, suppress_results(0)
		, suppress_errors(0)
		, line(0)
		, column(0)
		, error(error)
		, userdata(userdata)
	{}
};

} // namespace parse_state

/** @} */ // end of doc-group lib_core_parser

/** @cond INTERNAL */
template<> struct enable_enum_bitwise_ops<parser::ParserModifier> : true_type {};
/** @endcond */ // INTERNAL

using parser::Undefined;
using parser::Nothing;
using parser::Empty;
using parser::Begin;
using parser::End;

using parser::Char;
using parser::CharRange;
using parser::String;

using parser::Any;
using parser::All;

using parser::Ref;

using parser::Close;

using parser::ParserType;
using parser::ParserModifier;
using parser::PMod;
using parser::Parser;
using parser::FixedParserAllocator;

using parse_state::ParseResultCode;
using parse_state::ParseResult;
using parse_state::ParsePosition;
using parse_state::ParseError;
using parse_state::ParseState;

using parser::PDef;

} // namespace togo
