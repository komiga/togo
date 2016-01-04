#line 2 "togo/core/filesystem/filesystem.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/filesystem/types.hpp>
#include <togo/core/filesystem/filesystem.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/core/filesystem/filesystem/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif

namespace togo {

/// Get the directory part of a path.
StringRef filesystem::path_dir(StringRef const& path) {
	auto p = path.data + path.size - 1;
	if (path.size > 1) {
		while (*p == '/') {
			--p;
		}
	}
	for (; p >= path.data; --p) {
		if (*p == '/') {
			return {path.data, static_cast<unsigned>(p - path.data + (p == path.data))};
		}
	}
	return ".";
}

} // namespace togo
