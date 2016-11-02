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
StringRef string::path_dir(StringRef const& path) {
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
StringRef string::path_file(StringRef const& path) {
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

/// Get the name and extension offsets within a path.
void string::path_parts(StringRef const& path, unsigned& name, unsigned& extension) {
	name = path.size;
	extension = path.size;
	if (path.empty()) {
		return;
	}
	auto t = find_tail(path);
	auto p = t ? (t + 1) : path.data;
	if (!is_dots(p, static_cast<unsigned>(end(path) - p))) {
		name = p - path.data;
		auto e = end(path) - 1;
		for (; e >= p; --e) {
			if (*e == '.') {
				break;
			}
		}
		if (e > p) {
			extension = e - path.data;
		}
	}
}

/// Get the name part of a path, if any.
StringRef string::path_name(StringRef const& path) {
	unsigned name, extension;
	string::path_parts(path, name, extension);
	return {path.data + name, path.data + extension};
}

/// Get the file extension part of a path, if any.
StringRef string::path_extension(StringRef const& path) {
	unsigned name, extension;
	string::path_parts(path, name, extension);
	return {path.data + min(path.size, extension + 1), path.data + path.size};
}

} // namespace togo
