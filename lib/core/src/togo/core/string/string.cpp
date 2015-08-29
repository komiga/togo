#line 2 "togo/core/string/string.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/string/string.hpp>

#include <cstring>

namespace togo {

/// Compare two strings for equality.
///
/// This will short-circuit if lhs and rhs are not the same size.
bool string::compare_equal(StringRef const& lhs, StringRef const& rhs) {
	// TODO: Use memcmp()/block-wise compare?
	if (lhs.size != rhs.size) {
		return false;
	}
	unsigned size = lhs.size;
	char const* lp = lhs.data;
	char const* rp = rhs.data;
	while (size--) {
		if (*lp++ != *rp++) {
			return false;
		}
	}
	return true;
}

/// Copy string.
///
/// dst will be NUL-terminated.
/// The capacity of dst must be at least src.size + 1.
void string::copy(char* const dst, unsigned const capacity, StringRef const& src) {
	TOGO_ASSERT(capacity > src.size, "dst not large enough to store src + NUL");
	if (src.any()) {
		std::memcpy(dst, src.data, src.size);
	}
	dst[src.size] = '\0';
}

/// Trim trailing slashes from string.
///
/// The first trailing slash is replaced by a NUL.
/// Returns new size of string (not including NUL terminator).
unsigned string::trim_trailing_slashes(char* const string, unsigned const size) {
	TOGO_ASSERTE(string != nullptr);
	char* it = (string + size) - 1;
	while (it >= string && (*it == '/' || *it == '\\')) {
		--it;
	}
	++it;
	unsigned const new_size = it - string;
	if (new_size != size) {
		*it = '\0';
	}
	return new_size;
}

/// Ensure string has a trailing slash by appending if necessary.
///
/// str will be NUL terminated if it is modified.
/// If str is modified, its capacity must be at least size + 2.
/// Returns new size of str (not including NUL terminator).
unsigned string::ensure_trailing_slash(
	char* str,
	unsigned capacity,
	unsigned size
) {
	if (size == 0 || (str[size - 1] != '/' && str[size - 1] != '\\')) {
		TOGO_ASSERT(capacity > size + 2, "str not large enough to store slash and NUL");
		str[size++] = '/';
		str[size] = '\0';
	}
	return size;
}

} // namespace togo
