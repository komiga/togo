#line 2 "togo/system.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/system.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/impl/system/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif
