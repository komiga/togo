#line 2 "togo/core/system/system.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief OS interface.
@ingroup lib_core_system
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/system/system.gen_interface>

namespace togo {
namespace system {

/**
	@addtogroup lib_core_system
	@{
*/

/// Process ID.
unsigned pid();

/// Number of CPU cores.
unsigned num_cores();

/// Hostname.
StringRef hostname();

/// Sleep the current thread for a duration in milliseconds.
void sleep_ms(unsigned duration_ms);

/// Time in seconds from monotonic system clock.
///
/// The return value should have a precision of milliseconds or
/// better (typically nanoseconds).
f64 time_monotonic();

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

/** @} */ // end of doc-group lib_core_system

} // namespace system
} // namespace togo
