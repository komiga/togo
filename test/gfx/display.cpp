
#include <togo/assert.hpp>
#include <togo/system.hpp>
#include <togo/gfx/init.hpp>
#include <togo/gfx/display.hpp>
#include <togo/gfx/context.hpp>

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
		gfx::DisplayFlags::borderless,
		config
	);
	gfx::Context* const context = gfx::context::create(
		display,
		gfx::ContextFlags::none
	);

	system::sleep_ms(1000);

	gfx::context::destroy(context);
	gfx::display::destroy(display);

	gfx::shutdown();
	return 0;
}
