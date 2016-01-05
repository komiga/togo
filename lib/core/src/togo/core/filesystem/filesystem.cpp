#line 2 "togo/core/filesystem/filesystem.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/filesystem/types.hpp>
#include <togo/core/filesystem/filesystem.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/core/filesystem/filesystem/private.ipp>
	#include <togo/core/filesystem/filesystem/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif

namespace togo {

namespace {

inline static char const* find_tail(StringRef const& path) {
	for (auto p = end(path) - 1; p >= path.data; --p) {
		if (*p == '/') {
			return p;
		}
	}
	return nullptr;
}

inline static bool is_dots(char const* p, unsigned size) {
	return size <= 2 && p[0] == '.' && (size == 1 || p[1] == '.');
}

} // anonymous namespace

/// Get the directory part of a path.
StringRef filesystem::path_dir(StringRef const& path) {
	if (path.empty()) {
		return "";
	}
	auto t = find_tail(path);
	auto p = t ? (t + 1) : path.data;
	if (is_dots(p, static_cast<unsigned>(end(path) - p))) {
		return path;
	} else if (t) {
		return {path.data, static_cast<unsigned>(t - path.data) + (t == path.data)};
	}
	return "";
}

/// Get the file part of a path, if any.
StringRef filesystem::path_file(StringRef const& path) {
	if (path.empty()) {
		return "";
	}
	auto t = find_tail(path);
	auto p = t ? (t + 1) : path.data;
	auto size = static_cast<unsigned>(end(path) - p);
	if (is_dots(p, size)) {
		return "";
	} else if (t) {
		return {p, size};
	}
	return path;
}

} // namespace togo
