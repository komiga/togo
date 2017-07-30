#line 2 "togo/platform/notification/notification/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/platform/config.hpp>
#include <togo/platform/types.hpp>
#include <togo/core/string/types.hpp>
#include <togo/platform/notification/types.hpp>
#include <togo/platform/notification/internal.hpp>
#include <togo/platform/notification/notification.hpp>
#include <togo/platform/notification/notification/types.hpp>

namespace togo {
namespace notification {

bool init_impl(StringRef app_name);
void shutdown_impl();
bool poll_impl(bool wait);

void destroy_impl(Notification& notification);

inline void set_modified(Notification& notification, bool modified) {
	return internal::set_property(notification, S_MODIFIED, M_MODIFIED, modified);
}

inline bool is_modified(Notification const& notification) {
	return internal::get_property(notification, S_MODIFIED, M_MODIFIED);
}

inline void set_posted(Notification& notification, bool posted) {
	return internal::set_property(notification, S_POSTED, M_POSTED, posted);
}

inline bool was_posted(Notification const& notification) {
	return internal::get_property(notification, S_POSTED, M_POSTED);
}

} // namespace notification
} // namespace togo
