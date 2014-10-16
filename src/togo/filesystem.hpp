#line 2 "togo/filesystem.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Filesystem interface.
@ingroup filesystem
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/fixed_array.hpp>
#include <togo/string_types.hpp>
#include <togo/filesystem_types.hpp>

namespace togo {

namespace filesystem {

/**
	@addtogroup filesystem
	@{
*/

/// Get the path to the directory of the process's executable.
StringRef exec_dir();

/// Get the path to the working directory.
///
/// str will be NUL-terminated.
/// Returns the size of str, or 0 if an error occurred.
unsigned working_dir(char* str, unsigned capacity);

/// Get the path to the working directory.
template<unsigned N>
inline unsigned working_dir(char (&str)[N]) {
	return working_dir(str, N);
}

/// Get the path to the working directory.
template<unsigned N>
inline unsigned working_dir(FixedArray<char, N>& str) {
	unsigned const size = working_dir(str._data, N);
	fixed_array::resize(str, size);
	return size;
}

/// Set the working directory.
///
/// path must be NUL-terminated.
bool set_working_dir(StringRef const& path);

/// Check if a path is a file.
///
/// path must be NUL-terminated.
bool is_file(StringRef const& path);

/// Check if a path is a directory.
///
/// path must be NUL-terminated.
bool is_directory(StringRef const& path);

/// Create a file.
///
/// path must be NUL-terminated.
/// If the file already exists, this will fail.
bool create_file(StringRef const& path);

/// Remove a file.
///
/// path must be NUL-terminated.
bool remove_file(StringRef const& path);

/// Create a directory.
///
/// path must be NUL-terminated.
/// If the directory already exists, this will fail.
bool create_directory(StringRef const& path);

/// Remove a directory.
///
/// path must be NUL-terminated.
/// The directory must be empty.
bool remove_directory(StringRef const& path);

/** @} */ // end of doc-group filesystem

} // namespace filesystem

/// Resets the working directory to its pre-initialization path.
inline WorkingDirScope::~WorkingDirScope() {
	TOGO_ASSERTE(filesystem::set_working_dir(_prev_path));
}

/// Stores the current working directory path and then sets the
/// working directory to path.
inline WorkingDirScope::WorkingDirScope(
	StringRef const& path
)
	: _prev_path()
{
	TOGO_ASSERTE(filesystem::working_dir(_prev_path));
	TOGO_ASSERTE(filesystem::set_working_dir(path));
}

} // namespace togo
