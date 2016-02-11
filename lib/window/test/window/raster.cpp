
#include <togo/core/error/assert.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/vector/2_type.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/random/random.hpp>
#include <togo/image/pixmap/pixmap.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/input/input.hpp>
#include <togo/window/input/input_buffer.hpp>

#include <togo/support/test.hpp>

using namespace togo;

void draw(Window* window) {
	TOGO_LOG_TRACED("draw()\n");
	XS64M s{system::secs_since_epoch()};
	IntUDist<u8> dist{0, 255};
	Color color{
		random::next_udist(s, dist),
		random::next_udist(s, dist),
		random::next_udist(s, dist)
	};
	pixmap::fill(window::backbuffer(window), color);
	window::push_backbuffer(window);
}

signed main() {
	memory_init();

#if defined(TOGO_WINDOW_BACKEND_SUPPORTS_RASTER)
	window::init(0, 0);

	Window* const window = window::create_raster(
		"togo window",
		UVec2{1024, 768},
		WindowFlags::borderless |
		WindowFlags::resizable
	);
	InputBuffer ib{memory::default_allocator()};
	input_buffer::add_window(ib, window);

	bool quit = false;
	bool dirty = false;
	bool mouse_lock = false;
	window::set_mouse_lock(window, mouse_lock);

	InputEventType event_type{};
	InputEvent const* event = nullptr;
	for (;;) {
		dirty = false;
		input_buffer::update(ib);
		while (input_buffer::poll(ib, event_type, event)) {
			TOGO_ASSERTE(event->window == window);
			if (event_type == InputEventType::window_backbuffer_dirtied) {
				dirty = true;
			} else if (event_type == InputEventType::window_close_request) {
				quit = true;
				break;
			}
		}
		if (quit) {
			break;
		}
		if (input::key_released(window, KeyCode::escape)) {
			quit = true;
		}
		if (input::key_released(window, KeyCode::f1)) {
			mouse_lock = !mouse_lock;
			window::set_mouse_lock(window, mouse_lock);
			TOGO_LOG("mouse lock toggled\n");
		}
		if (dirty) {
			draw(window);
		}
		system::sleep_ms(50);
	}
	input_buffer::remove_window(ib, window);
	window::destroy(window);
	window::shutdown();
#else
	TOGO_LOG("raster windows are not supported by backend\n");
#endif

	return 0;
}
