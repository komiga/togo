#line 2 "togo/window/window/impl/raster_xcb.ipp"
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
#include <togo/window/window/impl/raster_xcb.hpp>
#include <togo/window/input/types.hpp>

#include <cstdlib>

namespace togo {

window::XCBGlobals window::_xcb_globals{};

void window::init_impl() {
	_xcb_globals.c = xcb_connect(nullptr, nullptr);
	int err = xcb_connection_has_error(_xcb_globals.c);
	TOGO_ASSERTF(err <= 0, "failed to connect to XCB with error: %d", err);

	// Get first screen
	xcb_setup_t const* setup = xcb_get_setup(_xcb_globals.c);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
	_xcb_globals.screen = iter.data;
	TOGO_ASSERTE(_xcb_globals.screen);

	{// Get atoms
	xcb_intern_atom_cookie_t cookie;
	xcb_intern_atom_reply_t* reply;
	xcb_generic_error_t* err;

	unsigned num_atoms = sizeof(_xcb_globals.atom) / sizeof(XCBAtom);
	TOGO_ASSERTE(num_atoms == 1);
	for (
		auto* atom = reinterpret_cast<XCBAtom*>(&_xcb_globals.atom);
		num_atoms;
		++atom, --num_atoms
	) {
		atom->id = atom->fallback;
		cookie = xcb_intern_atom(
			_xcb_globals.c, 1, static_cast<u16>(atom->name.size), atom->name.data
		);
		reply = xcb_intern_atom_reply(_xcb_globals.c, cookie, &err);
		if (err) {
			TOGO_LOGF(
				"failed to fetch atom: %.*s\n",
				atom->name.size, atom->name.data
			);
			std::free(err);
			err = nullptr;
		} else {
			TOGO_DEBUG_ASSERTE(reply);
			atom->id = reply->atom;
		}
		std::free(reply);
	}}
}

void window::shutdown_impl() {
	xcb_disconnect(_xcb_globals.c);
	_xcb_globals.c = nullptr;
}

Window* window::create_raster(
	StringRef title,
	UVec2 size,
	WindowFlags flags,
	Allocator& allocator
) {
	xcb_void_cookie_t err_cookie;
	xcb_generic_error_t* err;

	xcb_window_t id = xcb_generate_id(_xcb_globals.c);

	{// Create window
	u32 attr_mask
		= XCB_CW_BACK_PIXMAP
		| XCB_CW_BACKING_STORE
		| XCB_CW_EVENT_MASK
	;
	static u32 const attrs[]{
		XCB_BACK_PIXMAP_PARENT_RELATIVE,
		XCB_BACKING_STORE_NOT_USEFUL,
		0
			| XCB_EVENT_MASK_KEY_PRESS
			| XCB_EVENT_MASK_KEY_RELEASE
			| XCB_EVENT_MASK_BUTTON_PRESS
			| XCB_EVENT_MASK_BUTTON_RELEASE
			| XCB_EVENT_MASK_POINTER_MOTION
			| XCB_EVENT_MASK_EXPOSURE
			| XCB_EVENT_MASK_RESIZE_REDIRECT
			| XCB_EVENT_MASK_FOCUS_CHANGE
	};
	err_cookie = xcb_create_window_checked(
		_xcb_globals.c,
		XCB_COPY_FROM_PARENT,
		id,
		_xcb_globals.screen->root,
		0, 0,
		static_cast<u16>(size.width),
		static_cast<u16>(size.height),
		0, // border
		// TODO: What does this class mean?
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		// TODO: Make visual from window configuration? cf. xcb_visualtype_t
		_xcb_globals.screen->root_visual,
		attr_mask, attrs
	);
	// NB: We own result
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	err_cookie = xcb_map_window_checked(_xcb_globals.c, id);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	TOGO_ASSERTE(xcb_flush(_xcb_globals.c) > 0);

	Window* const window = TOGO_CONSTRUCT(
		allocator, Window, size, flags, {}, allocator,
		XCBWindowImpl{id, false}
	);
	window::set_title(window, title);
	return window;
}

void window::destroy(Window* window) {
	xcb_void_cookie_t err_cookie;
	xcb_generic_error_t* err;

	err_cookie = xcb_destroy_window_checked(_xcb_globals.c, window->_impl.id);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	TOGO_ASSERTE(xcb_flush(_xcb_globals.c) > 0);

	Allocator& allocator = *window->_allocator;
	TOGO_DESTROY(allocator, window);
}

void window::set_title(Window* window, StringRef title) {
	xcb_void_cookie_t err_cookie;
	xcb_generic_error_t* err;

	err_cookie = xcb_change_property_checked(
		_xcb_globals.c,
		XCB_PROP_MODE_REPLACE,
		window->_impl.id,
		_xcb_globals.atom._NET_WM_NAME.id,
		XCB_ATOM_STRING,
		8,
		title.size,
		title.data
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
}

void window::set_mouse_lock(Window* /*window*/, bool /*enable*/) {
	// TODO
}

// private

static Window* ib_find_window_by_id(
	InputBuffer& ib,
	xcb_window_t const id
) {
	for (auto window : ib._windows) {
		if (window != nullptr && id == window->_impl.id) {
			return window;
		}
	}
	return nullptr;
}

inline static KeyCode tl_xcb_key_code(xcb_key_press_event_t* event) {
	// TODO: use xcb_get_keyboard_mapping()? xcb_keycode_t is only a byte
	TOGO_LOG_DEBUGF("keycode: %u\n", static_cast<unsigned>(event->detail));
	return KeyCode::escape;
}

inline static KeyAction tl_xcb_key_action(xcb_key_press_event_t* event) {
	// TODO: Key repeat action?
	switch (event->response_type & ~0x80) {
	case XCB_KEY_PRESS: return KeyAction::press;
	case XCB_KEY_RELEASE: return KeyAction::release;
	default:
		TOGO_ASSERTE(false);
	}
}

inline static KeyMod tl_xcb_key_mods(xcb_key_press_event_t* event) {
	auto m = static_cast<xcb_key_but_mask_t>(event->state);
	KeyMod mods = KeyMod::none;
	// FIXME: Dunno which ModX is alt
	if (m & XCB_KEY_BUT_MASK_MOD_1) {
		mods |= KeyMod::alt;
	}
	if (m & XCB_KEY_BUT_MASK_CONTROL) {
		mods |= KeyMod::ctrl;
	}
	if (m & XCB_KEY_BUT_MASK_SHIFT) {
		mods |= KeyMod::shift;
	}
	return mods;
}

inline static MouseButton tl_xcb_mouse_button(xcb_button_t button) {
	// TODO: mouse wheel up and down are 4 and 5, respectively
	// (also, togo doesn't know about mouse wheel yet)
	switch (button) {
	case 1: return MouseButton::left;
	case 2: return MouseButton::right;
	case 3: return MouseButton::middle;
	default:
		return static_cast<MouseButton>(-1);
	}
}

void window::attach_to_input_buffer_impl(Window* /*window*/) {}
void window::detach_from_input_buffer_impl(Window* /*window*/) {}

#define DO_EVENT(opcode_, type_, id_name_) \
	case opcode_: { \
		auto event = reinterpret_cast<type_*>(generic_event); \
		auto window = ib_find_window_by_id(ib, event->id_name_);

#define END_EVENT() \
		break; }

void window::process_events(InputBuffer& ib) {
	xcb_generic_event_t* generic_event;
	while ((generic_event = xcb_poll_for_event(_xcb_globals.c))) {
	// NB: MSB is whether the event came from another client
	unsigned event_type = generic_event->response_type & ~0x80;
	switch (event_type) {
	DO_EVENT(XCB_EXPOSE, xcb_expose_event_t, window)
		// TODO: Re-render the exposed bits of the window pixmap
		(void)event;
		(void)window;
	END_EVENT()

	// NB: Same event type
	case XCB_KEY_PRESS: // fall-through
	DO_EVENT(XCB_KEY_RELEASE, xcb_key_press_event_t, event)
		auto key_code = tl_xcb_key_code(event);
		if (key_code != static_cast<KeyCode>(-1)) {
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::key,
				KeyEvent{
					window,
					tl_xcb_key_action(event),
					key_code,
					tl_xcb_key_mods(event)
				}
			);
		}
	END_EVENT()

	// NB: Same event type
	case XCB_BUTTON_PRESS: // fall-through
	DO_EVENT(XCB_BUTTON_RELEASE, xcb_button_press_event_t, event)
		auto button = tl_xcb_mouse_button(event->detail);
		if (button != static_cast<MouseButton>(-1)) {
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::mouse_button,
				MouseButtonEvent{
					window,
					event_type == XCB_BUTTON_PRESS
						? MouseButtonAction::press
						: MouseButtonAction::release
					,
					button
				}
			);
		}
		if (!window->_impl.focus_last) {
			xcb_void_cookie_t err_cookie = xcb_set_input_focus_checked(
				_xcb_globals.c,
				XCB_INPUT_FOCUS_POINTER_ROOT,
				window->_impl.id,
				XCB_CURRENT_TIME // FIXME: Is this good?
			);
			xcb_generic_error_t* err;
			TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
		}
	END_EVENT()

	DO_EVENT(XCB_MOTION_NOTIFY, xcb_motion_notify_event_t, event)
		object_buffer::write(
			window->_input_buffer->_buffer,
			InputEventType::mouse_motion,
			MouseMotionEvent{
				window,
				event->event_x,
				event->event_y
			}
		);
	END_EVENT()

	// NB: Same event type
	case XCB_FOCUS_IN: // fall-through
	DO_EVENT(XCB_FOCUS_OUT, xcb_focus_in_event_t, event)
		window->_impl.focus_last = event_type == XCB_FOCUS_IN;
		object_buffer::write(
			window->_input_buffer->_buffer,
			InputEventType::window_focus,
			WindowFocusEvent{window, window->_impl.focus_last}
		);
	END_EVENT()

	default:
		TOGO_LOG_DEBUGF(
			"unknown event: %u (client bit: %u)\n",
			event_type,
			static_cast<unsigned>(generic_event->response_type & 0x80)
		);
		break;
	}
	// XCB.. no.. :(
	std::free(generic_event);
	}
}

#undef DO_EVENT
#undef END_EVENT

} // namespace togo
