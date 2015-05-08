#line 2 "togo/window/window/impl/raster_xcb.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/string/string.hpp>

#include <xcb/xcb.h>

namespace togo {

struct XCBWindowImpl {
	xcb_window_t id;
	bool focus_last;
};

using WindowImpl = XCBWindowImpl;

namespace window {

struct XCBAtom {
	StringRef name;
	xcb_atom_t fallback;
	xcb_atom_t id;

	constexpr XCBAtom(StringRef name, xcb_atom_t fallback)
		: name(name)
		, fallback(fallback)
		, id(XCB_ATOM_NONE)
	{}
};


struct XCBGlobals {
	xcb_connection_t* c{nullptr};
	xcb_screen_t* screen{nullptr};

	struct {
		XCBAtom title{"_NET_WM_NAME", XCB_ATOM_WM_NAME};
	} atom;
};

extern XCBGlobals _xcb_globals;

} // namespace window

} // namespace togo
