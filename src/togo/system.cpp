#line 2 "togo/system.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/assert.hpp>
#include <togo/system.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/impl/system/linux.ipp>
#else
	#error "unimplemented for target platform"
#endif

#include <ctime>

namespace togo {

u64 system::secs_since_epoch() {
	auto const s = std::time(nullptr);
	TOGO_ASSERT(s != static_cast<std::time_t>(-1), "std::time() failed");
	return static_cast<u64>(s);
}

} // namespace togo
