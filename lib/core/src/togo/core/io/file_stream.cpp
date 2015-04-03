#line 2 "togo/core/io/file_stream.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/io/file_stream.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/core/io/file_stream/posix.ipp>
#else
	#error "missing file_io implementation for target platform"
#endif
