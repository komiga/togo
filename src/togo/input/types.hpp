#line 2 "togo/input/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Input types.
@ingroup types
@ingroup input
*/

#pragma once

#include <togo/config.hpp>
#include <togo/memory/types.hpp>
#include <togo/io/object_buffer_type.hpp>
#include <togo/gfx/types.hpp>

namespace togo {

/**
	@addtogroup input
	@{
*/

/// Input event types.
enum class InputEventType : unsigned {
	key,
	mouse_button,
	mouse_motion,
	display_focus,
	display_close_request,
	display_resize,
};

enum : unsigned {
	/// Maximum number of displays in input buffer.
	INPUT_SYSTEM_NUM_DISPLAYS = 4,
	/// Default buffer capacity.
	INPUT_SYSTEM_DEFAULT_INIT_CAPACITY = 1024,
};

/// Input buffer.
struct InputBuffer {
	unsigned _num_displays;
	gfx::Display* _displays[INPUT_SYSTEM_NUM_DISPLAYS];
	ObjectBuffer<unsigned, unsigned> _buffer;

	InputBuffer() = delete;
	InputBuffer(InputBuffer const&) = delete;
	InputBuffer(InputBuffer&&) = delete;
	InputBuffer& operator=(InputBuffer const&) = delete;
	InputBuffer& operator=(InputBuffer&&) = delete;

	~InputBuffer();
	InputBuffer(
		Allocator& allocator,
		u32 const init_capacity = INPUT_SYSTEM_DEFAULT_INIT_CAPACITY
	);
};

/// Key codes.
enum class KeyCode : unsigned {
	f1,
	f2,
	f3,
	f4,
	f5,
	f6,
	f7,
	f8,
	f9,
	f10,
	f11,
	f12,

	n0,
	n1,
	n2,
	n3,
	n4,
	n5,
	n6,
	n7,
	n8,
	n9,

	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	i,
	j,
	k,
	l,
	m,
	n,
	o,
	p,
	q,
	r,
	s,
	t,
	u,
	v,
	w,
	x,
	y,
	z,

	numpad_0,
	numpad_1,
	numpad_2,
	numpad_3,
	numpad_4,
	numpad_5,
	numpad_6,
	numpad_7,
	numpad_8,
	numpad_9,
	numpad_decimal,
	numpad_divide,
	numpad_multiply,
	numpad_subtract,
	numpad_add,
	numpad_enter,
	numpad_equal,

	grave,
	tab,
	space,
	minus,
	equal,
	backspace,
	left_bracket,
	right_bracket,
	backslash,
	semicolon,
	apostrophe,
	comma,
	period,
	slash,

	caps_lock,
	scroll_lock,
	num_lock,

	escape,
	enter,

	print_screen,
	pause,
	insert,
	del,
	home,
	end,
	page_up,
	page_down,

	up,
	down,
	left,
	right,

	left_shift,
	left_control,
	left_alt,
	left_super,
	right_shift,
	right_control,
	right_alt,
	right_super,

	COUNT
};

/// Key event actions.
enum class KeyAction : unsigned {
	press,
	release,
	repeat,
};

/// Key modifiers.
enum class KeyMod {
	none = 0,
	alt = 1 << 0,
	ctrl = 1 << 1,
	shift = 1 << 2,
};

/// Key input event.
struct KeyEvent {
	gfx::Display* display;
	KeyAction action;
	KeyCode code;
	KeyMod mods;
};

/// Mouse buttons.
enum class MouseButton : unsigned {
	left,
	right,
	middle,

	COUNT
};

/// Mouse button event actions.
enum class MouseButtonAction : unsigned {
	press,
	release,
};

/// Mouse button event.
struct MouseButtonEvent {
	gfx::Display* display;
	MouseButtonAction action;
	MouseButton button;
};

/// Mouse motion event.
struct MouseMotionEvent {
	gfx::Display* display;
	signed x;
	signed y;
};

/// Display focus event.
struct DisplayFocusEvent {
	gfx::Display* display;
	bool focused;
};

/// Display close request event.
struct DisplayCloseRequestEvent {
	gfx::Display* display;
};

/// Display close request event.
struct DisplayResizeEvent {
	gfx::Display* display;
	unsigned old_width;
	unsigned old_height;
	unsigned new_width;
	unsigned new_height;
};

/// Input event union.
///
/// @warning Members should only be accessed by the reported event
/// type from input_buffer::poll().
union InputEvent {
	gfx::Display* const display;

	KeyEvent const key;
	MouseButtonEvent const mouse_button;
	MouseMotionEvent const mouse_motion;
	DisplayFocusEvent const display_focus;
	DisplayCloseRequestEvent const display_close_request;
	DisplayResizeEvent const display_resize;
};

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<KeyMod> : true_type {};
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group input

} // namespace togo
