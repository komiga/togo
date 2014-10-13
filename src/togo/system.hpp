#line 2 "togo/system.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief OS interface.
@ingroup system
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/fixed_array.hpp>
#include <togo/string_types.hpp>

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

/// Get the path to the directory of the current process's executable.
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

/// Change the working directory.
///
/// path must be NUL-terminated.
bool change_working_dir(StringRef const& path);

/** @} */ // end of doc-group system

} // namespace system
} // namespace togo
