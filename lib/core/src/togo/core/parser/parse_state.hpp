#line 2 "togo/core/parser/parse_state.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ParseState interface.
@ingroup lib_core_parser
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/parser/types.hpp>
#include <togo/core/parser/parse_state.hpp>

#include <cstdarg>

#include <togo/core/parser/parse_state.gen_interface>

namespace togo {
namespace parser {

/// Increment error suppression counter.
inline void suppress_errors(ParseState& s) {
	++s.suppress_errors;
}

/// Decrement error suppression counter.
inline void unsuppress_errors(ParseState& s) {
	TOGO_DEBUG_ASSERTE(s.suppress_errors > 0);
	--s.suppress_errors;
}

/// Increment result suppression counter.
inline void suppress_results(ParseState& s) {
	++s.suppress_results;
}

/// Decrement result suppression counter.
inline void unsuppress_results(ParseState& s) {
	TOGO_DEBUG_ASSERTE(s.suppress_results > 0);
	--s.suppress_results;
}

/// Get parse position.
inline ParsePosition position(ParseState const& s) {
	return {s.p, array::size(s.results)};
}

/// Set parse position.
inline void set_position(ParseState& s, ParsePosition const& pos) {
	TOGO_DEBUG_ASSERTE(pos.i <= array::size(s.results));
	s.p = pos.p;
	s.results._size = pos.i;
	// array::resize(s.results, pos.i);
}

/// Pop results.
inline void pop(ParseState& s, unsigned num = 1) {
	TOGO_DEBUG_ASSERTE(num <= array::size(s.results));
	s.results._size -= num;
}

/// Push result.
inline void push(ParseState& s, ParseResult const& r) {
	if (!s.suppress_results) {
		array::push_back(s.results, r);
	}
}

/// Push result.
inline void push(ParseState& s, ParseResult&& r) {
	if (!s.suppress_results) {
		array::push_back(s.results, rvalue_ref(r));
	}
}

/// Set parse error.
#if defined(TOGO_COMPILER_CLANG) || \
	defined(TOGO_COMPILER_GCC)
	__attribute__((__format__ (__printf__, 2, 3)))
#endif
inline void set_error(ParseState& s, char const* format, ...) {
	if (s.suppress_errors || !s.error) {
		return;
	}
	va_list va;
	va_start(va, format);
	parser::set_error_va(s, format, va);
	va_end(va);
}

/// Parse failure.
inline ParseResultCode fail() {
	return ParseResultCode::fail;
}

/// Parse failure with error message.
#if defined(TOGO_COMPILER_CLANG) || \
	defined(TOGO_COMPILER_GCC)
	__attribute__((__format__ (__printf__, 2, 3)))
#endif
inline ParseResultCode fail(ParseState& s, char const* format, ...) {
	va_list va;
	va_start(va, format);
	parser::set_error_va(s, format, va);
	va_end(va);
	return ParseResultCode::fail;
}

/// Parse failure with expected sub-parser match.
///
/// Sets error if it hasn't been set before.
inline ParseResultCode fail_expected_sub_match(ParseState& s, StringRef parser_name) {
	if (!s.suppress_errors && s.error && fixed_array::empty(s.error->message)) {
		parser::set_error(s,
			"in %.*s: expected match, got none from sub-parser",
			parser_name.size, parser_name.data
		);
	}
	return ParseResultCode::fail;
}

/// Speculative parse yielded no match.
inline ParseResultCode no_match() {
	return ParseResultCode::no_match;
}

/// Parse success.
inline ParseResultCode ok() {
	return ParseResultCode::ok;
}

/// Parse success with result.
inline ParseResultCode ok(ParseState& s, ParseResult&& r) {
	parser::push(s, rvalue_ref(r));
	return ParseResultCode::ok;
}

} // namespace parser
} // namespace togo
