#line 2 "togo/core/parser/parse_state.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/parser/types.hpp>
#include <togo/core/parser/parse_state.hpp>

#include <cstdarg>
#include <cstdio>

namespace togo {

/// Reset parser state position.
void parse_state::reset_position(ParseState& s) {
	s.p = s.t = s.b;
	s.line = 0;
	s.column = 0;
}

/// Clear parser state results and error state.
void parse_state::clear_results(ParseState& s) {
	array::clear(s.results);
	parse_state::clear_error(s);
}

/// Clear error state.
void parse_state::clear_error(ParseState& s) {
	if (s.error) {
		parse_state::clear_error(*s.error);
	}
}

/// Clear error state.
void parse_state::clear_error(ParseError& error) {
	error.line = 0;
	error.column = 0;
	error.result_code = ParseResultCode::ok;
	fixed_array::clear(error.message);
}

/// Set parser state data.
void parse_state::set_data(ParseState& s, char const* b, char const* e) {
	s.b = s.p = s.t = b;
	s.e = e;
	if (s.b < s.e && *(s.e - 1) == '\0') {
		--s.e;
	}
}

/// Update line and column.
void parse_state::update_text_position(ParseState& s) {
	auto t = s.t;
	auto p = s.t;
	for (; p < s.p; ++p) {
		if (*p == '\n') {
			++s.line;
			s.column = 0;
			t = p;
		}
	}
	s.column += p - t;
	s.t = s.p;
}

/// Set parse error (va_list).
void parse_state::set_error_va(ParseState& s, char const* format, va_list va) {
	if (s.suppress_errors || !s.error) {
		return;
	}
	auto& error = *s.error;
	auto size = std::vsnprintf(error.message._data, error.message.CAPACITY, format, va);
	TOGO_ASSERTE(size > 0);
	fixed_array::resize(error.message, unsigned_cast(size));
	error.line = s.line;
	error.column = s.column;
	auto t = s.t;
	auto p = s.t;
	for (; p < s.p; ++p) {
		if (*p == '\n') {
			++error.line;
			error.column = 0;
			t = p;
		}
	}
	error.column += p - t;
}

} // namespace togo
