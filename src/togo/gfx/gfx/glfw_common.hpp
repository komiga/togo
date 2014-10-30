#line 2 "togo/gfx/gfx/glfw_common.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>
#include <togo/gfx/gfx/common.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define TOGO_GLFW_CHECK(expr) \
	if (!(expr)) { goto glfw_error; }

namespace togo {
namespace gfx {

inline void glfw_config_setup(gfx::DisplayConfig const& config) {
	TOGO_ASSERT(
		enum_bool(config.flags & gfx::DisplayConfigFlags::double_buffered),
		"GLFW display is always double-buffered"
	);

	glfwWindowHint(GLFW_RED_BITS, config.color_bits.red);
	glfwWindowHint(GLFW_GREEN_BITS, config.color_bits.green);
	glfwWindowHint(GLFW_BLUE_BITS, config.color_bits.blue);
	glfwWindowHint(GLFW_ALPHA_BITS, config.color_bits.alpha);
	glfwWindowHint(GLFW_DEPTH_BITS, config.depth_bits);
	glfwWindowHint(GLFW_STENCIL_BITS, config.stencil_bits);

	if (config.msaa_num_buffers == 0 || config.msaa_num_samples == 0) {
		glfwWindowHint(GLFW_SAMPLES, 0);
	} else {
		glfwWindowHint(GLFW_SAMPLES, config.msaa_num_samples);
	}
	return;
}

} // namespace gfx
} // namespace togo
