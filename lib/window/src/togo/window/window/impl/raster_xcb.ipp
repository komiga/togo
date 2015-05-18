#line 2 "togo/window/window/impl/raster_xcb.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/window/impl/private.hpp>
#include <togo/window/window/impl/raster_xcb.hpp>
#include <togo/window/input/types.hpp>

#include <xcb/xcb_icccm.h>

#include <cstdlib>

namespace togo {

window::XCBGlobals window::_xcb_globals{};
window::XCBAtomCache window::_xcb_atom{};

namespace {

struct XCBAtomLookup {
	StringRef name;
	unsigned offset;
	bool required;
};

#define ATOM(lookup_, required_) \
	{#lookup_, offsetof(window::XCBAtomCache, lookup_), required_}

static XCBAtomLookup const _xcb_atom_lookup[]{
	ATOM(WM_DELETE_WINDOW, true),
	ATOM(_NET_WM_BYPASS_COMPOSITOR, false),
};

#undef ATOM

} // anonymous namespace

void window::init_impl() {
	xcb_generic_error_t* err;

	{// Connect
	_xcb_globals.c = xcb_connect(nullptr, nullptr);
	int connect_err = xcb_connection_has_error(_xcb_globals.c);
	TOGO_ASSERTF(connect_err <= 0, "failed to connect to XCB with error: %d", connect_err);

	// Get first screen
	xcb_setup_t const* setup = xcb_get_setup(_xcb_globals.c);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
	_xcb_globals.screen = iter.data;
	TOGO_ASSERTE(_xcb_globals.screen);
	}

	{// Initialize EWMH
	auto ewmh_cookie = xcb_ewmh_init_atoms(_xcb_globals.c, &_xcb_globals.c_ewmh);
	TOGO_ASSERTE(xcb_ewmh_init_atoms_replies(&_xcb_globals.c_ewmh, ewmh_cookie, &err) == 1);
	}

	{// Cache atoms
	xcb_intern_atom_cookie_t cookie;
	xcb_intern_atom_reply_t* reply;

	for (auto& lookup : _xcb_atom_lookup) {
		cookie = xcb_intern_atom(
			_xcb_globals.c, 1,
			static_cast<u16>(lookup.name.size),
			lookup.name.data
		);
		reply = xcb_intern_atom_reply(_xcb_globals.c, cookie, &err);
		TOGO_ASSERTE(!err && reply);
		*reinterpret_cast<xcb_atom_t*>(pointer_add(&_xcb_atom, lookup.offset)) = reply->atom;
		TOGO_ASSERTF(
			!lookup.required || reply->atom != XCB_ATOM_NONE,
			"required atom does not exist: '%.*s'",
			lookup.name.size, lookup.name.data
		);
		std::free(reply);
	}}

	{// Check for EWMH support by the WM
	auto cookie = xcb_ewmh_get_supporting_wm_check(
		&_xcb_globals.c_ewmh, _xcb_globals.screen->root
	);
	xcb_window_t wm_window_from_root;
	TOGO_ASSERTE(xcb_ewmh_get_supporting_wm_check_reply(
		&_xcb_globals.c_ewmh, cookie, &wm_window_from_root, &err
	) == 1 && !err);

	if (wm_window_from_root != XCB_NONE) {
		xcb_get_property_cookie_t cookie = xcb_ewmh_get_supporting_wm_check(
			&_xcb_globals.c_ewmh, wm_window_from_root
		);
		xcb_window_t wm_window_from_child;
		TOGO_ASSERTE(xcb_ewmh_get_supporting_wm_check_reply(
			&_xcb_globals.c_ewmh, cookie, &wm_window_from_child, &err
		) == 1 && !err);
		if (wm_window_from_root != wm_window_from_child) {
			wm_window_from_root = XCB_NONE;
		}
	}
	TOGO_ASSERT(wm_window_from_root != XCB_NONE, "EWMH-supporting WM required");
	}
}

void window::shutdown_impl() {
	xcb_ewmh_connection_wipe(&_xcb_globals.c_ewmh);
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
	u32 attr_mask = 0
		| XCB_CW_BACK_PIXMAP
		| XCB_CW_BACK_PIXEL
		| XCB_CW_BACKING_STORE
		// | XCB_CW_OVERRIDE_REDIRECT
		| XCB_CW_SAVE_UNDER
		| XCB_CW_EVENT_MASK
	;
	static u32 const attrs[]{
		XCB_BACK_PIXMAP_NONE,
		_xcb_globals.screen->black_pixel,
		XCB_BACKING_STORE_NOT_USEFUL,
		// 0, // override-redirect
		0, // save-under
		0
			| XCB_EVENT_MASK_KEY_PRESS
			| XCB_EVENT_MASK_KEY_RELEASE
			| XCB_EVENT_MASK_BUTTON_PRESS
			| XCB_EVENT_MASK_BUTTON_RELEASE
			| XCB_EVENT_MASK_POINTER_MOTION
			| XCB_EVENT_MASK_EXPOSURE
			| XCB_EVENT_MASK_STRUCTURE_NOTIFY
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
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		// TODO: Make visual from window configuration? cf. xcb_visualtype_t
		_xcb_globals.screen->root_visual,
		attr_mask, attrs
	);
	// NB: We own result
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	// Set ICCCM and EWMH properties
	{// WM_PROTOCOLS
	xcb_atom_t protocols[]{
		_xcb_atom.WM_DELETE_WINDOW,
		_xcb_globals.c_ewmh._NET_WM_PING,
	};
	err_cookie = xcb_icccm_set_wm_protocols_checked(
		_xcb_globals.c, id,
		_xcb_globals.c_ewmh.WM_PROTOCOLS,
		array_extent(protocols), protocols
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	// _NET_WM_WINDOW_TYPE
	err_cookie = xcb_ewmh_set_wm_window_type_checked(
		&_xcb_globals.c_ewmh, id,
		1, &_xcb_globals.c_ewmh._NET_WM_WINDOW_TYPE_NORMAL
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));

	{// WM hints
	xcb_icccm_wm_hints_t wm_hints{};
	xcb_icccm_wm_hints_set_input(&wm_hints, 1);
	xcb_icccm_wm_hints_set_normal(&wm_hints);
	err_cookie = xcb_icccm_set_wm_hints_checked(_xcb_globals.c, id, &wm_hints);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	{// Size hints
	xcb_size_hints_t size_hints{};
	// xcb_icccm_size_hints_set_size(&size_hints, 0, size.width, size.height);
	if (enum_bool(flags & WindowFlags::resizable)) {
		// xcb_icccm_size_hints_set_min_size(&size_hints, 1, 1);
	} else {
		xcb_icccm_size_hints_set_min_size(&size_hints, size.width, size.height);
		xcb_icccm_size_hints_set_max_size(&size_hints, size.width, size.height);
	}
	err_cookie = xcb_icccm_set_wm_normal_hints_checked(_xcb_globals.c, id, &size_hints);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	{// WM_CLASS
	// TODO: Proper instance and class names
	static char const wm_class[] = "togo_app\0togo_app";
	err_cookie = xcb_icccm_set_wm_class_checked(
		_xcb_globals.c, id,
		array_extent(wm_class), wm_class
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	{// WM_CLIENT_MACHINE
	auto hostname = system::hostname();
	err_cookie = xcb_icccm_set_wm_client_machine_checked(
		_xcb_globals.c, id,
		XCB_ATOM_STRING, 8,
		hostname.size, hostname.data
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	// _NET_WM_PID
	err_cookie = xcb_ewmh_set_wm_pid_checked(&_xcb_globals.c_ewmh, id, system::pid());
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));

	// Enable _NET_WM_BYPASS_COMPOSITOR (not in xcb_ewmh)
	if (_xcb_atom._NET_WM_BYPASS_COMPOSITOR != XCB_ATOM_NONE) {
		u32 value = 1;
		err_cookie = xcb_change_property_checked(
			_xcb_globals.c, XCB_PROP_MODE_REPLACE, id,
			_xcb_atom._NET_WM_BYPASS_COMPOSITOR,
			XCB_ATOM_CARDINAL, 32, 1, &value
		);
		TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	// Map window
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

	err_cookie = xcb_icccm_set_wm_name_checked(
		_xcb_globals.c, window->_impl.id,
		XCB_ATOM_STRING,
		8, title.size, title.data
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));

	err_cookie = xcb_ewmh_set_wm_name_checked(
		&_xcb_globals.c_ewmh, window->_impl.id,
		title.size, title.data
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
	// TODO: use xcb_get_keyboard_mapping()? xcb_keycode_t is only a byte.
	// Should probably use xkb...
	// TOGO_LOG_DEBUGF("keycode: %u\n", static_cast<unsigned>(event->detail));
	return event->detail == 9 ? KeyCode::escape : KeyCode::space;
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
	case 2: return MouseButton::middle;
	case 3: return MouseButton::right;
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
	xcb_void_cookie_t err_cookie;
	xcb_generic_error_t* err;
	xcb_generic_event_t* generic_event;
	unsigned event_type;

	while ((generic_event = xcb_poll_for_event(_xcb_globals.c))) {
	// NB: MSB is whether the event came from another client
	event_type = generic_event->response_type & ~0x80;
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
		if (button == static_cast<MouseButton>(-1)) {
			break;
		}
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
		if (!window->_impl.focus_last) {
			err_cookie = xcb_set_input_focus_checked(
				_xcb_globals.c,
				XCB_INPUT_FOCUS_POINTER_ROOT,
				window->_impl.id,
				XCB_CURRENT_TIME // FIXME: Is this good?
			);
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
		bool focus = event_type == XCB_FOCUS_IN;
		if (focus != window->_impl.focus_last) {
			window->_impl.focus_last = focus;
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::window_focus,
				WindowFocusEvent{window, focus}
			);
		}
	END_EVENT()

	DO_EVENT(XCB_CONFIGURE_NOTIFY, xcb_configure_notify_event_t, window)
		if (
			event->width != window->_size.width ||
			event->height != window->_size.height
		) {
			UVec2 const new_size{event->width, event->height};
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::window_resize,
				WindowResizeEvent{window, window->_size, new_size}
			);
			window->_size = new_size;
		}
	END_EVENT()

	DO_EVENT(XCB_CLIENT_MESSAGE, xcb_client_message_event_t, window)
	if (event->type == _xcb_globals.c_ewmh.WM_PROTOCOLS) {
		xcb_atom_t message = event->data.data32[0];
		if (message == _xcb_atom.WM_DELETE_WINDOW) {
			// CHECK: Seems like a focused/not focused pair is being generated
			// by AwesomeWM around this message. Is this normal behavior?
			object_buffer::write(
				window->_input_buffer->_buffer,
				InputEventType::window_close_request,
				WindowCloseRequestEvent{window}
			);
		} else if (message == _xcb_globals.c_ewmh._NET_WM_PING) {
			TOGO_LOG_DEBUG("WM_PROTOCOLS: ping\n");
			event->window = _xcb_globals.screen->root;
			// FIXME: No checked variant; uses xcb_send_event()
			xcb_ewmh_send_client_message(
				_xcb_globals.c,
				_xcb_globals.screen->root,
				_xcb_globals.screen->root,
				_xcb_globals.c_ewmh.WM_PROTOCOLS,
				3 * sizeof(u32), event->data.data32
			);
		} else {
			TOGO_LOG_DEBUGF("unhandled WM_PROTOCOLS message: %u\n", message);
		}
	} else {
		TOGO_LOG_DEBUGF("unhandled client message type: %u\n", event->type);
	}
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

	// TODO: Resize pixmaps
}

#undef DO_EVENT
#undef END_EVENT

} // namespace togo
