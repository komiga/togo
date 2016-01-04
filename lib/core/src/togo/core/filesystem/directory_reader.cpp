#line 2 "togo/core/filesystem/directory_reader.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/filesystem/types.hpp>
#include <togo/core/filesystem/directory_reader.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/core/filesystem/directory_reader/posix.ipp>
#else
	#error "missing DirectoryReader implementation for target platform"
#endif

namespace togo {

TOGO_LUA_MARK_USERDATA_ANCHOR(DirectoryReader);

DirectoryReader::DirectoryReader()
	: _options(0)
	, _impl()
{}

DirectoryReader::~DirectoryReader() {
	directory_reader::close(*this);
}

} // namespace togo
