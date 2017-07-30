#line 2 "togo/platform/notification/notification/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/platform/config.hpp>

#include <libnotify/notification.h>

namespace togo {
namespace notification {

// Forward declarations
struct Notification;

struct NotificationImpl {
	NotifyNotification* handle;

	NotificationImpl(
		Notification& notification,
		StringRef title,
		StringRef body
	);
};

} // namespace notification
} // namespace togo
