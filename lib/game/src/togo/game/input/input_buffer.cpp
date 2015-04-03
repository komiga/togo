#line 2 "togo/game/input/input_buffer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/game/gfx/display/types.hpp>
#include <togo/game/gfx/display/private.hpp>
#include <togo/game/input/input_buffer.hpp>

#undef TOGO_TEST_LOG_ENABLE
#if defined(TOGO_TEST_INPUT_BUFFER)
	#define TOGO_TEST_LOG_ENABLE 
#endif
#include <togo/core/log/test.hpp>

namespace togo {

InputBuffer::~InputBuffer() {
	for (auto& display : _displays) {
		if (display != nullptr) {
			gfx::display::detach_from_input_buffer(display);
			display = nullptr;
			--_num_displays;
		}
	}
}

/// Add a display.
///
/// If the display is already owned by an input buffer, an assertion
/// will fail. If the input buffer has no space for more displays, an
/// assertion will fail.
void input_buffer::add_display(InputBuffer& ib, gfx::Display* display) {
	TOGO_DEBUG_ASSERTE(display);
	TOGO_ASSERT(
		ib._num_displays < INPUT_SYSTEM_NUM_DISPLAYS,
		"input buffer has no space for display"
	);
	for (auto& it : ib._displays) {
		if (it == nullptr) {
			it = display;
			++ib._num_displays;
			gfx::display::attach_to_input_buffer(display, ib);
			return;
		}
	}
	TOGO_ASSERT(false, "something has gone terribly wrong");
}

/// Remove a display.
///
/// If the display is not owned by the input buffer, an assertion
/// will fail.
void input_buffer::remove_display(InputBuffer& ib, gfx::Display* display) {
	TOGO_DEBUG_ASSERTE(display);
	for (auto& it : ib._displays) {
		if (it == display) {
			gfx::display::detach_from_input_buffer(display);
			it = nullptr;
			--ib._num_displays;
			return;
		}
	}
	TOGO_ASSERT(false, "something has gone terribly wrong");
}

static void update_input_states(gfx::Display* display) {
	for (auto const code : display->_key_clear_queue) {
		display->_key_states[
			unsigned_cast(code)
		] &= ~(1 << unsigned_cast(KeyAction::release));
	}
	fixed_array::clear(display->_key_clear_queue);
	for (auto& state : display->_mouse_button_states) {
		state &= ~(1 << unsigned_cast(MouseButtonAction::release));
	}
}

/// Update input states.
///
/// This should be executed once per frame before polling the input
/// buffer.
void input_buffer::update(InputBuffer& ib) {
	for (auto display : ib._displays) {
		if (!display) {
			continue;
		}
		update_input_states(display);
	}
}

static void set_key_state(KeyEvent const& event) {
	gfx::Display* const display = event.display;
	if (event.action == KeyAction::press) {
		// If state already has KeyAction::release, retain it
		// (it will be cleared by update())
		display->_key_states[
			unsigned_cast(event.code)
		] |= 1 << unsigned_cast(event.action);
	} else if (event.action == KeyAction::release) {
		// Replace state and enqueue clear
		display->_key_states[
			unsigned_cast(event.code)
		] = 1 << unsigned_cast(event.action);
		for (auto const code : display->_key_clear_queue) {
			if (code == event.code) {
				return;
			}
		}
		if (fixed_array::space(display->_key_clear_queue)) {
			fixed_array::push_back(display->_key_clear_queue, event.code);
		} else {
			TOGO_LOGF(
				"input_buffer: warning: discarded key-clear for key %u on display %p\n",
				unsigned_cast(event.code),
				event.display
			);
		}
	}
}

/// Poll events.
///
/// Returns true if an event was fetched.
///
/// @warning This must be called on the thread that created the
/// displays.
bool input_buffer::poll(
	InputBuffer& ib,
	InputEventType& type,
	InputEvent const*& event
) {
	if (!ib._buffer._consume_mode) {
		gfx::display::process_events(ib);
		if (!object_buffer::empty(ib._buffer)) {
			object_buffer::begin_consume(ib._buffer);
		} else {
			return false;
		}
	} else {
		if (!object_buffer::has_more(ib._buffer)) {
			object_buffer::end_consume(ib._buffer);
			return false;
		}
	}
	void const* vptr = nullptr;
	object_buffer::read(ib._buffer, type, vptr);
	event = static_cast<InputEvent const*>(vptr);
	#if defined(TOGO_TEST_INPUT_BUFFER)
		TOGO_TEST_LOGF("input event: %p => ", event->display);
		switch (type) {
		case InputEventType::key:
			TOGO_TEST_LOGF(
				"key: action = %u  code = %u  mods = %u\n",
				unsigned_cast(event->key.action),
				unsigned_cast(event->key.code),
				unsigned_cast(event->key.mods)
			);
			break;
		case InputEventType::mouse_button:
			TOGO_TEST_LOGF(
				"mouse_button: action = %u   button = %u\n",
				unsigned_cast(event->mouse_button.action),
				unsigned_cast(event->mouse_button.button)
			);
			break;
		case InputEventType::mouse_motion:
			TOGO_TEST_LOGF(
				"mouse_motion: (%d, %d)\n",
				event->mouse_motion.x, event->mouse_motion.y
			);
			break;
		case InputEventType::display_focus:
			TOGO_TEST_LOGF(
				"display_focus: focused = %s\n",
				event->display_focus.focused ? "true" : "false"
			);
			break;
		case InputEventType::display_close_request:
			TOGO_TEST_LOG("display_close_request\n");
			break;
		case InputEventType::display_resize:
			TOGO_TEST_LOGF(
				"display_resize: (%-4u, %-4u) -> (%-4u, %-4u)\n",
				event->display_resize.old_width,
				event->display_resize.old_height,
				event->display_resize.new_width,
				event->display_resize.new_height
			);
			break;
		}
	#endif
	switch (type) {
	case InputEventType::key:
		set_key_state(event->key);
		break;

	case InputEventType::mouse_button:
		if (event->mouse_button.action == MouseButtonAction::press) {
			event->display->_mouse_button_states[
				unsigned_cast(event->mouse_button.button)
			] |= 1 << unsigned_cast(event->mouse_button.action);
		} else if (event->mouse_button.action == MouseButtonAction::release) {
			event->display->_mouse_button_states[
				unsigned_cast(event->mouse_button.button)
			]  = 1 << unsigned_cast(event->mouse_button.action);
		}
		break;

	case InputEventType::mouse_motion:
		event->display->_mouse_x = event->mouse_motion.x;
		event->display->_mouse_y = event->mouse_motion.y;
		break;

	default:
		break;
	};
	return true;
}

} // namespace togo
