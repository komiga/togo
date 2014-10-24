#line 2 "togo/directory_reader.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief DirectoryReader interface.
@ingroup filesystem
@ingroup filesystem_directory_reader
*/

#pragma once

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/string_types.hpp>
#include <togo/filesystem_types.hpp>

namespace togo {

namespace directory_reader {

/**
	@addtogroup filesystem_directory_reader
	@{
*/

/// Open directory.
///
/// An assertion will fail if the directory reader is already open.
/// ignore_dotfiles will ignore regular directories and files that
/// start with ".".
bool open(
	DirectoryReader& reader,
	StringRef const& path,
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

/** @} */ // end of doc-group filesystem_directory_reader

} // namespace directory_reader

} // namespace togo
