#line 2 "togo/platform/notification/notification.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Notification interface.
@ingroup lib_platform_notification
*/

#pragma once

// igen-source: notification/notification_li.cpp

#include <togo/platform/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/string/types.hpp>
#include <togo/platform/notification/types.hpp>
#include <togo/platform/notification/internal.hpp>

#include <togo/platform/notification/notification.gen_interface>

namespace togo {
namespace notification {

/**
	@addtogroup lib_platform_notification
	@{
*/

/// Set the application name.
void set_app_name(StringRef app_name);

/// Type.
inline Type type(Notification const& notification) {
	return static_cast<Type>(
		internal::get_property(notification, M_TYPE, S_TYPE)
	);
}

/// Set type.
inline void set_type(Notification& notification, Type type) {
	internal::set_property(notification, M_TYPE, S_TYPE, unsigned_cast(type));
	internal::set_property(notification, M_MODIFIED, S_MODIFIED, true);
}

/// Priority.
inline Priority priority(Notification const& notification) {
	return static_cast<Priority>(
		internal::get_property(notification, M_PRIORITY, S_PRIORITY)
	);
}

/// Set priority.
inline void set_priority(Notification& notification, Priority priority) {
	internal::set_property(notification, M_PRIORITY, S_PRIORITY, unsigned_cast(priority));
	internal::set_property(notification, M_MODIFIED, S_MODIFIED, true);
}

/// How long until a posted notification is made inactive (milliseconds).
inline signed lifetime(Notification const& notification) {
	signed value = signed_cast(internal::get_property(notification, M_LIFETIME, S_LIFETIME));
	return
		internal::get_property(notification, M_LIFETIME_SIGN, S_LIFETIME_SIGN)
		? -value
		: value
	;
}

/// Set lifetime (milliseconds).
///
/// If lifetime is <0, togo's default lifetime is used; see set_default_lifetime().
inline void set_lifetime(Notification& notification, signed lifetime) {
	bool sign = lifetime < 0;
	internal::set_property(notification, M_LIFETIME_SIGN, S_LIFETIME_SIGN, sign);
	internal::set_property(notification, M_LIFETIME, S_LIFETIME,
		min(unsigned_cast(sign ? -lifetime : lifetime), unsigned_cast(M_LIFETIME))
	);
	internal::set_property(notification, M_MODIFIED, S_MODIFIED, true);
}

/// Set title text.
void set_title(Notification& notification, StringRef title);

/// Set body text.
void set_body(Notification& notification, StringRef body);

/// Whether the notification is visible to the user.
bool is_active(Notification& notification);

/// Show the notification.
///
/// If the notification is already active, its state will be updated.
/// If the notification could not be shown or updated, false is returned.
bool post(Notification& notification);

/// Stop showing the notification if it is active.
void dismiss(Notification& notification);

/** @} */ // end of doc-group lib_platform_notification

} // namespace notification
} // namespace togo
