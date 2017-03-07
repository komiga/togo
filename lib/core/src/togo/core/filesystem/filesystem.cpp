#line 2 "togo/core/filesystem/filesystem.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/filesystem/types.hpp>
#include <togo/core/filesystem/filesystem.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/core/filesystem/filesystem/private.ipp>
	#include <togo/core/filesystem/filesystem/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif

namespace togo {

/// Create a directory and all of its parents.
///
/// Returns false if the path could not be created in its entirety.
bool filesystem::create_directory_whole(StringRef const path) {
	if (path.empty()) {
		return false;
	}

	// NB: should be proofed against Windows UNC paths
	bool exists = false;
	unsigned depth = 0;
	unsigned last_size = 0;
	StringRef sub{path};
	for (;
		last_size != sub.size && sub.any() && !string::path_is_root(sub);
		sub = string::path_dir(sub)
	) {
		last_size = sub.size;
		++depth;
		if ((exists = filesystem::is_directory(sub))) {
			break;
		}
	}

	if (depth == 0) {
		// root always exists
		return true;
	}

	// subtle: string::path_dir() could return a different string (e.g., "")
	sub.data = path.data;
	sub.size = last_size;
	auto p = end(sub);
	auto e = end(path);
	while (p <= e) {
		if (!filesystem::create_directory(sub, true)) {
			return false;
		}
		for (++p; p < e && !(*p == '/' || *p == '\\'); ++p);
		sub.size = p - begin(sub);
	}
	return true;
}

} // namespace togo
