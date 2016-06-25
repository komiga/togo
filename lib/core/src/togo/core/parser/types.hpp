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
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/string/types.hpp>

namespace togo {
namespace parser {

/**
	@addtogroup lib_core_parser
	@{
*/

// Forward declarations
struct Parser;
struct ParseState;

/// Parser type.
enum class ParserType : unsigned {
	Undefined,
	Nothing,
	Empty,
	Begin,
	End,

	Char,
	CharRange,
	String,

	Any,
	All,
	Maybe,

	Close,
};
static constexpr unsigned const c_num_types = unsigned_cast(ParserType::Close) + 1;

/// Close function type.
using close_func_type = void (
	ParseState& state,
	Parser const* parser,
	char const* from_p,
	unsigned from_i
);

/// Parser data.
template<ParserType> struct ParserData;

using Undefined = ParserData<ParserType::Undefined>;
using Nothing = ParserData<ParserType::Nothing>;
using Empty = ParserData<ParserType::Empty>;
using Begin = ParserData<ParserType::Begin>;
using End = ParserData<ParserType::End>;

using Char = ParserData<ParserType::Char>;
using CharRange = ParserData<ParserType::CharRange>;
using String = ParserData<ParserType::String>;

using Any = ParserData<ParserType::Any>;
using All = ParserData<ParserType::All>;
using Maybe = ParserData<ParserType::Maybe>;

using Close = ParserData<ParserType::Close>;

/// Parser: undefined.
template<>
struct ParserData<ParserType::Undefined> {};

/// Parser: match nothing (always succeeds).
template<>
struct ParserData<ParserType::Nothing> {};

/// Parser: match an empty input.
template<>
struct ParserData<ParserType::Empty> {};

/// Parser: match the beginning of the input.
template<>
struct ParserData<ParserType::Begin> {};

/// Parser: match the end of the input.
template<>
struct ParserData<ParserType::End> {};

/// Parser: match a single character.
template<>
struct ParserData<ParserType::Char> {
	signed c;
};

/// Parser: match a character range.
template<>
struct ParserData<ParserType::CharRange> {
	signed b;
	signed e;
};

/// Parser: match a string.
template<>
struct ParserData<ParserType::String> {
	StringRef s;
};

/// Parser: match any in a series.
template<>
struct ParserData<ParserType::Any> {
	unsigned num;
	Parser const** p;

	template<class... P>
	ParserData(Allocator& a, P&&... p);
};

/// Parser: match all in a series.
template<>
struct ParserData<ParserType::All> {
	unsigned num;
	Parser const** p;

	template<class... P>
	ParserData(Allocator& a, P&&... p);
};

/// Parser: Maybe match another parser.
template<>
struct ParserData<ParserType::Maybe> {
	Parser const* p;

	ParserData(Parser const& p);
	ParserData(Allocator& a, Parser&& p);
};

/// Parser: match a parser and Close a function if it succeeds.
template<>
struct ParserData<ParserType::Close> {
	close_func_type* f;
	Parser const* p;

	ParserData(close_func_type* f);
	ParserData(close_func_type* f, Parser const& p);
	ParserData(Allocator& a, close_func_type* f, Parser&& p);
};

union ParserStorage {
	Char Char;
	CharRange CharRange;
	String String;

	Any Any;
	All All;
	Maybe Maybe;

	Close Close;

	ParserStorage(no_init_tag const) {}

	ParserStorage(parser::Undefined&&) {}
	ParserStorage(parser::Nothing&&) {}
	ParserStorage(parser::Empty&&) {}
	ParserStorage(parser::Begin&&) {}
	ParserStorage(parser::End&&) {}

	ParserStorage(parser::Char&& d) : Char(rvalue_ref(d)) {}
	ParserStorage(parser::CharRange&& d) : CharRange(rvalue_ref(d)) {}
	ParserStorage(parser::String&& d) : String(rvalue_ref(d)) {}

	ParserStorage(parser::Any&& d) : Any(rvalue_ref(d)) {}
	ParserStorage(parser::All&& d) : All(rvalue_ref(d)) {}
	ParserStorage(parser::Maybe&& d) : Maybe(rvalue_ref(d)) {}

	ParserStorage(parser::Close&& d) : Close(rvalue_ref(d)) {}
};

/// Parser.
struct Parser {
	ParserType type;
	hash32 name_hash;
	StringRef name;
	ParserStorage s;

	/// Construct named Undefined parser.
	Parser(StringRef name);

	/// Construct unnamed Undefined parser.
	Parser();

	/// Construct named parser.
	template<ParserType T>
	Parser(StringRef name, ParserData<T>&& d);

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
	ParseResult(Value&& x) : type(type_bool), v(x) {}
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
		char const* b, char const* e,
		void* userdata = nullptr
	)
		: p(b)
		, b(b), e(e)
		, t(b)
		, results(allocator)
		, suppress_results(0)
		, suppress_errors(0)
		, line(0)
		, column(0)
		, error(nullptr)
		, userdata(userdata)
	{}
};

/** @} */ // end of doc-group lib_core_parser

} // namespace parser

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
using parser::Maybe;

using parser::Close;

using parser::ParserType;
using parser::Parser;
using parser::FixedParserAllocator;

using parser::ParseResultCode;
using parser::ParseResult;
using parser::ParseError;
using parser::ParseState;

} // namespace togo
