
#include <togo/assert.hpp>
#include <togo/system.hpp>
#include <togo/gfx/init.hpp>
#include <togo/gfx/display.hpp>
#include <togo/input_buffer.hpp>

#include "../common/helpers.hpp"

using namespace togo;

signed
main() {
	core_init();
	gfx::init(3, 2);

	gfx::Config config{};
	config.color_bits = {8, 8, 8, 0};
	config.depth_bits = 16;
	config.stencil_bits = 0;
	config.msaa_num_buffers = 0;
	config.msaa_num_samples = 0;
	config.flags = gfx::ConfigFlags::double_buffered;

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
		while (input_buffer::poll(ib, event_type, event)) {
			TOGO_ASSERTE(event->display == display);
			switch (event_type) {
			case InputEventType::key:
				switch (event->key.code) {
				case KeyCode::escape:
					quit = true;
					break;

				case KeyCode::f1:
					if (event->key.action == KeyAction::release) {
						mouse_lock = !mouse_lock;
						gfx::display::set_mouse_lock(display, mouse_lock);
					}
					break;

				default: break;
				}
				break;

			case InputEventType::display_close_request:
				quit = true;
				break;

			default:
				break;
			}
		}
		system::sleep_ms(50);
	}
	input_buffer::remove_display(ib, display);
	gfx::display::destroy(display);
	gfx::shutdown();
	return 0;
}
