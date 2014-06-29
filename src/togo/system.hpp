#line 2 "togo/system.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file system.hpp
@brief OS interface.
@ingroup system
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>

namespace togo {
namespace system {

/**
	@addtogroup system
	@{
*/

/// Get the number of cores for the system's processor.
u32 num_cores();

/** @} */ // end of doc-group system

} // namespace system
} // namespace togo
