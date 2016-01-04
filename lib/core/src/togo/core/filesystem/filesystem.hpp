#line 2 "togo/core/filesystem/filesystem.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Filesystem interface.
@ingroup lib_core_filesystem
*/

#pragma once

// igen-source: filesystem/filesystem_li.cpp

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/filesystem/types.hpp>

#include <togo/core/filesystem/filesystem.gen_interface>

namespace togo {

namespace filesystem {

/**
	@addtogroup lib_core_filesystem
	@{
*/

/// Path to the process executable parent directory.
StringRef exec_dir();

/// Working directory path.
///
/// Returns the size of str, or 0 if an error occurred.
unsigned working_dir(char* str, unsigned capacity);

/// Working directory path.
template<unsigned N>
inline unsigned working_dir(char (&str)[N]) {
	return working_dir(str, N);
}

/// Working directory path.
template<unsigned N>
inline unsigned working_dir(FixedArray<char, N>& str) {
	unsigned const size = working_dir(str._data, N);
	fixed_array::resize(str, size);
	return size;
}

/// Set the working directory path.
bool set_working_dir(StringRef path);

/// Whether path exists.
bool exists(StringRef path);

/// Whether path is a file.
bool is_file(StringRef path);

/// Whether path is a directory.
bool is_directory(StringRef path);

/// Last-modified time of file in seconds since the POSIX epoch.
///
/// Returns 0 on error. Only time-travelers would have a legitimate reason
/// for such a timestamp, and they are not welcome here.
u64 time_last_modified(StringRef path);

/// Size of file in bytes.
///
/// Returns 0 on error.
u64 file_size(StringRef path);

/// Create a file.
///
/// Unless overwrite == true, this will fail if the destination already exists.
bool create_file(StringRef path, bool overwrite = false);

/// Remove a file.
///
/// Unless accept_nonexistent == true, this will fail if the file does not
/// exist.
bool remove_file(StringRef path, bool accept_nonexistent = false);

/// Move a file.
///
/// If dest already exists, this will fail.
bool move_file(StringRef src, StringRef dest);

/// Copy a file.
///
/// Unless overwrite == true, this will fail if the destination already exists.
bool copy_file(StringRef src, StringRef dest, bool overwrite = false);

/// Create a directory.
///
/// Unless accept_exists == true, this will fail if the directory already
/// exists.
bool create_directory(StringRef path, bool accept_exists = false);

/// Remove a directory.
///
/// The directory must be empty.
/// Unless accept_nonexistent == true, this will fail if the directory does not
/// exist.
bool remove_directory(StringRef path, bool accept_nonexistent = false);

/** @} */ // end of doc-group lib_core_filesystem

} // namespace filesystem

/// Resets the working directory to its pre-initialization path.
inline WorkingDirScope::~WorkingDirScope() {
	TOGO_ASSERTE(filesystem::set_working_dir(_prev_path));
}

/// Stores the current working directory path and then sets the working
/// directory to path.
inline WorkingDirScope::WorkingDirScope(
	StringRef const& path
)
	: _prev_path()
{
	TOGO_ASSERTE(filesystem::working_dir(_prev_path));
	TOGO_ASSERTE(filesystem::set_working_dir(path));
}

} // namespace togo
