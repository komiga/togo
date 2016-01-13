#line 2 "togo/core/filesystem/filesystem.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/filesystem/types.hpp>
#include <togo/core/filesystem/filesystem.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/core/filesystem/filesystem/private.ipp>
	#include <togo/core/filesystem/filesystem/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif

namespace togo {



} // namespace togo
