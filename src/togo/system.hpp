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
unsigned num_cores();

/// Sleep the current thread for a duration in milliseconds.
void sleep_ms(unsigned duration_ms);

/// Get time in seconds from monotonic system clock.
///
/// The return value should have a precision of milliseconds or
/// better (typically nanoseconds).
float time_monotonic();

/// Get the system time in seconds since the POSIX epoch.
u64 secs_since_epoch();

/// Get the directory of the current process's executable.
char const* exec_dir();

/** @} */ // end of doc-group system

} // namespace system
} // namespace togo
