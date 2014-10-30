#line 2 "togo/string/string.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/string/string.hpp>

#include <cstring>

namespace togo {

// TODO: Use memcmp()/block-wise compare?
bool string::compare_equal(StringRef const& lhs, StringRef const& rhs) {
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

void string::copy(char* const dst, unsigned const capacity, StringRef const& src) {
	TOGO_ASSERT(capacity > src.size, "dst not large enough to store src + NUL");
	if (!src.empty()) {
		std::memcpy(dst, src.data, src.size);
	}
	dst[src.size] = '\0';
}

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

} // namespace togo
