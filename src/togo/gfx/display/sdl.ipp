#line 2 "togo/gfx/display/sdl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/memory/memory.hpp>
#include <togo/io/object_buffer.hpp>
#include <togo/gfx/gfx/sdl_common.hpp>
#include <togo/gfx/display.hpp>
#include <togo/gfx/display/types.hpp>
#include <togo/gfx/display/private.hpp>
#include <togo/gfx/display/sdl.hpp>
#include <togo/input/types.hpp>

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>

namespace togo {
namespace gfx {

gfx::Display* display::create(
	char const* title,
	unsigned width,
	unsigned height,
	gfx::DisplayFlags flags,
	gfx::DisplayConfig const& config,
	gfx::Display* share_with,
	Allocator& allocator
) {
	SDL_Window* handle = nullptr;
	SDL_GLContext context = nullptr;
	int x;
	int y;
	if (enum_bool(flags & gfx::DisplayFlags::centered)) {
		x = y = SDL_WINDOWPOS_CENTERED;
	} else {
		x = y = SDL_WINDOWPOS_UNDEFINED;
	}

	unsigned sdl_flags
		= SDL_WINDOW_ALLOW_HIGHDPI
		| SDL_WINDOW_OPENGL
	;
	if (enum_bool(flags & gfx::DisplayFlags::borderless)) {
		sdl_flags |= SDL_WINDOW_BORDERLESS;
	}
	if (enum_bool(flags & gfx::DisplayFlags::fullscreen)) {
		sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (enum_bool(flags & gfx::DisplayFlags::resizable)) {
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	}

	if (share_with) {
		gfx::display::make_current(share_with);
	}
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(
		SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
		share_with ? 1 : 0
	));
	sdl_config_setup(config);
	handle = SDL_CreateWindow(title, x, y, width, height, sdl_flags);
	TOGO_SDL_CHECK(!handle);
	context = SDL_GL_CreateContext(handle);
	TOGO_SDL_CHECK(!context);
	TOGO_SDL_CHECK(SDL_GL_MakeCurrent(handle, context));
	gfx::glew_init();

	return TOGO_CONSTRUCT(
		allocator, gfx::Display, width, height, flags, config, allocator,
		SDLDisplayImpl{handle, context}
	);

sdl_error:
	TOGO_ASSERTF(false, "failed to create display: %s", SDL_GetError());
}

void display::set_title(gfx::Display* display, char const* title) {
	SDL_SetWindowTitle(display->_impl.handle, title);
}

void display::set_mouse_lock(gfx::Display* display, bool enable) {
	SDL_SetWindowGrab(display->_impl.handle, enable ? SDL_TRUE : SDL_FALSE);
}

void display::set_swap_mode(gfx::Display* display, gfx::DisplaySwapMode mode) {
	signed interval;
	switch (mode) {
	case gfx::DisplaySwapMode::immediate:		interval = 0; break;
	case gfx::DisplaySwapMode::wait_refresh:	interval = 1; break;
	}
	display::make_current(display);
	TOGO_SDL_CHECK(SDL_GL_SetSwapInterval(interval));
}

void display::make_current(gfx::Display* display) {
	TOGO_SDL_CHECK(SDL_GL_MakeCurrent(display->_impl.handle, display->_impl.context));
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to make window current: %s", SDL_GetError());
}

void display::swap_buffers(gfx::Display* display) {
	if (enum_bool(display->_config.flags & gfx::DisplayConfigFlags::double_buffered)) {
		SDL_GL_SwapWindow(display->_impl.handle);
	}
}

void display::destroy(gfx::Display* display) {
	Allocator& allocator = *display->_allocator;
	SDL_GL_DeleteContext(display->_impl.context);
	SDL_DestroyWindow(display->_impl.handle);
	TOGO_DESTROY(allocator, display);
}

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

void display::attach_to_input_buffer_impl(gfx::Display* /*display*/) {}
void display::detach_from_input_buffer_impl(gfx::Display* /*display*/) {}

gfx::Display* sdl_ib_find_display_by_id(
	InputBuffer& ib,
	unsigned const id,
	bool const allow_fallback
) {
	for (auto display : ib._displays) {
		if (
			display != nullptr && (
				(allow_fallback && id == 0) ||
				id == SDL_GetWindowID(display->_impl.handle)
			)
		) {
			return display;
		}
	}
	return nullptr;
}

void display::process_events(InputBuffer& ib) {
	gfx::Display* display = nullptr;
	KeyCode key_code{};
	MouseButton button{};
	SDL_Event event{};
	while (SDL_PollEvent(&event)) {
	switch (event.type) {
	case SDL_KEYDOWN: // fall-through
	case SDL_KEYUP:
		// If there are no displays attached to the IB, we can't
		// report the event
		display = sdl_ib_find_display_by_id(ib, event.key.windowID, true);
		if (!display) {
			break;
		}
		key_code = sdl_tl_key_code(event.key);
		if (key_code != static_cast<KeyCode>(-1)) {
			object_buffer::write(
				display->_input_buffer->_buffer,
				InputEventType::key,
				KeyEvent{
					display,
					sdl_tl_key_action(event.key),
					key_code,
					sdl_tl_key_mods(event.key)
				}
			);
		}
		break;

	case SDL_MOUSEBUTTONDOWN: // fall-through
	case SDL_MOUSEBUTTONUP:
		display = sdl_ib_find_display_by_id(ib, event.button.windowID, true);
		if (!display) {
			break;
		}
		button = sdl_tl_mouse_button(event.button);
		if (button != static_cast<MouseButton>(-1)) {
			object_buffer::write(
				display->_input_buffer->_buffer,
				InputEventType::mouse_button,
				MouseButtonEvent{
					display,
					sdl_tl_mouse_action(event.button),
					button
				}
			);
		}
		break;

	case SDL_MOUSEMOTION:
		display = sdl_ib_find_display_by_id(ib, event.motion.windowID, false);
		if (!display) {
			break;
		}
		object_buffer::write(
			display->_input_buffer->_buffer,
			InputEventType::mouse_motion,
			MouseMotionEvent{
				display,
				event.motion.x,
				event.motion.y
			}
		);
		break;

	case SDL_WINDOWEVENT:
		display = sdl_ib_find_display_by_id(ib, event.window.windowID, false);
		if (!display) {
			break;
		}
		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			if (
				display->_width != static_cast<unsigned>(event.window.data1) ||
				display->_height != static_cast<unsigned>(event.window.data2)
			) {
				unsigned const old_width = display->_width;
				unsigned const old_height = display->_height;
				display->_width = static_cast<unsigned>(event.window.data1);
				display->_height = static_cast<unsigned>(event.window.data2);
				object_buffer::write(
					display->_input_buffer->_buffer,
					InputEventType::display_resize,
					DisplayResizeEvent{
						display,
						old_width, old_height,
						display->_width, display->_height
					}
				);
			}
			break;
		case SDL_WINDOWEVENT_CLOSE:
			object_buffer::write(
				display->_input_buffer->_buffer,
				InputEventType::display_close_request,
				DisplayCloseRequestEvent{display}
			);
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			object_buffer::write(
				display->_input_buffer->_buffer,
				InputEventType::display_focus,
				DisplayFocusEvent{display, true}
			);
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			object_buffer::write(
				display->_input_buffer->_buffer,
				InputEventType::display_focus,
				DisplayFocusEvent{display, false}
			);
			break;
		}
		break;
	}
	} // while
}

} // namespace gfx
} // namespace togo
