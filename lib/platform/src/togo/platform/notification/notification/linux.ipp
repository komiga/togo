#line 2 "togo/platform/notification/notification/linux.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/platform/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/temp_allocator.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/platform/notification/notification.hpp>
#include <togo/platform/notification/internal.hpp>
#include <togo/platform/notification/notification/private.hpp>
#include <togo/platform/notification/notification/linux.hpp>

#include <glib.h>
#include <libnotify/notify.h>

namespace togo {

namespace notification {

namespace {

static StringRef s_type_to_category[]{
	// generic
	{},
	// error
	{},
	// persisting_process
	{},
	// alarm
	{},
	// message
	"im.received",
	// media
	{},
};
static_assert(array_extent(s_type_to_category) == notification::c_num_type, "");

static NotifyUrgency s_priority_to_urgency[]{
	// normal
	NOTIFY_URGENCY_NORMAL,
	// low
	NOTIFY_URGENCY_LOW,
	// high
	NOTIFY_URGENCY_CRITICAL,
};
static_assert(array_extent(s_priority_to_urgency) == notification::c_num_priority, "");

enum class ClosedReason : signed {
	unclosed = -1,
	expired,
	user,
	host,
	undefined,
};
static StringRef s_closed_reason_all[]{
	"unclosed",
	"expired",
	"user",
	"host",
	"undefined",
};
static StringRef* s_closed_reason = s_closed_reason_all + 1;

static ClosedReason get_closed_reason(NotifyNotification* handle) {
	return static_cast<ClosedReason>(notify_notification_get_closed_reason(handle));
}

static ClosedReason get_closed_reason(Notification& notification) {
	return get_closed_reason(notification._impl->handle);
}

static void signal_handler_closed(
	NotifyNotification* /*handle*/,
	gpointer userdata
) {
	auto& notification = *static_cast<Notification*>(userdata);
	// auto closed_reason = get_closed_reason(notification);
	// TOGO_LOG_DEBUGF("signal_handler_closed() %s\n", s_closed_reason[signed_cast(closed_reason)]);
	notification::set_posted(notification, false);
}

} // anonymous namespace

NotificationImpl::NotificationImpl(
	Notification& notification,
	StringRef title,
	StringRef body
)
	: handle(notify_notification_new("X", nullptr, nullptr))
{
	auto handler_id = g_signal_connect(G_OBJECT(handle),
		"closed",
		G_CALLBACK(signal_handler_closed),
		&notification
	);
	TOGO_ASSERTE(handler_id > 0);

	notification::set_title(notification, title);
	notification::set_body(notification, body);
}

} // namespace notification

bool notification::init_impl(StringRef app_name) {
	FixedArray<char, 64> app_name_cstr;
	string::copy(app_name_cstr, app_name);
	return notify_init(begin(app_name_cstr));
}

void notification::shutdown_impl() {
	notify_uninit();
}

bool notification::poll_impl(bool wait) {
	auto event_processed = g_main_context_iteration(
		g_main_context_get_thread_default(),
		wait
	);
	// TOGO_LOG_DEBUGF("poll() %s\n", event_processed ? "true" : "false");
	return event_processed;
}

void notification::set_app_name(StringRef app_name) {
	FixedArray<char, 64> app_name_cstr;
	string::copy(app_name_cstr, app_name);
	notify_set_app_name(begin(app_name_cstr));
}

void notification::destroy_impl(Notification& notification) {
	g_object_unref(G_OBJECT(notification._impl->handle));
	notification._impl->handle = nullptr;
}

void notification::set_title(Notification& notification, StringRef title) {
	auto handle = notification._impl->handle;
	TempAllocator<512> a;
	Array<char> title_cstr{a};
	string::copy(title_cstr, title);
	g_object_set(G_OBJECT(handle), "summary", begin(title_cstr), nullptr);
}

void notification::set_body(Notification& notification, StringRef body) {
	auto handle = notification._impl->handle;
	TempAllocator<1024> a;
	Array<char> body_cstr{a};
	string::copy(body_cstr, body);
	g_object_set(G_OBJECT(handle), "body", begin(body_cstr), nullptr);
}

bool notification::is_active(Notification& notification) {
	if (notification::was_posted(notification)) {
		auto closed_reason = get_closed_reason(notification);
		// TOGO_LOG_DEBUGF("is_active() %s\n", s_closed_reason[signed_cast(closed_reason)]);
		if (closed_reason == ClosedReason::unclosed) {
			return true;
		}
		notification::set_posted(notification, false);
		return false;
	}
	return false;
}

bool notification::post(Notification& notification) {
	auto handle = notification._impl->handle;
	if (notification::is_modified(notification)) {
		notification::set_modified(notification, false);
		auto category = s_type_to_category[
			unsigned_cast(notification::type(notification))
		];
		if (category.any()) {
			notify_notification_set_category(handle, category.data);
		} else {
			// ugh, notify_notification_set_category() does NOTHING when given null and {'\0'}
			notify_notification_set_hint(handle, "category", nullptr);
		}
		auto urgency = s_priority_to_urgency[
			unsigned_cast(notification::priority(notification))
		];
		notify_notification_set_urgency(handle, urgency);
	}
	if (!notification::is_active(notification)) {
		signed lifetime = notification::lifetime(notification);
		if (lifetime == -1) {
			lifetime = notification::default_lifetime();
		}
		switch (lifetime) {
		case -1: lifetime = NOTIFY_EXPIRES_DEFAULT; break;
		case  0: lifetime = NOTIFY_EXPIRES_NEVER; break;
		}
		notify_notification_set_timeout(handle, lifetime);
	}

	GError* error = nullptr;
	if (!notify_notification_show(handle, &error)) {
		TOGO_LOG_ERRORF("notify_notification_show(): %s\n", error->message);
		g_error_free(error);
		return false;
	}
	notification::set_posted(notification, true);
	return true;
}

void notification::dismiss(Notification& notification) {
	auto handle = notification._impl->handle;
	if (notification::was_posted(notification)) {
		GError* error = nullptr;
		if (!notify_notification_close(handle, &error)) {
			TOGO_LOG_ERRORF("notify_notification_close(): %s\n", error->message);
			g_error_free(error);
		}
		notification::set_posted(notification, false);
	}
}

} // namespace togo
