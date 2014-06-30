#line 2 "togo/condvar.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/threading_types.hpp>
#include <togo/condvar.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/condvar/posix.ipp>
#else
	#error "missing condvar implementation for target platform"
#endif
