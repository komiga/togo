#line 2 "togo/filesystem/filesystem.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/filesystem/types.hpp>
#include <togo/filesystem/filesystem.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/filesystem/filesystem/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif
