#line 2 "togo/core/threading/mutex.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/core/threading/mutex.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/core/threading/mutex/posix.ipp>
#else
	#error "missing mutex implementation for target platform"
#endif
