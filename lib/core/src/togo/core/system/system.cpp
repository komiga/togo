#line 2 "togo/core/system/system.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/system/system.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/core/system/system/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif

#include <ctime>

namespace togo {

/// System time in seconds since the POSIX epoch.
u64 system::secs_since_epoch() {
	auto const s = std::time(nullptr);
	TOGO_ASSERT(s != static_cast<std::time_t>(-1), "std::time() failed");
	return static_cast<u64>(s);
}

} // namespace togo
