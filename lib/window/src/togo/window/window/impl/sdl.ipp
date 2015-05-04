#line 2 "togo/window/window/impl/sdl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/window/impl/private.hpp>
#include <togo/window/window/impl/sdl.hpp>
#include <togo/window/input/types.hpp>

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>

namespace togo {

namespace {

enum {
	INIT_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_VIDEO,
};

} // anonymous namespace

void window::init_impl() {
	TOGO_SDL_CHECK(SDL_Init(INIT_SYSTEMS) != 0);

#if defined(TOGO_CONFIG_WINDOW_ENABLE_OPENGL)
	if (_globals.with_gl) {
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1));
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, _globals.context_major));
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, _globals.context_minor));
		if (_globals.context_major >= 3) {
			TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
			TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG));
		}
	}
#else
	TOGO_ASSERT(!_globals.with_gl, "OpenGL support disabled in lib/window");
#endif
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to initialize window backend: %s", SDL_GetError());
}

void window::shutdown_impl() {
	SDL_Quit();
}

static void set_creation_properties(
	WindowFlags const flags,
	signed& x, signed& y,
	unsigned& sdl_flags
) {
	if (enum_bool(flags & WindowFlags::centered)) {
		x = y = SDL_WINDOWPOS_CENTERED;
	} else {
		x = y = SDL_WINDOWPOS_UNDEFINED;
	}

	if (enum_bool(flags & WindowFlags::borderless)) {
		sdl_flags |= SDL_WINDOW_BORDERLESS;
	}
	if (enum_bool(flags & WindowFlags::fullscreen)) {
		sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (enum_bool(flags & WindowFlags::resizable)) {
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	}
}

Window* window::create(
	StringRef title,
	UVec2 size,
	WindowFlags flags,
	Allocator& allocator
) {
	signed x;
	signed y;
	unsigned sdl_flags
		= SDL_WINDOW_ALLOW_HIGHDPI
	;
	set_creation_properties(flags, x, y, sdl_flags);

	SDL_Window* handle = SDL_CreateWindow(title.data, x, y, size.x, size.y, sdl_flags);
	TOGO_SDL_CHECK(!handle);

	return TOGO_CONSTRUCT(
		allocator, Window, size, flags, {}, allocator,
		SDLWindowImpl{handle, nullptr}
	);

sdl_error:
	TOGO_ASSERTF(false, "failed to create window: %s", SDL_GetError());
}

#if defined(TOGO_CONFIG_WINDOW_ENABLE_OPENGL)
Window* window::create_opengl(
	StringRef title,
	UVec2 size,
	WindowFlags flags,
	WindowOpenGLConfig const& config,
	Window* share_with,
	Allocator& allocator
) {
	SDL_Window* handle = nullptr;
	SDL_GLContext context = nullptr;

	signed x;
	signed y;
	unsigned sdl_flags
		= SDL_WINDOW_ALLOW_HIGHDPI
		| SDL_WINDOW_OPENGL
	;
	set_creation_properties(flags, x, y, sdl_flags);

	if (share_with) {
		window::bind_context(share_with);
	}
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(
		SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
		share_with ? 1 : 0
	));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(
		SDL_GL_DOUBLEBUFFER,
		enum_bool(config.flags & WindowOpenGLConfig::Flags::double_buffered) ? 1 : 0
	));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_RED_SIZE, config.color_bits.red));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, config.color_bits.green));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, config.color_bits.blue));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, config.color_bits.alpha));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config.depth_bits));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, config.stencil_bits));

	if (config.msaa_num_buffers == 0 || config.msaa_num_samples == 0) {
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0));
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0));
	} else {
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, config.msaa_num_buffers));
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.msaa_num_samples));
	}

	handle = SDL_CreateWindow(title.data, x, y, size.x, size.y, sdl_flags);
	TOGO_SDL_CHECK(!handle);
	context = SDL_GL_CreateContext(handle);
	TOGO_SDL_CHECK(!context);
	TOGO_SDL_CHECK(SDL_GL_MakeCurrent(handle, context));
	glew_init();

	return TOGO_CONSTRUCT(
		allocator, Window, size, flags, config, allocator,
		SDLWindowImpl{handle, context}
	);

sdl_error:
	TOGO_ASSERTF(false, "failed to create window: %s", SDL_GetError());
}
#endif

void window::destroy(Window* window) {
	Allocator& allocator = *window->_allocator;
#if defined(TOGO_CONFIG_WINDOW_ENABLE_OPENGL)
	if (window->_impl.context) {
		SDL_GL_DeleteContext(window->_impl.context);
	}
#endif
	SDL_DestroyWindow(window->_impl.handle);
	TOGO_DESTROY(allocator, window);
}

void window::set_title(Window* window, StringRef title) {
	SDL_SetWindowTitle(window->_impl.handle, title.data);
}

void window::set_mouse_lock(Window* window, bool enable) {
	SDL_SetWindowGrab(window->_impl.handle, enable ? SDL_TRUE : SDL_FALSE);
}

void window::set_swap_mode(Window* window, WindowSwapMode mode) {
	signed interval;
	switch (mode) {
	case WindowSwapMode::immediate:		interval = 0; break;
	case WindowSwapMode::wait_refresh:	interval = 1; break;
	}
	window::bind_context(window);
	TOGO_SDL_CHECK(SDL_GL_SetSwapInterval(interval));
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to set window swap mode: %s", SDL_GetError());
}

#if defined(TOGO_CONFIG_WINDOW_ENABLE_OPENGL)
void window::bind_context(Window* window) {
	TOGO_ASSERTE(window->_impl.context);
	TOGO_SDL_CHECK(SDL_GL_MakeCurrent(window->_impl.handle, window->_impl.context));
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to bind window context: %s", SDL_GetError());
}

void window::unbind_context() {
	TOGO_SDL_CHECK(SDL_GL_MakeCurrent(nullptr, nullptr));
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to unbind window context: %s", SDL_GetError());
}

void window::swap_buffers(Window* window) {
	if (enum_bool(window->_config.flags & WindowOpenGLConfig::Flags::double_buffered)) {
		SDL_GL_SwapWindow(window->_impl.handle);
	}
}
#endif

// private

inline KeyCode sdl_tl_key_code(SDL_KeyboardEvent const& kev) {
	switch (kev.keysym.scancode) {
	case SDL_SCANCODE_A:				return KeyCode::a;
	case SDL_SCANCODE_B:				return KeyCode::b;
	case SDL_SCANCODE_C:				return KeyCode::c;
	case SDL_SCANCODE_D:				return KeyCode::d;
	case SDL_SCANCODE_E:				return KeyCode::e;
	case SDL_SCANCODE_F:				return KeyCode::f;
	case SDL_SCANCODE_G:				return KeyCode::g;
	case SDL_SCANCODE_H:				return KeyCode::h;
	case SDL_SCANCODE_I:				return KeyCode::i;
	case SDL_SCANCODE_J:				return KeyCode::j;
	case SDL_SCANCODE_K:				return KeyCode::k;
	case SDL_SCANCODE_L:				return KeyCode::l;
	case SDL_SCANCODE_M:				return KeyCode::m;
	case SDL_SCANCODE_N:				return KeyCode::n;
	case SDL_SCANCODE_O:				return KeyCode::o;
	case SDL_SCANCODE_P:				return KeyCode::p;
	case SDL_SCANCODE_Q:				return KeyCode::q;
	case SDL_SCANCODE_R:				return KeyCode::r;
	case SDL_SCANCODE_S:				return KeyCode::s;
	case SDL_SCANCODE_T:				return KeyCode::t;
	case SDL_SCANCODE_U:				return KeyCode::u;
	case SDL_SCANCODE_V:				return KeyCode::v;
	case SDL_SCANCODE_W:				return KeyCode::w;
	case SDL_SCANCODE_X:				return KeyCode::x;
	case SDL_SCANCODE_Y:				return KeyCode::y;
	case SDL_SCANCODE_Z:				return KeyCode::z;
	case SDL_SCANCODE_1:				return KeyCode::n1;
	case SDL_SCANCODE_2:				return KeyCode::n2;
	case SDL_SCANCODE_3:				return KeyCode::n3;
	case SDL_SCANCODE_4:				return KeyCode::n4;
	case SDL_SCANCODE_5:				return KeyCode::n5;
	case SDL_SCANCODE_6:				return KeyCode::n6;
	case SDL_SCANCODE_7:				return KeyCode::n7;
	case SDL_SCANCODE_8:				return KeyCode::n8;
	case SDL_SCANCODE_9:				return KeyCode::n9;
	case SDL_SCANCODE_0:				return KeyCode::n0;
	case SDL_SCANCODE_RETURN:			return KeyCode::enter;
	case SDL_SCANCODE_ESCAPE:			return KeyCode::escape;
	case SDL_SCANCODE_BACKSPACE:		return KeyCode::backspace;
	case SDL_SCANCODE_TAB:				return KeyCode::tab;
	case SDL_SCANCODE_SPACE:			return KeyCode::space;
	case SDL_SCANCODE_MINUS:			return KeyCode::minus;
	case SDL_SCANCODE_EQUALS:			return KeyCode::equal;
	case SDL_SCANCODE_LEFTBRACKET:		return KeyCode::left_bracket;
	case SDL_SCANCODE_RIGHTBRACKET:		return KeyCode::right_bracket;
	case SDL_SCANCODE_BACKSLASH:		return KeyCode::backslash;
	case SDL_SCANCODE_SEMICOLON:		return KeyCode::semicolon;
	case SDL_SCANCODE_APOSTROPHE:		return KeyCode::apostrophe;
	case SDL_SCANCODE_GRAVE:			return KeyCode::grave;
	case SDL_SCANCODE_COMMA:			return KeyCode::comma;
	case SDL_SCANCODE_PERIOD:			return KeyCode::period;
	case SDL_SCANCODE_SLASH:			return KeyCode::slash;
	case SDL_SCANCODE_CAPSLOCK:			return KeyCode::caps_lock;
	case SDL_SCANCODE_F1:				return KeyCode::f1;
	case SDL_SCANCODE_F2:				return KeyCode::f2;
	case SDL_SCANCODE_F3:				return KeyCode::f3;
	case SDL_SCANCODE_F4:				return KeyCode::f4;
	case SDL_SCANCODE_F5:				return KeyCode::f5;
	case SDL_SCANCODE_F6:				return KeyCode::f6;
	case SDL_SCANCODE_F7:				return KeyCode::f7;
	case SDL_SCANCODE_F8:				return KeyCode::f8;
	case SDL_SCANCODE_F9:				return KeyCode::f9;
	case SDL_SCANCODE_F10:				return KeyCode::f10;
	case SDL_SCANCODE_F11:				return KeyCode::f11;
	case SDL_SCANCODE_F12:				return KeyCode::f12;
	case SDL_SCANCODE_PRINTSCREEN:		return KeyCode::print_screen;
	case SDL_SCANCODE_SCROLLLOCK:		return KeyCode::scroll_lock;
	case SDL_SCANCODE_PAUSE:			return KeyCode::pause;
	case SDL_SCANCODE_INSERT:			return KeyCode::insert;
	case SDL_SCANCODE_HOME:				return KeyCode::home;
	case SDL_SCANCODE_PAGEUP:			return KeyCode::page_up;
	case SDL_SCANCODE_DELETE:			return KeyCode::del;
	case SDL_SCANCODE_END:				return KeyCode::end;
	case SDL_SCANCODE_PAGEDOWN:			return KeyCode::page_down;
	case SDL_SCANCODE_RIGHT:			return KeyCode::right;
	case SDL_SCANCODE_LEFT:				return KeyCode::left;
	case SDL_SCANCODE_DOWN:				return KeyCode::down;
	case SDL_SCANCODE_UP:				return KeyCode::up;
	case SDL_SCANCODE_NUMLOCKCLEAR:		return KeyCode::num_lock;
	case SDL_SCANCODE_KP_DIVIDE:		return KeyCode::numpad_divide;
	case SDL_SCANCODE_KP_MULTIPLY:		return KeyCode::numpad_multiply;
	case SDL_SCANCODE_KP_MINUS:			return KeyCode::numpad_subtract;
	case SDL_SCANCODE_KP_PLUS:			return KeyCode::numpad_add;
	case SDL_SCANCODE_KP_ENTER:			return KeyCode::numpad_enter;
	case SDL_SCANCODE_KP_1:				return KeyCode::numpad_1;
	case SDL_SCANCODE_KP_2:				return KeyCode::numpad_2;
	case SDL_SCANCODE_KP_3:				return KeyCode::numpad_3;
	case SDL_SCANCODE_KP_4:				return KeyCode::numpad_4;
	case SDL_SCANCODE_KP_5:				return KeyCode::numpad_5;
	case SDL_SCANCODE_KP_6:				return KeyCode::numpad_6;
	case SDL_SCANCODE_KP_7:				return KeyCode::numpad_7;
	case SDL_SCANCODE_KP_8:				return KeyCode::numpad_8;
	case SDL_SCANCODE_KP_9:				return KeyCode::numpad_9;
	case SDL_SCANCODE_KP_0:				return KeyCode::numpad_0;
	case SDL_SCANCODE_KP_PERIOD:		return KeyCode::numpad_decimal;
	case SDL_SCANCODE_KP_EQUALS:		return KeyCode::numpad_equal;
	case SDL_SCANCODE_LCTRL:			return KeyCode::left_control;
	case SDL_SCANCODE_LSHIFT:			return KeyCode::left_shift;
	case SDL_SCANCODE_LALT:				return KeyCode::left_alt;
	case SDL_SCANCODE_LGUI:				return KeyCode::left_super;
	case SDL_SCANCODE_RCTRL:			return KeyCode::right_control;
	case SDL_SCANCODE_RSHIFT:			return KeyCode::right_shift;
	case SDL_SCANCODE_RALT:				return KeyCode::right_alt;
	case SDL_SCANCODE_RGUI:				return KeyCode::right_super;
	default:
		return static_cast<KeyCode>(-1);
	}
}

inline KeyAction sdl_tl_key_action(SDL_KeyboardEvent const& kev) {
	if (kev.repeat) {
		return KeyAction::repeat;
	}
	switch (kev.state) {
	case SDL_PRESSED: return KeyAction::press;
	case SDL_RELEASED: return KeyAction::release;
	default:
		TOGO_ASSERT(false, "unrecognized key action");
	}
}

inline KeyMod sdl_tl_key_mods(SDL_KeyboardEvent const& kev) {
	unsigned const sdl_mods = kev.keysym.mod;
	KeyMod mods = KeyMod::none;
	if (sdl_mods & KMOD_ALT) {
		mods |= KeyMod::alt;
	}
	if (sdl_mods & KMOD_CTRL) {
		mods |= KeyMod::ctrl;
	}
	if (sdl_mods & KMOD_SHIFT) {
		mods |= KeyMod::shift;
	}
	return mods;
}

inline MouseButtonAction sdl_tl_mouse_action(SDL_MouseButtonEvent const& mbev) {
	switch (mbev.state) {
	case SDL_PRESSED: return MouseButtonAction::press;
	case SDL_RELEASED: return MouseButtonAction::release;
	default:
		TOGO_ASSERT(false, "unrecognized mouse button action");
	}
}

inline MouseButton sdl_tl_mouse_button(SDL_MouseButtonEvent const& mbev) {
	switch (mbev.button) {
	case SDL_BUTTON_LEFT: return MouseButton::left;
	case SDL_BUTTON_RIGHT: return MouseButton::right;
	case SDL_BUTTON_MIDDLE: return MouseButton::middle;
	default:
		return static_cast<MouseButton>(-1);
	}
}

void window::attach_to_input_buffer_impl(Window* /*window*/) {}
void window::detach_from_input_buffer_impl(Window* /*window*/) {}

Window* sdl_ib_find_window_by_id(
	InputBuffer& ib,
	unsigned const id,
	bool const allow_fallback
) {
	for (auto window : ib._windows) {
		if (
			window != nullptr && (
				(allow_fallback && id == 0) ||
				id == SDL_GetWindowID(window->_impl.handle)
			)
		) {
			return window;
		}
	}
	return nullptr;
}

void window::process_events(InputBuffer& ib) {
	Window* window = nullptr;
	KeyCode key_code{};
	MouseButton button{};
	SDL_Event event{};
	while (SDL_PollEvent(&event)) {
	switch (event.type) {
	case SDL_KEYDOWN: // fall-through
	case SDL_KEYUP:
		// If there are no windows attached to the IB, we can't
		// report the event
		window = sdl_ib_find_window_by_id(ib, event.key.windowID, true);
		if (!window) {
			break;
		}
		key_code = sdl_tl_key_code(event.key);
		if (key_code != static_cast<KeyCode>(-1)) {
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::key,
				KeyEvent{
					window,
					sdl_tl_key_action(event.key),
					key_code,
					sdl_tl_key_mods(event.key)
				}
			);
		}
		break;

	case SDL_MOUSEBUTTONDOWN: // fall-through
	case SDL_MOUSEBUTTONUP:
		window = sdl_ib_find_window_by_id(ib, event.button.windowID, true);
		if (!window) {
			break;
		}
		button = sdl_tl_mouse_button(event.button);
		if (button != static_cast<MouseButton>(-1)) {
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::mouse_button,
				MouseButtonEvent{
					window,
					sdl_tl_mouse_action(event.button),
					button
				}
			);
		}
		break;

	case SDL_MOUSEMOTION:
		window = sdl_ib_find_window_by_id(ib, event.motion.windowID, false);
		if (!window) {
			break;
		}
		object_buffer::write(
			window->_input_buffer->_buffer,
			InputEventType::mouse_motion,
			MouseMotionEvent{
				window,
				event.motion.x,
				event.motion.y
			}
		);
		break;

	case SDL_WINDOWEVENT:
		window = sdl_ib_find_window_by_id(ib, event.window.windowID, false);
		if (!window) {
			break;
		}
		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			if (
				window->_size.x != unsigned_cast(event.window.data1) ||
				window->_size.y != unsigned_cast(event.window.data2)
			) {
				auto const old_size = window->_size;
				window->_size.x = unsigned_cast(event.window.data1);
				window->_size.y = unsigned_cast(event.window.data2);
				object_buffer::write(
					window->_input_buffer->_buffer,
					InputEventType::window_resize,
					WindowResizeEvent{window, old_size, window->_size}
				);
			}
			break;
		case SDL_WINDOWEVENT_CLOSE:
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::window_close_request,
				WindowCloseRequestEvent{window}
			);
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::window_focus,
				WindowFocusEvent{window, true}
			);
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::window_focus,
				WindowFocusEvent{window, false}
			);
			break;
		}
		break;
	}
	} // while
}

} // namespace togo
