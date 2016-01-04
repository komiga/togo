#line 2 "togo/core/filesystem/directory_reader/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/filesystem/types.hpp>

namespace togo {
namespace directory_reader {

namespace {
	enum : u32 {
		OPT_NONE = 0,
		OPT_PREPEND_PATH	= 1 << 0,
		OPT_RECURSIVE		= 1 << 1,
		OPT_IGNORE_DOTFILES	= 1 << 2,
	};
} // anonymous namespace

inline void set_options(
	DirectoryReader& reader,
	bool const prepend_path,
	bool const recursive,
	bool const ignore_dotfiles
) {
	reader._options
		= (prepend_path    ? OPT_PREPEND_PATH : OPT_NONE)
		| (recursive       ? OPT_RECURSIVE : OPT_NONE)
		| (ignore_dotfiles ? OPT_IGNORE_DOTFILES : OPT_NONE)
	;
}

inline bool option_prepend_path(DirectoryReader const& reader) {
	return reader._options & OPT_PREPEND_PATH;
}

inline bool option_recursive(DirectoryReader const& reader) {
	return reader._options & OPT_RECURSIVE;
}

inline bool option_ignore_dotfiles(DirectoryReader const& reader) {
	return reader._options & OPT_IGNORE_DOTFILES;
}

} // namespace directory_reader
} // namespace togo
