
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/system/system.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/display.hpp>
#include <togo/game/input/input.hpp>
#include <togo/game/input/input_buffer.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main() {
	memory_init();
	gfx::init(3, 3);

	gfx::DisplayConfig config{};
	config.color_bits = {8, 8, 8, 0};
	config.depth_bits = 16;
	config.stencil_bits = 0;
	config.msaa_num_buffers = 0;
	config.msaa_num_samples = 0;
	config.flags = gfx::DisplayConfigFlags::double_buffered;

	gfx::Display* const display = gfx::display::create(
		"togo display",
		1024, 768,
		gfx::DisplayFlags::borderless |
		gfx::DisplayFlags::resizable,
		config
	);
	InputBuffer ib{memory::default_allocator()};
	input_buffer::add_display(ib, display);

	bool quit = false;
	bool mouse_lock = false;
	gfx::display::set_mouse_lock(display, mouse_lock);

	InputEventType event_type{};
	InputEvent const* event = nullptr;
	while (!quit) {
		input_buffer::update(ib);
		while (input_buffer::poll(ib, event_type, event)) {
			TOGO_ASSERTE(event->display == display);
			if (event_type == InputEventType::display_close_request) {
				quit = true;
			}
		}
		if (input::key_released(display, KeyCode::escape)) {
			quit = true;
		}
		if (input::key_released(display, KeyCode::f1)) {
			mouse_lock = !mouse_lock;
			gfx::display::set_mouse_lock(display, mouse_lock);
			TOGO_LOG("mouse lock toggled\n");
		}
		system::sleep_ms(50);
	}
	input_buffer::remove_display(ib, display);
	gfx::display::destroy(display);
	gfx::shutdown();
	return 0;
}
