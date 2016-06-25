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

/// Update line and column.
void parser::update_text_position(ParseState& s) {
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
void parser::set_error_va(ParseState& s, char const* format, va_list va) {
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
