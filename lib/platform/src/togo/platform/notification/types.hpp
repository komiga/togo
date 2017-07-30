#line 2 "togo/notification/notification/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Notification types.
@ingroup lib_platform_types
@ingroup lib_platform_notification
*/

#pragma once

#include <togo/platform/config.hpp>
#include <togo/platform/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/lua/types.hpp>

namespace togo {
namespace notification {

/**
	@addtogroup lib_platform_notification
	@{
*/

/// Notification type.
enum class Type : unsigned {
	generic,

	/// Indicates that an error has occurred.
	error,

	/// Indicates that the application is persisting (e.g., a background
	/// service/task).
	persisting_process,

	/// Indicates that an alarm has triggered.
	alarm,

	/// Indicates that a message has arrived (e.g., SMS).
	message,

	/// Indicates that the application is playing media.
	media,
};
static constexpr unsigned const c_num_type = unsigned_cast(Type::media) + 1;

/// Notification priority.
enum class Priority : unsigned {
	normal,
	low,
	high,
};
static constexpr unsigned const c_num_priority = unsigned_cast(Priority::high) + 1;

/// Maximum lifetime of a notification in milliseconds (8m74s, 524.287s, 524287ms, 0x7FFFF).
static constexpr signed const MAX_LIFETIME = 0x7FFFF;

struct NotificationImpl;

/// Notification.
struct Notification {
	TOGO_LUA_MARK_USERDATA(Notification);

	u32 _properties;
	NotificationImpl* _impl;

	Notification(Notification&&) = default;
	Notification& operator=(Notification&&) = default;

	Notification(Notification const&) = delete;
	Notification& operator=(Notification const&) = delete;

	Notification(
		StringRef title,
		StringRef body,
		signed lifetime = -1,
		Type type = Type::generic,
		Priority priority = Priority::normal
	);
	~Notification();
};

/** @} */ // end of doc-group lib_platform_notification
} // namespace notification

using notification::Notification;

} // namespace togo
