#line 2 "togo/window/window/impl/raster_xcb.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/string/string.hpp>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

#include <cstddef>

namespace togo {

struct XCBWindowImpl {
	xcb_window_t id;
	bool focus_last;
};

using WindowImpl = XCBWindowImpl;

namespace window {

struct XCBGlobals {
	xcb_connection_t* c{nullptr};
	xcb_screen_t* screen{nullptr};
	xcb_ewmh_connection_t c_ewmh{};
	bool has_ewmh{false};
	u8 depth;
	xcb_visualid_t visual_id;
	xcb_gcontext_t gc{XCB_NONE};
};

struct XCBAtomCache {
	xcb_atom_t WM_DELETE_WINDOW;
	xcb_atom_t _NET_WM_BYPASS_COMPOSITOR;
};

extern XCBGlobals _xcb_globals;
extern XCBAtomCache _xcb_atom;

} // namespace window

} // namespace togo
