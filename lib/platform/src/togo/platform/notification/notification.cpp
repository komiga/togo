#line 2 "togo/platform/notification/notification.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/platform/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/platform/notification/notification.hpp>
#include <togo/platform/notification/internal.hpp>

#if defined(TOGO_PLATFORM_LINUX)
	#include <togo/platform/notification/notification/private.ipp>
	#include <togo/platform/notification/notification/linux.ipp>
#else
	#error "missing Notification implementation for target platform"
#endif

namespace togo {

TOGO_LUA_MARK_USERDATA_ANCHOR(notification::Notification);

namespace notification {

Globals _globals{
	false,
	-1
};

Notification::Notification(
	StringRef title,
	StringRef body,
	signed lifetime,
	Type type,
	Priority priority
)
	: _properties(0)
	, _impl(nullptr)
{
	notification::set_type(*this, type);
	notification::set_lifetime(*this, lifetime);
	notification::set_priority(*this, priority);
	this->_impl = TOGO_ALLOCATE(memory::default_allocator(), NotificationImpl);
	new (this->_impl) NotificationImpl(*this, title, body);
}

Notification::~Notification() {
	notification::dismiss(*this);
	notification::destroy_impl(*this);
	TOGO_DEALLOCATE(memory::default_allocator(), this->_impl);
	this->_impl = nullptr;
}

} // namespace notification

/// Initialize the notification system.
bool notification::init(StringRef app_name) {
	TOGO_ASSERT(!_globals.initialized, "notification system has already been initialized");

	_globals.initialized = notification::init_impl(app_name);
	return _globals.initialized;
}

/// Shutdown the notification system.
void notification::shutdown() {
	TOGO_ASSERT(_globals.initialized, "notification system has not been initialized");

	notification::shutdown_impl();
	_globals.initialized = false;
}

/// Default lifetime for all notifications (milliseconds).
signed notification::default_lifetime() {
	return _globals.default_lifetime;
}

/// Set default lifetime (milliseconds).
///
/// If lifetime is <0, the platform default is used, if any, falling back to
/// 15000ms (15s) if the platform has no default.
/// If lifetime is 0, posted notifications stay active until dismissed.
/// Lifetime is capped at notification::MAX_LIFETIME.
void notification::set_default_lifetime(signed lifetime) {
	_globals.default_lifetime = clamp(lifetime, -1, notification::MAX_LIFETIME);
}

/// Poll for events.
///
/// Returns true if an event was processed.
///
/// If wait is true, this will block until an event occurs.
bool notification::poll(bool wait IGEN_DEFAULT(false)) {
	TOGO_ASSERT(_globals.initialized, "notification system has not been initialized");

	return notification::poll_impl(wait);
}

} // namespace togo
