#line 2 "togo/core/filesystem/filesystem/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/string/types.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <linux/limits.h>
	#define TOGO_PATH_MAX PATH_MAX
#endif

namespace togo {
namespace filesystem {

StringRef to_cstring(StringRef path, FixedArray<char, TOGO_PATH_MAX>* buffer = nullptr);

} // namespace filesystem
} // namespace togo
