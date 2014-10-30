#line 2 "togo/filesystem/directory_reader/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/filesystem/types.hpp>

namespace togo {
namespace directory_reader {

namespace {
	enum : u32 {
		OPT_NONE = 0,
		OPT_RECURSIVE		= 1 << 0,
		OPT_IGNORE_DOTFILES	= 1 << 1,
	};
} // anonymous namespace

inline void set_options(
	DirectoryReader& reader,
	bool const recursive,
	bool const ignore_dotfiles
) {
	reader._options
		= (recursive       ? OPT_RECURSIVE : OPT_NONE)
		| (ignore_dotfiles ? OPT_IGNORE_DOTFILES : OPT_NONE)
	;
}

inline bool option_recursive(
	DirectoryReader const& reader
) {
	return reader._options & OPT_RECURSIVE;
}

inline bool option_ignore_dotfiles(
	DirectoryReader const& reader
) {
	return reader._options & OPT_IGNORE_DOTFILES;
}

} // namespace directory_reader
} // namespace togo
