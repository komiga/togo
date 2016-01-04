#line 2 "togo/core/filesystem/directory_reader.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief DirectoryReader interface.
@ingroup lib_core_filesystem
@ingroup lib_core_filesystem_directory_reader

@defgroup lib_core_filesystem_directory_reader DirectoryReader
@ingroup lib_core_filesystem
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/filesystem/types.hpp>

namespace togo {
namespace directory_reader {

/**
	@addtogroup lib_core_filesystem_directory_reader
	@{
*/

/// Open directory.
///
/// If prepend_path is true, read entries will include the directory path.
///
/// An assertion will fail if the directory reader is already open.
/// ignore_dotfiles will ignore regular directories and files that
/// start with ".".
bool open(
	DirectoryReader& reader,
	StringRef const& path,
	bool prepend_path,
	bool recursive,
	bool ignore_dotfiles
);

/// Close directory.
void close(DirectoryReader& reader);

/// Read next entry.
///
/// Directory entries will always end with a slash.
/// "." and ".." entries are always ignored.
bool read(
	DirectoryReader& reader,
	DirectoryEntry& entry,
	DirectoryEntry::Type type_mask
);

/** @} */ // end of doc-group lib_core_filesystem_directory_reader

} // namespace directory_reader
} // namespace togo
