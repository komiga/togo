#line 2 "togo/file_io.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/file_io.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/file_io/posix.ipp>
#else
	#error "missing file_io implementation for target platform"
#endif
