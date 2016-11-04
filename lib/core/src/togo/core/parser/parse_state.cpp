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
	parse_state::clear_error(s.error);
}

/// Clear error state.
void parse_state::clear_error(ParseError& error) {
	error.pos = 0;
	error.line = 0;
	error.column = 0;
	error.result_code = ParseResultCode::ok;
	array::clear(error.message);
}

/// Copy or move error state.
///
/// If both errors have the same allocator, src will be moved into dst.
void parse_state::copy_or_move_error(ParseError& dst, ParseError& src) {
	if (dst.message._allocator == src.message._allocator) {
		dst = rvalue_ref(src);
		return;
	}
	dst.pos = src.pos;
	dst.line = src.line;
	dst.column = src.column;
	dst.result_code = src.result_code;
	array::copy(dst.message, src.message);
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
	if (s.suppress_errors) {
		return;
	}
	va_list prospect_va;
	va_copy(prospect_va, va);
	auto size = std::vsnprintf(nullptr, 0, format, prospect_va) + 1;
	va_end(prospect_va);

	array::resize(s.error.message, size);
	std::vsnprintf(begin(s.error.message), size, format, va);
	s.error.pos = s.p - s.b;
	s.error.line = s.line;
	s.error.column = s.column;
	auto t = s.t;
	auto p = s.t;
	for (; p < s.p; ++p) {
		if (*p == '\n') {
			++s.error.line;
			s.error.column = 0;
			t = p;
		}
	}
	s.error.column += p - t;
}

} // namespace togo
