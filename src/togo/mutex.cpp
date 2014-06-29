#line 2 "togo/mutex.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/threading_types.hpp>
#include <togo/mutex.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/mutex/posix.ipp>
#else
	#error "missing mutex implementation for target platform"
#endif
