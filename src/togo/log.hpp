#line 2 "togo/log.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Logging.
@ingroup log
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>

namespace togo {
namespace log {

/**
	@addtogroup log
	@{
*/

#if defined(TOGO_COMPILER_CLANG) || \
	defined(TOGO_COMPILER_GCC)
	__attribute__((__format__ (__printf__, 1, 2)))
#endif
void printf(char const* const msg, ...);

/// Log message.
#define TOGO_LOG(msg) \
	::togo::log::printf(msg)

/// Log formatted message.
#define TOGO_LOGF(msg, ...) \
	::togo::log::printf(msg, __VA_ARGS__)

/// Log message (traced).
#define TOGO_LOG_TRACED(msg) \
	TOGO_LOGF("%s @ %4d: " msg, __FILE__, __LINE__)

/// Log formatted message (traced).
#define TOGO_LOGF_TRACED(msg, ...) \
	TOGO_LOGF("%s @ %4d: " msg, __FILE__, __LINE__, __VA_ARGS__)

/// Log error message.
#define TOGO_LOG_ERROR(msg) \
	TOGO_LOG_TRACED("error: " msg)

/// Log formatted error message.
#define TOGO_LOG_ERRORF(msg, ...) \
	TOGO_LOGF_TRACED("error: " msg, __VA_ARGS__)

#if defined(TOGO_DEBUG)
	#define TOGO_LOG_DEBUG(msg) \
		TOGO_LOG_TRACED("debug: " msg)

	#define TOGO_LOG_DEBUGF(msg, ...) \
		TOGO_LOGF_TRACED("debug: " msg, __VA_ARGS__)
#else
	/// Log debug message.
	#define TOGO_LOG_DEBUG(msg) (void(0))

	/// Log formatted debug message.
	#define TOGO_LOG_DEBUGF(msg, ...) (void(0))
#endif

/** @} */ // end of doc-group log

} // namespace log
} // namespace togo
