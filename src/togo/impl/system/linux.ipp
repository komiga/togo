#line 2 "togo/impl/system/linux.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/utility.hpp>
#include <togo/system.hpp>

#include <unistd.h>

namespace togo {

unsigned system::num_cores() {
	signed const num = sysconf(_SC_NPROCESSORS_ONLN);
	return static_cast<unsigned>(max(0, num));
}

} // namespace togo
