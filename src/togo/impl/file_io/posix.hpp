#line 2 "togo/impl/file_io/posix.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>

#include <cstdio>

namespace togo {

struct PosixFileStreamData {
	FILE* handle{nullptr};
};

using FileStreamData = PosixFileStreamData;

} // namespace togo
