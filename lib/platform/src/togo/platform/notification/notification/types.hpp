#line 2 "togo/platform/notification/notification/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/platform/config.hpp>
#include <togo/platform/types.hpp>
#include <togo/platform/notification/types.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/platform/notification/notification/linux.hpp>
#else
	#error "missing Notification implementation for target platform"
#endif

namespace togo {
namespace notification {

struct Globals {
	bool initialized;
	signed default_lifetime;
};

extern Globals _globals;

} // namespace notification
} // namespace togo
