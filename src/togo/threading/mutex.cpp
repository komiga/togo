#line 2 "togo/threading/mutex.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/threading/types.hpp>
#include <togo/threading/mutex.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/threading/mutex/posix.ipp>
#else
	#error "missing mutex implementation for target platform"
#endif
