#line 2 "togo/log.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file log.hpp
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

/// Log type.
enum class Type : unsigned {
	general = 0,
	debug,
	error,
};

#if defined(TOGO_COMPILER_CLANG) || \
	defined(TOGO_COMPILER_GCC)
	__attribute__((__format__ (__printf__, 2, 3)))
#endif
void
printf(
	log::Type const l,
	char const* msg,
	...
);

#if defined(TOGO_COMPILER_CLANG) || \
	defined(TOGO_COMPILER_GCC)
	__attribute__((__format__ (__printf__, 4, 5)))
#endif
void
printf_trace(
	log::Type const l,
	unsigned line,
	char const* file,
	char const* msg,
	...
);

/// Log message.
#define TOGO_LOG(msg) \
	::togo::log::printf(::togo::log::Type::general, msg)

/// Log formatted message.
#define TOGO_LOGF(msg, ...) \
	::togo::log::printf(::togo::log::Type::general, msg, __VA_ARGS__)

/// Log error message.
#define TOGO_LOG_ERROR(msg) \
	::togo::log::printf(::togo::log::Type::error, msg)

/// Log formatted error message.
#define TOGO_LOG_ERRORF(msg, ...) \
	::togo::log::printf(::togo::log::Type::error, msg, __VA_ARGS__)

#if defined(TOGO_DEBUG)
	#define TOGO_LOG_DEBUG(msg) \
		::togo::log::printf_trace(::togo::log::Type::debug, __LINE__, __FILE__, msg)

	#define TOGO_LOG_DEBUGF(msg, ...) \
		::togo::log::printf_trace(::togo::log::Type::debug, __LINE__, __FILE__, msg, __VA_ARGS__)
#else
	/// Log debug message.
	#define TOGO_LOG_DEBUG(msg) (void(0))

	/// Log formatted debug message.
	#define TOGO_LOG_DEBUGF(msg, ...) (void(0))
#endif

/** @} */ // end of doc-group log

} // namespace log
} // namespace togo
