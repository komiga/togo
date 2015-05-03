#line 2 "togo/window/window/impl/glfw.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>

#include <GLFW/glfw3.h>

namespace togo {

struct GLFWWindowImpl {
	GLFWwindow* handle;
};

using WindowImpl = GLFWWindowImpl;

#define TOGO_GLFW_CHECK(expr) \
	if (!(expr)) { goto glfw_error; }

} // namespace togo
