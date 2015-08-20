
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/system/system.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/input/input.hpp>
#include <togo/window/input/input_buffer.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main() {
	memory_init();

#if defined(TOGO_WINDOW_BACKEND_SUPPORTS_OPENGL)
	window::init(3, 3);

	WindowOpenGLConfig config{};
	config.color_bits = {8, 8, 8, 0};
	config.depth_bits = 16;
	config.stencil_bits = 0;
	config.msaa_num_buffers = 0;
	config.msaa_num_samples = 0;
	config.flags = WindowOpenGLConfig::Flags::double_buffered;

	Window* const window = window::create_opengl(
		"togo window",
		UVec2{1024, 768},
		WindowFlags::borderless |
		WindowFlags::resizable,
		config
	);
	InputBuffer ib{memory::default_allocator()};
	input_buffer::add_window(ib, window);

	bool quit = false;
	bool mouse_lock = false;
	window::set_mouse_lock(window, mouse_lock);

	InputEventType event_type{};
	InputEvent const* event = nullptr;
	while (!quit) {
		input_buffer::update(ib);
		while (input_buffer::poll(ib, event_type, event)) {
			TOGO_ASSERTE(event->window == window);
			if (event_type == InputEventType::window_close_request) {
				quit = true;
			}
		}
		if (input::key_released(window, KeyCode::escape)) {
			quit = true;
		}
		if (input::key_released(window, KeyCode::f1)) {
			mouse_lock = !mouse_lock;
			window::set_mouse_lock(window, mouse_lock);
			TOGO_LOG("mouse lock toggled\n");
		}
		window::swap_buffers(window);
		system::sleep_ms(50);
	}
	input_buffer::remove_window(ib, window);
	window::destroy(window);
	window::shutdown();
#else
	TOGO_LOG("OpenGL windows are not supported by backend\n");
#endif

	return 0;
}
