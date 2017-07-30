
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/system/system.hpp>
#include <togo/platform/notification/notification.hpp>

#include <togo/support/test.hpp>

using namespace togo;

#define POLL \
	notification::poll();

signed main() {
	memory_init();
	TOGO_ASSERTE(notification::init("togo/platform/test/notfication/notification"));

{
	Notification notification{
		"111 Title 111",
		"111 Body 111",
		2000
	};
	TOGO_ASSERTE(notification::lifetime(notification) == 2000);
	TOGO_ASSERTE(notification::type(notification) == notification::Type::generic);
	TOGO_ASSERTE(notification::priority(notification) == notification::Priority::normal);
	POLL; (void)!notification::is_active(notification);
	TOGO_ASSERTE(notification::post(notification));
	POLL; (void)notification::is_active(notification);
	system::sleep_ms(1000);
	POLL; (void)notification::is_active(notification);
	system::sleep_ms(1200);
	POLL; (void)!notification::is_active(notification);

	notification::set_title(notification, "222 Title 222");
	notification::set_body(notification, "222 Body 222");
	TOGO_ASSERTE(notification::post(notification));
	POLL; (void)notification::is_active(notification);
	system::sleep_ms(3000);
	POLL; (void)!notification::is_active(notification);

	notification::set_lifetime(notification, 0);
	notification::set_title(notification, "333 Title 333");
	notification::set_body(notification, "333 Body 333");
	TOGO_ASSERTE(notification::post(notification));
	system::sleep_ms(2000);
	POLL; notification::dismiss(notification);
	POLL; (void)!notification::is_active(notification);
	system::sleep_ms(1000);
}

	notification::shutdown();
	return 0;
}
