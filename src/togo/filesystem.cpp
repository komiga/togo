#line 2 "togo/filesystem.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/filesystem.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/impl/filesystem/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif
