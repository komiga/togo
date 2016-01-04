#line 2 "togo/core/filesystem/filesystem/private.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/filesystem/filesystem.hpp>
#include <togo/core/filesystem/filesystem/private.hpp>

#include <cstring>

namespace togo {

namespace {
	static thread_local FixedArray<char, TOGO_PATH_MAX> _cstring_copy{};
} // anonymous namespace

StringRef filesystem::to_cstring(StringRef path, FixedArray<char, TOGO_PATH_MAX>* buffer) {
	if (!buffer) {
		buffer = &_cstring_copy;
	}
	if (path.empty()) {
		path = "";

	// FIXME: If data was allocated to path.size, this is OOB.
	// Undesireable, but sorta not actually a violation? Ho hum.
	} else if (path.data[path.size] != '\0') {
		string::copy(*buffer, path);
		path = *buffer;
	}
	return path;
}

} // namespace togo
