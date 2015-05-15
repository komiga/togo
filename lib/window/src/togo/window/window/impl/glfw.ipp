#line 2 "togo/window/window/impl/glfw.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/window/impl/private.hpp>
#include <togo/window/window/impl/glfw.hpp>
#include <togo/window/input/types.hpp>

#include <GLFW/glfw3.h>

#include <cmath>

namespace togo {

void window::init_impl() {
	TOGO_GLFW_CHECK(glfwInit());

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _globals.context_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _globals.context_minor);
	if (_globals.context_major >= 3) {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}
	return;

glfw_error:
	TOGO_ASSERT(false, "failed to initialize window backend\n");
}

void window::shutdown_impl() {
	glfwTerminate();
}

Window* window::create_opengl(
	StringRef title,
	UVec2 size,
	WindowFlags flags,
	WindowOpenGLConfig const& config,
	Window* share_with,
	Allocator& allocator
) {
	glfwWindowHint(
		GLFW_DECORATED,
		enum_bool(flags & WindowFlags::borderless) ? GL_TRUE : GL_FALSE
	);
	glfwWindowHint(
		GLFW_RESIZABLE,
		enum_bool(flags & WindowFlags::resizable) ? GL_TRUE : GL_FALSE
	);

	GLFWwindow* const share_with_handle
		= share_with
		? share_with->_impl.handle
		: nullptr
	;

	TOGO_ASSERT(
		enum_bool(config.flags & WindowOpenGLConfig::Flags::double_buffered),
		"GLFW window is always double-buffered"
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

	GLFWwindow* const handle = glfwCreateWindow(
		size.x, size.y, title.data, nullptr, share_with_handle
	);
	TOGO_ASSERT(handle, "failed to create window");
	glfwMakeContextCurrent(handle);
	glew_init();

	Window* const window = TOGO_CONSTRUCT(
		allocator, Window, size, flags, config, allocator,
		GLFWWindowImpl{handle}
	);
	glfwSetWindowUserPointer(handle, window);
	return window;
}

void window::destroy(Window* window) {
	Allocator& allocator = *window->_allocator;
	glfwDestroyWindow(window->_impl.handle);
	TOGO_DESTROY(allocator, window);
}

void window::set_title(Window* window, StringRef title) {
	glfwSetWindowTitle(window->_impl.handle, title.data);
}

void window::set_mouse_lock(Window* /*window*/, bool /*enable*/) {
	TOGO_LOG_DEBUG("window::set_mouse_lock() not implemented for GLFW\n");
	// GLFW TODO: Need a GLFW_CURSOR_CONSTRAINED (i.e., cursor
	// locked to window as in GLFW_CURSOR_DISABLED, but still
	// visible).
	/*glfwSetInputMode(
		window->_impl.handle,
		GLFW_CURSOR,
		enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
	);*/
}

void window::set_swap_mode(Window* window, WindowSwapMode mode) {
	signed interval;
	switch (mode) {
	case WindowSwapMode::immediate:		interval = 0; break;
	case WindowSwapMode::wait_refresh:	interval = 1; break;
	}
	window::bind_context(window);
	glfwSwapInterval(interval);
}

void window::bind_context(Window* window) {
	glfwMakeContextCurrent(window->_impl.handle);
}

void window::unbind_context() {
	glfwMakeContextCurrent(nullptr);
}

void window::swap_buffers(Window* window) {
	glfwSwapBuffers(window->_impl.handle);
}

// private

inline KeyCode glfw_tl_key_code(signed const key) {
	switch (key) {
	case GLFW_KEY_SPACE:				return KeyCode::space;
	case GLFW_KEY_APOSTROPHE:			return KeyCode::apostrophe;
	case GLFW_KEY_COMMA:				return KeyCode::comma;
	case GLFW_KEY_MINUS:				return KeyCode::minus;
	case GLFW_KEY_PERIOD:				return KeyCode::period;
	case GLFW_KEY_SLASH:				return KeyCode::slash;
	case GLFW_KEY_0:					return KeyCode::n0;
	case GLFW_KEY_1:					return KeyCode::n1;
	case GLFW_KEY_2:					return KeyCode::n2;
	case GLFW_KEY_3:					return KeyCode::n3;
	case GLFW_KEY_4:					return KeyCode::n4;
	case GLFW_KEY_5:					return KeyCode::n5;
	case GLFW_KEY_6:					return KeyCode::n6;
	case GLFW_KEY_7:					return KeyCode::n7;
	case GLFW_KEY_8:					return KeyCode::n8;
	case GLFW_KEY_9:					return KeyCode::n9;
	case GLFW_KEY_SEMICOLON:			return KeyCode::semicolon;
	case GLFW_KEY_EQUAL:				return KeyCode::equal;
	case GLFW_KEY_A:					return KeyCode::a;
	case GLFW_KEY_B:					return KeyCode::b;
	case GLFW_KEY_C:					return KeyCode::c;
	case GLFW_KEY_D:					return KeyCode::d;
	case GLFW_KEY_E:					return KeyCode::e;
	case GLFW_KEY_F:					return KeyCode::f;
	case GLFW_KEY_G:					return KeyCode::g;
	case GLFW_KEY_H:					return KeyCode::h;
	case GLFW_KEY_I:					return KeyCode::i;
	case GLFW_KEY_J:					return KeyCode::j;
	case GLFW_KEY_K:					return KeyCode::k;
	case GLFW_KEY_L:					return KeyCode::l;
	case GLFW_KEY_M:					return KeyCode::m;
	case GLFW_KEY_N:					return KeyCode::n;
	case GLFW_KEY_O:					return KeyCode::o;
	case GLFW_KEY_P:					return KeyCode::p;
	case GLFW_KEY_Q:					return KeyCode::q;
	case GLFW_KEY_R:					return KeyCode::r;
	case GLFW_KEY_S:					return KeyCode::s;
	case GLFW_KEY_T:					return KeyCode::t;
	case GLFW_KEY_U:					return KeyCode::u;
	case GLFW_KEY_V:					return KeyCode::v;
	case GLFW_KEY_W:					return KeyCode::w;
	case GLFW_KEY_X:					return KeyCode::x;
	case GLFW_KEY_Y:					return KeyCode::y;
	case GLFW_KEY_Z:					return KeyCode::z;
	case GLFW_KEY_LEFT_BRACKET:			return KeyCode::left_bracket;
	case GLFW_KEY_BACKSLASH:			return KeyCode::backslash;
	case GLFW_KEY_RIGHT_BRACKET:		return KeyCode::right_bracket;
	case GLFW_KEY_GRAVE_ACCENT:			return KeyCode::grave;
	case GLFW_KEY_ESCAPE:				return KeyCode::escape;
	case GLFW_KEY_ENTER:				return KeyCode::enter;
	case GLFW_KEY_TAB:					return KeyCode::tab;
	case GLFW_KEY_BACKSPACE:			return KeyCode::backspace;
	case GLFW_KEY_INSERT:				return KeyCode::insert;
	case GLFW_KEY_DELETE:				return KeyCode::del;
	case GLFW_KEY_RIGHT:				return KeyCode::right;
	case GLFW_KEY_LEFT:					return KeyCode::left;
	case GLFW_KEY_DOWN:					return KeyCode::down;
	case GLFW_KEY_UP:					return KeyCode::up;
	case GLFW_KEY_PAGE_UP:				return KeyCode::page_up;
	case GLFW_KEY_PAGE_DOWN:			return KeyCode::page_down;
	case GLFW_KEY_HOME:					return KeyCode::home;
	case GLFW_KEY_END:					return KeyCode::end;
	case GLFW_KEY_CAPS_LOCK:			return KeyCode::caps_lock;
	case GLFW_KEY_SCROLL_LOCK:			return KeyCode::scroll_lock;
	case GLFW_KEY_NUM_LOCK:				return KeyCode::num_lock;
	case GLFW_KEY_PRINT_SCREEN:			return KeyCode::print_screen;
	case GLFW_KEY_PAUSE:				return KeyCode::pause;
	case GLFW_KEY_F1:					return KeyCode::f1;
	case GLFW_KEY_F2:					return KeyCode::f2;
	case GLFW_KEY_F3:					return KeyCode::f3;
	case GLFW_KEY_F4:					return KeyCode::f4;
	case GLFW_KEY_F5:					return KeyCode::f5;
	case GLFW_KEY_F6:					return KeyCode::f6;
	case GLFW_KEY_F7:					return KeyCode::f7;
	case GLFW_KEY_F8:					return KeyCode::f8;
	case GLFW_KEY_F9:					return KeyCode::f9;
	case GLFW_KEY_F10:					return KeyCode::f10;
	case GLFW_KEY_F11:					return KeyCode::f11;
	case GLFW_KEY_F12:					return KeyCode::f12;
	case GLFW_KEY_KP_0:					return KeyCode::numpad_0;
	case GLFW_KEY_KP_1:					return KeyCode::numpad_1;
	case GLFW_KEY_KP_2:					return KeyCode::numpad_2;
	case GLFW_KEY_KP_3:					return KeyCode::numpad_3;
	case GLFW_KEY_KP_4:					return KeyCode::numpad_4;
	case GLFW_KEY_KP_5:					return KeyCode::numpad_5;
	case GLFW_KEY_KP_6:					return KeyCode::numpad_6;
	case GLFW_KEY_KP_7:					return KeyCode::numpad_7;
	case GLFW_KEY_KP_8:					return KeyCode::numpad_8;
	case GLFW_KEY_KP_9:					return KeyCode::numpad_9;
	case GLFW_KEY_KP_DECIMAL:			return KeyCode::numpad_decimal;
	case GLFW_KEY_KP_DIVIDE:			return KeyCode::numpad_divide;
	case GLFW_KEY_KP_MULTIPLY:			return KeyCode::numpad_multiply;
	case GLFW_KEY_KP_SUBTRACT:			return KeyCode::numpad_subtract;
	case GLFW_KEY_KP_ADD:				return KeyCode::numpad_add;
	case GLFW_KEY_KP_ENTER:				return KeyCode::numpad_enter;
	case GLFW_KEY_KP_EQUAL:				return KeyCode::numpad_equal;
	case GLFW_KEY_LEFT_SHIFT:			return KeyCode::left_shift;
	case GLFW_KEY_LEFT_CONTROL:			return KeyCode::left_control;
	case GLFW_KEY_LEFT_ALT:				return KeyCode::left_alt;
	case GLFW_KEY_LEFT_SUPER:			return KeyCode::left_super;
	case GLFW_KEY_RIGHT_SHIFT:			return KeyCode::right_shift;
	case GLFW_KEY_RIGHT_CONTROL:		return KeyCode::right_control;
	case GLFW_KEY_RIGHT_ALT:			return KeyCode::right_alt;
	case GLFW_KEY_RIGHT_SUPER:			return KeyCode::right_super;
	default:
		return static_cast<KeyCode>(-1);
	}
}

inline KeyAction glfw_tl_key_action(signed const action) {
	switch (action) {
	case GLFW_PRESS: return KeyAction::press;
	case GLFW_RELEASE: return KeyAction::release;
	case GLFW_REPEAT: return KeyAction::repeat;
	default:
		TOGO_ASSERT(false, "unrecognized key action");
	}
}

inline KeyMod glfw_tl_key_mods(signed mods) {
	mods &= GLFW_MOD_ALT | GLFW_MOD_CONTROL | GLFW_MOD_SHIFT;
	switch (mods) {
	case GLFW_MOD_ALT: return KeyMod::alt;
	case GLFW_MOD_CONTROL: return KeyMod::ctrl;
	case GLFW_MOD_SHIFT: return KeyMod::shift;
	case GLFW_MOD_ALT | GLFW_MOD_CONTROL: return KeyMod::alt | KeyMod::ctrl;
	case GLFW_MOD_ALT | GLFW_MOD_SHIFT: return KeyMod::alt | KeyMod::shift;
	case GLFW_MOD_CONTROL | GLFW_MOD_SHIFT: return KeyMod::alt | KeyMod::ctrl;
	case GLFW_MOD_ALT | GLFW_MOD_CONTROL | GLFW_MOD_SHIFT: return KeyMod::alt | KeyMod::ctrl | KeyMod::shift;
	default:
		return KeyMod::none;
	}
}

inline MouseButtonAction glfw_tl_mouse_action(signed const action) {
	switch (action) {
	case GLFW_PRESS: return MouseButtonAction::press;
	case GLFW_RELEASE: return MouseButtonAction::release;
	default:
		TOGO_ASSERT(false, "unrecognized mouse button action");
	}
}

inline MouseButton glfw_tl_mouse_button(signed const button) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::left;
	case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::middle;
	case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::right;
	default:
		return static_cast<MouseButton>(-1);
	}
}

void glfw_window_close_cb(GLFWwindow* const handle) {
	auto const window = static_cast<Window*>(
		glfwGetWindowUserPointer(handle)
	);
	object_buffer::write(
		window->_input_buffer->_buffer,
		InputEventType::window_close_request,
		WindowCloseRequestEvent{window}
	);
}

void glfw_window_focus_cb(
	GLFWwindow* const handle,
	signed const focused
) {
	auto const window = static_cast<Window*>(
		glfwGetWindowUserPointer(handle)
	);
	object_buffer::write(
		window->_input_buffer->_buffer,
		InputEventType::window_focus,
		WindowFocusEvent{window, focused == GL_TRUE}
	);
}

void glfw_window_size_cb(
	GLFWwindow* const handle,
	signed const width,
	signed const height
) {
	auto const window = static_cast<Window*>(
		glfwGetWindowUserPointer(handle)
	);
	if (
		window->_size.x != unsigned_cast(width) ||
		window->_size.y != unsigned_cast(height)
	) {
		auto const old_size = window->_size;
		window->_size.x = unsigned_cast(width);
		window->_size.y = unsigned_cast(height);
		object_buffer::write(
			window->_input_buffer->_buffer,
			InputEventType::window_resize,
			WindowResizeEvent{window, old_size, window->_size}
		);
	}
}

void glfw_key_cb(
	GLFWwindow* const handle,
	signed const key,
	signed const /*scancode*/,
	signed const action,
	signed const mods
) {
	auto const window = static_cast<Window*>(
		glfwGetWindowUserPointer(handle)
	);
	KeyCode const key_code = glfw_tl_key_code(key);
	if (key_code != static_cast<KeyCode>(-1)) {
		object_buffer::write(
			window->_input_buffer->_buffer,
			InputEventType::key,
			KeyEvent{
				window,
				glfw_tl_key_action(action),
				key_code,
				glfw_tl_key_mods(mods)
			}
		);
	}
}

void glfw_mouse_button_cb(
	GLFWwindow* const handle,
	signed const glfw_button,
	signed const action,
	signed const /*mods*/
) {
	auto const window = static_cast<Window*>(
		glfwGetWindowUserPointer(handle)
	);
	MouseButton const button = glfw_tl_mouse_button(glfw_button);
	if (button != static_cast<MouseButton>(-1)) {
		object_buffer::write(
			window->_input_buffer->_buffer,
			InputEventType::mouse_button,
			MouseButtonEvent{
				window,
				glfw_tl_mouse_action(action),
				button
			}
		);
	}
}

void glfw_cursor_pos_cb(
	GLFWwindow* const handle,
	double const xpos,
	double const ypos
) {
	auto const window = static_cast<Window*>(
		glfwGetWindowUserPointer(handle)
	);
	object_buffer::write(
		window->_input_buffer->_buffer,
		InputEventType::mouse_motion,
		MouseMotionEvent{
			window,
			static_cast<signed>(std::floor(xpos)),
			static_cast<signed>(std::floor(ypos))
		}
	);
}

void window::attach_to_input_buffer_impl(Window* window) {
	glfwSetWindowCloseCallback(window->_impl.handle, glfw_window_close_cb);
	glfwSetWindowFocusCallback(window->_impl.handle, glfw_window_focus_cb);
	glfwSetWindowSizeCallback(window->_impl.handle, glfw_window_size_cb);
	glfwSetKeyCallback(window->_impl.handle, glfw_key_cb);
	glfwSetMouseButtonCallback(window->_impl.handle, glfw_mouse_button_cb);
	glfwSetCursorPosCallback(window->_impl.handle, glfw_cursor_pos_cb);
}

void window::detach_from_input_buffer_impl(Window* window) {
	glfwSetWindowCloseCallback(window->_impl.handle, nullptr);
	glfwSetWindowFocusCallback(window->_impl.handle, nullptr);
	glfwSetWindowSizeCallback(window->_impl.handle, nullptr);
	glfwSetKeyCallback(window->_impl.handle, nullptr);
	glfwSetMouseButtonCallback(window->_impl.handle, nullptr);
	glfwSetCursorPosCallback(window->_impl.handle, nullptr);
}

void window::process_events(InputBuffer&) {
	glfwPollEvents();
}

} // namespace togo
