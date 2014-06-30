#line 2 "togo/file_stream.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/file_stream.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/file_stream/posix.ipp>
#else
	#error "missing file_stream implementation for target platform"
#endif
