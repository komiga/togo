#line 2 "togo/string.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/string_types.hpp>
#include <togo/string.hpp>

namespace togo {
namespace string {

// TODO: Use memcmp()/block-wise compare?
bool compare_equal(StringRef const& lhs, StringRef const& rhs) {
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

} // namespace string
} // namespace togo
