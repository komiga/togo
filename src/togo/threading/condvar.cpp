#line 2 "togo/threading/condvar.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/threading/types.hpp>
#include <togo/threading/condvar.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/threading/condvar/posix.ipp>
#else
	#error "missing condvar implementation for target platform"
#endif
