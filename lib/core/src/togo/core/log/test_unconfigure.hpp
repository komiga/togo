#line 2 "togo/core/log/test_unconfigure.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Undefine all test logging macros.
@ingroup lib_core_log
*/

namespace togo {
namespace log {

#undef TOGO_TEST_LOG
#undef TOGO_TEST_LOGF
#undef TOGO_TEST_LOG_ERROR
#undef TOGO_TEST_LOG_ERRORF
#undef TOGO_TEST_LOG_DEBUG
#undef TOGO_TEST_LOG_DEBUGF

} // namespace log
} // namespace togo
