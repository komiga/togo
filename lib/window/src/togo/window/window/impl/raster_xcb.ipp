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
#include <togo/image/pixmap/pixmap.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/window/impl/private.hpp>
#include <togo/window/window/impl/raster_xcb.hpp>
#include <togo/window/input/types.hpp>

#include <xcb/xcbext.h>
#include <xcb/xcb_icccm.h>

#include <cstdlib>

#undef TOGO_TEST_LOG_ENABLE
#if defined(TOGO_TEST_WINDOW)
	#define TOGO_TEST_LOG_ENABLE
#endif
#include <togo/core/log/test.hpp>

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

namespace window {
namespace {

static void create_xcb_gc(xcb_drawable_t drawable) {
	TOGO_DEBUG_ASSERTE(_xcb_globals.gc == XCB_NONE);
	xcb_void_cookie_t err_cookie;
	xcb_generic_error_t* err;

	constexpr u32 const attr_mask = 0
		| XCB_GC_FOREGROUND
		| XCB_GC_BACKGROUND
		| XCB_GC_GRAPHICS_EXPOSURES
	;
	static u32 const attrs[]{
		_xcb_globals.screen->white_pixel,
		_xcb_globals.screen->black_pixel,
		0,
	};
	_xcb_globals.gc = xcb_generate_id(_xcb_globals.c);
	err_cookie = xcb_create_gc_checked(
		_xcb_globals.c,
		_xcb_globals.gc,
		drawable,
		attr_mask, attrs
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
}

static void put_image_area(
	Window* window,
	unsigned dst_x, unsigned dst_y,
	unsigned src_x, unsigned src_y,
	unsigned width, unsigned height
) {
	auto& b = window->_impl.backbuffer;
	xcb_protocol_request_t proc_req{
		// 2 internal + put_image req + 1 per row + padding
		2 + 1 + height + 1, // count
		0, // ext
		XCB_PUT_IMAGE, // opcode
		1 // isvoid
	};

	xcb_put_image_request_t req{};
	// major_opcode
	req.format = XCB_IMAGE_FORMAT_Z_PIXMAP;
	// length
	req.drawable = window->_impl.id;
	req.gc = _xcb_globals.gc;
	req.width = width;
	req.height = height;
	req.dst_x = signed_cast(dst_x);
	req.dst_y = signed_cast(dst_y);
	req.left_pad = 0;
	req.depth = _xcb_globals.depth;
	// pad0

	if (
		!_xcb_globals.iovec_temporary ||
		_xcb_globals.iovec_temporary_size < proc_req.count
	) {
		memory::default_allocator().deallocate(_xcb_globals.iovec_temporary);
		_xcb_globals.iovec_temporary = TOGO_ALLOCATE_N(
			memory::default_allocator(), struct iovec, proc_req.count
		);
		_xcb_globals.iovec_temporary_size = proc_req.count;
	}
	auto* vec = _xcb_globals.iovec_temporary;

	// NB: xcb_put_image_request_t is padded in-structure
	vec[2].iov_base = reinterpret_cast<char*>(&req);
	vec[2].iov_len = sizeof(req);

	unsigned const pixel_size = pixel_format::pixel_size(b.format);
	unsigned bytes_to_next_row = pixel_size * b.size.width;
	u8* data = b.data + (pixel_size * (src_y * b.size.width + src_x));

	unsigned data_size = pixel_size * width;
	unsigned n = 3;
	for (auto rows = height; rows--;) {
		vec[n].iov_base = data;
		vec[n].iov_len = data_size;
		data += bytes_to_next_row;
		++n;
	}
	data_size *= height;

	// padding (XCB replaces this with pad data)
	vec[n].iov_base = nullptr;
	vec[n].iov_len = -data_size & 3;

	proc_req.count -= 2;
	xcb_send_request(_xcb_globals.c, 0, vec + 2, &proc_req);
}

} // anonymous namespace
} // namespace window

void window::init_impl() {
	xcb_generic_error_t* err;

	{// Connect
	_xcb_globals.c = xcb_connect(nullptr, nullptr);
	auto connect_err = xcb_connection_has_error(_xcb_globals.c);
	TOGO_ASSERTF(connect_err <= 0, "failed to connect to XCB with error: %d", connect_err);
	}

	{// Gather info from setup
	auto* setup = xcb_get_setup(_xcb_globals.c);
	// TOGO_LOG_DEBUGF("maximum_request_length = %u\n", setup->maximum_request_length);

	{// Get first screen
	auto iter = xcb_setup_roots_iterator(setup);
	TOGO_ASSERTE(iter.rem > 0);
	_xcb_globals.screen = iter.data;
	TOGO_ASSERTE(_xcb_globals.screen);
	}

	_xcb_globals.depth = 0;
	_xcb_globals.visual_id = ~0u;
	TOGO_ASSERT(
		setup->image_byte_order == XCB_IMAGE_ORDER_LSB_FIRST,
		"dunno what to do with non-little-endian host!"
	);

	for (auto i = xcb_setup_pixmap_formats_iterator(setup); i.rem; xcb_format_next(&i)) {
		auto* format = i.data;
		if (
			format->depth == 24 &&
			format->bits_per_pixel == 32 &&
			format->scanline_pad == 32
		) {
			_xcb_globals.depth = format->depth;
			break;
		}
	}
	TOGO_ASSERT(_xcb_globals.depth != 0, "could not find a suitable XCB pixmap format");

	auto& pf_info = pixel_format_info[unsigned_cast(XCBGlobals::pixel_format_id)];
	for (
		auto i = xcb_screen_allowed_depths_iterator(_xcb_globals.screen);
		i.rem && _xcb_globals.visual_id == ~0u; xcb_depth_next(&i)
	) {
		auto* depth = i.data;
		if (depth->depth != _xcb_globals.depth) {
			continue;
		}
	for (auto j = xcb_depth_visuals_iterator(depth); j.rem; xcb_visualtype_next(&j)) {
		auto* visual_type = j.data;
		if (
			(
				visual_type->visual_id == _xcb_globals.screen->root_visual ||
				_xcb_globals.visual_id == ~0u
			) &&
			visual_type->_class == XCB_VISUAL_CLASS_TRUE_COLOR &&
			visual_type->bits_per_rgb_value == 8 &&
			// TODO: Check for masks in BE order (also: Pixmap probably
			// won't support it until something like this this is found)
			visual_type->red_mask == pf_info.mask[0] &&
			visual_type->green_mask == pf_info.mask[1] &&
			visual_type->blue_mask == pf_info.mask[2]
		) {
			_xcb_globals.visual_id = visual_type->visual_id;
			if (visual_type->visual_id == _xcb_globals.screen->root_visual) {
				break;
			}
		}
	}}
	TOGO_ASSERT(_xcb_globals.visual_id != ~0u, "could not find a suitable XCB visual ID");
	TOGO_TEST_LOG_DEBUGF(
		"chosen visual: depth = %u, id = %u\n",
		_xcb_globals.depth, _xcb_globals.visual_id
	);

#if defined(TOGO_TEST_WINDOW)
	TOGO_TEST_LOG_DEBUGF(
		"XCB bits:"
		" image_byte_order = %u,"
		" root_depth = %u,"
		" root_visual = %u,"
		"\n"
		, setup->image_byte_order
		, _xcb_globals.screen->root_depth
		, _xcb_globals.screen->root_visual
	);

	TOGO_TEST_LOG_DEBUG("XCB pixel formats:\n");
	for (auto i = xcb_setup_pixmap_formats_iterator(setup); i.rem; xcb_format_next(&i)) {
		auto* format = i.data;
		TOGO_TEST_LOG_DEBUGF(
			"   "
			" depth = %u,"
			" bits_per_pixel = %u,"
			" scanline_pad = %u,"
			"\n"
			, format->depth
			, format->bits_per_pixel
			, format->scanline_pad
		);
	}

	static StringRef visual_class_name[]{
		"StaticGray",
		"GrayScale",
		"StaticColor",
		"PseudoColor",
		"TrueColor",
		"DirectColor",
	};

	TOGO_TEST_LOG_DEBUG("XCB depth + visual type info:\n");
	for (
		auto i = xcb_screen_allowed_depths_iterator(_xcb_globals.screen);
		i.rem; xcb_depth_next(&i)
	) {
		auto* depth = i.data;
		TOGO_TEST_LOG_DEBUGF(
			"'depth': %u, %u visuals:\n"
			, depth->depth
			, xcb_depth_visuals_length(depth)
		);
	for (auto j = xcb_depth_visuals_iterator(depth); j.rem; xcb_visualtype_next(&j)) {
		auto* visual_type = j.data;
		TOGO_DEBUG_ASSERTE(visual_type->_class >= 0 && visual_type->_class <= 5);
		auto class_name = visual_class_name[visual_type->_class];
		TOGO_TEST_LOGF(
			"   "
			" id = %u,"
			" class = %u = %.*s,"
			" bits_per_rgb_value = %u,"
			" colormap_entries = %u,"
			" red_mask = %08x,"
			" green_mask = %08x,"
			" blue_mask = %08x,"
			"\n"
			, visual_type->visual_id
			, visual_type->_class
			, class_name.size, class_name.data
			, visual_type->bits_per_rgb_value
			, visual_type->colormap_entries
			, visual_type->red_mask
			, visual_type->green_mask
			, visual_type->blue_mask
		);
	}}
#endif // defined(TOGO_TEST_WINDOW)
	}

	if (
		_xcb_globals.depth == _xcb_globals.screen->root_depth &&
		_xcb_globals.visual_id == _xcb_globals.screen->root_visual
	) {
		create_xcb_gc(_xcb_globals.screen->root);
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

	memory::default_allocator().deallocate(_xcb_globals.iovec_temporary);
	_xcb_globals.iovec_temporary = nullptr;
	_xcb_globals.iovec_temporary_size = 0;
}

Window* window::create_raster(
	StringRef title,
	UVec2 size,
	WindowFlags flags,
	Allocator& allocator
) {
	xcb_void_cookie_t err_cookie;
	xcb_generic_error_t* err;

	xcb_window_t window_id = xcb_generate_id(_xcb_globals.c);

	{// Create window
	constexpr u32 const attr_mask = 0
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
		_xcb_globals.depth,
		window_id,
		_xcb_globals.screen->root,
		0, 0,
		static_cast<u16>(size.width),
		static_cast<u16>(size.height),
		0, // border
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		_xcb_globals.visual_id,
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
		_xcb_globals.c, window_id,
		_xcb_globals.c_ewmh.WM_PROTOCOLS,
		array_extent(protocols), protocols
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	// _NET_WM_WINDOW_TYPE
	err_cookie = xcb_ewmh_set_wm_window_type_checked(
		&_xcb_globals.c_ewmh, window_id,
		1, &_xcb_globals.c_ewmh._NET_WM_WINDOW_TYPE_NORMAL
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));

	{// WM hints
	xcb_icccm_wm_hints_t wm_hints{};
	xcb_icccm_wm_hints_set_input(&wm_hints, 1);
	xcb_icccm_wm_hints_set_normal(&wm_hints);
	err_cookie = xcb_icccm_set_wm_hints_checked(_xcb_globals.c, window_id, &wm_hints);
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
	err_cookie = xcb_icccm_set_wm_normal_hints_checked(_xcb_globals.c, window_id, &size_hints);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	{// WM_CLASS
	// TODO: Proper instance and class names
	static char const wm_class[] = "togo_app\0togo_app";
	err_cookie = xcb_icccm_set_wm_class_checked(
		_xcb_globals.c, window_id,
		array_extent(wm_class), wm_class
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	{// WM_CLIENT_MACHINE
	auto hostname = system::hostname();
	err_cookie = xcb_icccm_set_wm_client_machine_checked(
		_xcb_globals.c, window_id,
		XCB_ATOM_STRING, 8,
		hostname.size, hostname.data
	);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	// _NET_WM_PID
	err_cookie = xcb_ewmh_set_wm_pid_checked(&_xcb_globals.c_ewmh, window_id, system::pid());
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));

	// Enable _NET_WM_BYPASS_COMPOSITOR (not in xcb_ewmh)
	if (_xcb_atom._NET_WM_BYPASS_COMPOSITOR != XCB_ATOM_NONE) {
		u32 value = 1;
		err_cookie = xcb_change_property_checked(
			_xcb_globals.c, XCB_PROP_MODE_REPLACE, window_id,
			_xcb_atom._NET_WM_BYPASS_COMPOSITOR,
			XCB_ATOM_CARDINAL, 32, 1, &value
		);
		TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	}

	// Map window
	err_cookie = xcb_map_window_checked(_xcb_globals.c, window_id);
	TOGO_ASSERTE(!(err = xcb_request_check(_xcb_globals.c, err_cookie)));
	TOGO_ASSERTE(xcb_flush(_xcb_globals.c) > 0);

	if (!_xcb_globals.gc) {
		create_xcb_gc(window_id);
	}

	Window* const window = TOGO_CONSTRUCT(
		allocator, Window, size, flags, {}, allocator,
		XCBWindowImpl{
			window_id,
			false, false,
			{size, XCBGlobals::pixel_format_id}
		}
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

Pixmap& window::backbuffer(Window* window) {
	return window->_impl.backbuffer;
}

void window::push_backbuffer(Window* window, ArrayRef<UVec4 const> areas) {
	auto const& wsize = window->_size;
	for (auto& area : areas) {
		if (
			area.x > wsize.width || area.y > wsize.height ||
			area.width == 0 || area.height == 0
		) {
			continue;
		} else if (
			area.x == 0 && area.y == 0 &&
			area.width == wsize.width && area.height == wsize.height
		) {
			xcb_put_image(
				_xcb_globals.c,
				XCB_IMAGE_FORMAT_Z_PIXMAP,
				window->_impl.id,
				_xcb_globals.gc,
				static_cast<u16>(area.width),
				static_cast<u16>(area.height),
				0, 0,
				0,
				_xcb_globals.depth,
				window->_impl.backbuffer.data_size,
				window->_impl.backbuffer.data
			);
		} else {
			put_image_area(
				window,
				area.x, area.y,
				area.x, area.y,
				min(area.width, wsize.width),
				min(area.height, wsize.height)
			);
		}
	}
	xcb_flush(_xcb_globals.c);
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
		} break;

void window::process_events(InputBuffer& ib) {
	xcb_void_cookie_t err_cookie;
	xcb_generic_error_t* err;
	xcb_generic_event_t* generic_event;
	unsigned event_type;
	bool any_resized = false;

	while ((generic_event = xcb_poll_for_event(_xcb_globals.c))) {
	// NB: MSB is whether the event came from another client
	event_type = generic_event->response_type & ~0x80;
	switch (event_type) {
	DO_EVENT(XCB_EXPOSE, xcb_expose_event_t, window)
		// TOGO_LOG_DEBUGF("EXPOSE. resized = %d\n", window->_impl.resized);
		if (!window->_impl.resized) {
			put_image_area(
				window,
				event->x, event->y,
				event->x, event->y,
				event->width, event->height
			);
		}
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
			any_resized = true;
			window->_impl.resized = true;
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

	if (any_resized) {
		for (auto window : ib._windows) {
			if (!window || !window->_impl.resized) {
				continue;
			}
			pixmap::resize(window->_impl.backbuffer, window->_size);
			put_image_area(
				window, 0, 0, 0, 0,
				window->_size.width, window->_size.height
			);
			window->_impl.resized = false;
		}
	}
}

#undef DO_EVENT
#undef END_EVENT

} // namespace togo

#include <togo/core/log/test_unconfigure.hpp>
