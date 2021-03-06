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
#include <togo/core/string/string.hpp>
#include <togo/core/parser/types.hpp>
#include <togo/core/parser/parse_state.hpp>

#include <cstdarg>

#include <togo/core/parser/parse_state.gen_interface>

namespace togo {
namespace parse_state {

/// Initialize parser state.
inline void init(ParseState& s) {
	s.result_code = ParseResultCode::ok;
	s.suppress_results = 0;
	s.suppress_errors = 0;
	parse_state::clear_results(s);
	parse_state::reset_position(s);
}

/// Set parser state data from string.
inline void set_data(ParseState& s, StringRef const data) {
	parse_state::set_data(s, begin(data), end(data));
}

/// Set parser state data from array reference.
inline void set_data_array(ParseState& s, ArrayRef<char const> data) {
	parse_state::set_data(s, begin(data), end(data));
}

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

/// Pop results.
inline void pop(ParseState& s, unsigned num = 1) {
	TOGO_DEBUG_ASSERTE(num <= array::size(s.results));
	s.results._size -= num;
}

/// Pop results back to a size.
inline void set_num_results(ParseState& s, unsigned size) {
	TOGO_DEBUG_ASSERTE(size <= array::size(s.results));
	s.results._size = size;
}

/// Pop results back to a parse position.
inline void set_num_results(ParseState& s, ParsePosition const& pos) {
	parse_state::set_num_results(s, min(array::size(s.results), pos.i));
}

/// Push a null result.
inline void push(ParseState& s) {
	if (!s.suppress_results) {
		array::push_back(s.results, {null_tag{}});
	}
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

/// Get parse position.
inline ParsePosition position(ParseState const& s) {
	return {s.p, array::size(s.results)};
}

/// Set parse position.
inline void set_position(ParseState& s, ParsePosition const& pos) {
	s.p = pos.p;
	parse_state::set_num_results(s, pos);
}

/// The number of results at a position.
inline unsigned num_results(ParseState const& s, ParsePosition const& pos) {
	return pos.i > array::size(s.results) ? 0 : (array::size(s.results) - pos.i);
}

/// Get result by index.
inline ParseResult& result(ParseState& s, unsigned i) {
	return s.results[i];
}

/// Get result from a position.
inline ParseResult& result(ParseState& s, ParsePosition const& pos, unsigned i) {
	return s.results[pos.i + i];
}

/// Set parse error.
TOGO_VALIDATE_FORMAT_PARAM(2, 3)
inline void set_error(ParseState& s, char const* format, ...) {
	if (s.suppress_errors) {
		return;
	}
	va_list va;
	va_start(va, format);
	parse_state::set_error_va(s, format, va);
	va_end(va);
}

/// Parse failure.
inline ParseResultCode fail(ParseState const&) {
	return ParseResultCode::fail;
}

/// Parse failure with error message.
TOGO_VALIDATE_FORMAT_PARAM(2, 3)
inline ParseResultCode fail(ParseState& s, char const* format, ...) {
	va_list va;
	va_start(va, format);
	parse_state::set_error_va(s, format, va);
	va_end(va);
	return ParseResultCode::fail;
}

/// Parse failure with expected sub-parser match.
///
/// Sets error if it hasn't been set before.
inline ParseResultCode fail_expected_sub_match(ParseState& s, StringRef parser_name) {
	if (!s.suppress_errors && array::empty(s.error.message)) {
		parse_state::set_error(s,
			"in %.*s: expected match, got none from sub-parser",
			parser_name.size, parser_name.data
		);
	}
	return ParseResultCode::fail;
}

/// Speculative parse yielded no match.
inline ParseResultCode no_match(ParseState const&) {
	return ParseResultCode::no_match;
}

/// Parse success.
inline ParseResultCode ok(ParseState const&) {
	return ParseResultCode::ok;
}

/// Parse success with result.
inline ParseResultCode ok(ParseState& s, ParseResult&& r) {
	parse_state::push(s, rvalue_ref(r));
	return ParseResultCode::ok;
}

/// Parse success with results removed.
inline ParseResultCode ok_replace(ParseState& s, ParsePosition const& pos) {
	parse_state::set_num_results(s, pos);
	return ParseResultCode::ok;
}

/// Parse success with result replacing results.
inline ParseResultCode ok_replace(ParseState& s, ParsePosition const& pos, ParseResult&& r) {
	parse_state::set_num_results(s, pos);
	parse_state::push(s, rvalue_ref(r));
	return ParseResultCode::ok;
}

} // namespace parse_state
} // namespace togo
