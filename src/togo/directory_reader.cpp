#line 2 "togo/directory_reader.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/filesystem_types.hpp>
#include <togo/directory_reader.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/filesystem/directory_reader/posix.ipp>
#else
	#error "missing DirectoryReader implementation for target platform"
#endif

namespace togo {

DirectoryReader::DirectoryReader()
	: _options(0)
	, _impl()
{}

DirectoryReader::~DirectoryReader() {
	directory_reader::close(*this);
}

} // namespace togo
