#line 2 "togo/core/log/test.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Test logging.
@ingroup log
*/

#include <togo/core/config.hpp>
#include <togo/core/log/log.hpp>

namespace togo {
namespace log {

/**
	@addtogroup log
	@{
*/

#include <togo/core/log/test_unconfigure.hpp>

#if defined(TOGO_TEST_LOG_ENABLE)
	#define TOGO_TEST_LOG(msg)				TOGO_LOG(msg)
	#define TOGO_TEST_LOGF(msg, ...)		TOGO_LOGF(msg, __VA_ARGS__)
	#define TOGO_TEST_LOG_ERROR(msg)		TOGO_LOG_ERROR(msg)
	#define TOGO_TEST_LOG_ERRORF(msg, ...)	TOGO_LOG_ERRORF(msg, __VA_ARGS__)
#else
	/// Log message (test).
	#define TOGO_TEST_LOG(msg) (void(0))

	/// Log formatted message (test).
	#define TOGO_TEST_LOGF(msg, ...) (void(0))

	/// Log error message (test).
	#define TOGO_TEST_LOG_ERROR(msg) (void(0))

	/// Log formatted error message (test).
	#define TOGO_TEST_LOG_ERRORF(msg, ...) (void(0))
#endif

#if defined(TOGO_TEST_LOG_ENABLE) && defined(TOGO_DEBUG)
	#define TOGO_TEST_LOG_DEBUG(msg)			TOGO_LOG_DEBUG(msg)
	#define TOGO_TEST_LOG_DEBUGF(msg, ...)		TOGO_LOG_DEBUGF(msg, __VA_ARGS__)
#else
	/// Log debug message (test).
	#define TOGO_TEST_LOG_DEBUG(msg) (void(0))

	/// Log formatted debug message (test).
	#define TOGO_TEST_LOG_DEBUGF(msg, ...) (void(0))
#endif

#undef TOGO_TEST_LOG_ENABLE

/** @} */ // end of doc-group log

} // namespace log
} // namespace togo
