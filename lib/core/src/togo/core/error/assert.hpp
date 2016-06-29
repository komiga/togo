#line 2 "togo/core/error/assert.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Assertion macros and debugging.
@ingroup lib_core_error
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_core_error
	@{
*/

/// Abort the program with contextual information.
TOGO_VALIDATE_FORMAT_PARAM(3, 4)
[[noreturn]]
void error_abort(unsigned line, char const* file, char const* msg, ...);

#if !defined(TOGO_DISABLE_ASSERTIONS) && !defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	#define TOGO_ASSERT(expr, msg) \
		do { if (!(expr)) { ::togo::error_abort(__LINE__, __FILE__, \
		"Assertion failed: `" #expr "`\nAssertion message: " msg "\n"); } } while (false)
	#define TOGO_ASSERTF(expr, msg, ...) \
		do { if (!(expr)) { ::togo::error_abort(__LINE__, __FILE__, \
		"Assertion failed: `" #expr "`\nAssertion message: " msg "\n", __VA_ARGS__); } } while (false)
	#define TOGO_ASSERTE(expr) \
		do { if (!(expr)) { ::togo::error_abort(__LINE__, __FILE__, \
		"Assertion failed: `" #expr "`\n"); } } while (false)
#else
	/// Assertion with an error message.
	#define TOGO_ASSERT(expr, msg) ((void)0)
	/// Assertion with a formatted error message.
	#define TOGO_ASSERTF(expr, msg, ...) ((void)0)
	/// Assertion with no message.
	#define TOGO_ASSERTE(expr) ((void)0)
#endif

#if defined(TOGO_DEBUG) && !defined(TOGO_DISABLE_ASSERTIONS) && !defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	#define TOGO_DEBUG_ASSERT(expr, msg) TOGO_ASSERT(expr, msg)
	#define TOGO_DEBUG_ASSERTF(expr, msg, ...) TOGO_ASSERTF(expr, msg, __VA_ARGS__)
	#define TOGO_DEBUG_ASSERTE(expr) TOGO_ASSERTE(expr)
#else
	/// Debug assertion with an error message.
	#define TOGO_DEBUG_ASSERT(expr, msg) ((void)0)
	/// Debug assertion with a formatted error message.
	#define TOGO_DEBUG_ASSERTF(expr, msg, ...) ((void)0)
	/// Debug assertion with no message.
	#define TOGO_DEBUG_ASSERTE(expr) ((void)0)
#endif

/** @} */ // end of doc-group lib_core_error

} // namespace togo
