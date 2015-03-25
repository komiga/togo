#line 2 "togo/system/system.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief OS interface.
@ingroup system
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/string/types.hpp>
#include <togo/system/system.gen_interface>

namespace togo {
namespace system {

/**
	@addtogroup system
	@{
*/

/// Number of cores.
unsigned num_cores();

/// Sleep the current thread for a duration in milliseconds.
void sleep_ms(unsigned duration_ms);

/// Time in seconds from monotonic system clock.
///
/// The return value should have a precision of milliseconds or
/// better (typically nanoseconds).
float time_monotonic();

/// Get environment variable value.
///
/// name must be NUL-terminated.
/// If the return value will be stored & reused, it should be copied
/// to another value.
/// If the variable is modified, the return value will not reflect
/// the value at the point of call.
/// If the variable is removed, the return value will be invalid.
StringRef environment_variable(StringRef const& name);

/// Set environment variable value.
///
/// name must be NUL-terminated.
/// value must be NUL-terminated.
bool set_environment_variable(
	StringRef const& name,
	StringRef const& value
);

/// Remove environment variable.
bool remove_environment_variable(StringRef const& name);

/** @} */ // end of doc-group system

} // namespace system
} // namespace togo
