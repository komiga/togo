#line 2 "togo/window/input/input_buffer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/window/impl/private.hpp>
#include <togo/window/input/input_buffer.hpp>

#undef TOGO_TEST_LOG_ENABLE
#if defined(TOGO_TEST_INPUT_BUFFER)
	#define TOGO_TEST_LOG_ENABLE
#endif
#include <togo/core/log/test.hpp>

namespace togo {

InputBuffer::~InputBuffer() {
	for (auto& window : _windows) {
		if (window != nullptr) {
			window::detach_from_input_buffer(window);
			window = nullptr;
			--_num_windows;
		}
	}
}

/// Add a window.
///
/// If the window is already owned by an input buffer, an assertion
/// will fail. If the input buffer has no space for more windows, an
/// assertion will fail.
void input_buffer::add_window(InputBuffer& ib, Window* window) {
	TOGO_DEBUG_ASSERTE(window);
	TOGO_ASSERT(
		ib._num_windows < INPUT_SYSTEM_NUM_WINDOWS,
		"input buffer has no space for window"
	);
	for (auto& it : ib._windows) {
		if (it == nullptr) {
			it = window;
			++ib._num_windows;
			window::attach_to_input_buffer(window, ib);
			return;
		}
	}
	TOGO_ASSERT(false, "something has gone terribly wrong");
}

/// Remove a window.
///
/// If the window is not owned by the input buffer, an assertion
/// will fail.
void input_buffer::remove_window(InputBuffer& ib, Window* window) {
	TOGO_DEBUG_ASSERTE(window);
	for (auto& it : ib._windows) {
		if (it == window) {
			window::detach_from_input_buffer(window);
			it = nullptr;
			--ib._num_windows;
			return;
		}
	}
	TOGO_ASSERT(false, "something has gone terribly wrong");
}

static void update_input_states(Window* window) {
	for (auto const code : window->_key_clear_queue) {
		window->_key_states[
			unsigned_cast(code)
		] &= ~(1 << unsigned_cast(KeyAction::release));
	}
	fixed_array::clear(window->_key_clear_queue);
	for (auto& state : window->_mouse_button_states) {
		state &= ~(1 << unsigned_cast(MouseButtonAction::release));
	}
}

/// Update input states.
///
/// This should be executed once per frame before polling the input
/// buffer.
void input_buffer::update(InputBuffer& ib) {
	for (auto window : ib._windows) {
		if (!window) {
			continue;
		}
		update_input_states(window);
	}
}

static void set_key_state(KeyEvent const& event) {
	Window* const window = event.window;
	if (event.action == KeyAction::press) {
		// If state already has KeyAction::release, retain it
		// (it will be cleared by update())
		window->_key_states[
			unsigned_cast(event.code)
		] |= 1 << unsigned_cast(event.action);
	} else if (event.action == KeyAction::release) {
		// Replace state and enqueue clear
		window->_key_states[
			unsigned_cast(event.code)
		] = 1 << unsigned_cast(event.action);
		for (auto const code : window->_key_clear_queue) {
			if (code == event.code) {
				return;
			}
		}
		if (fixed_array::space(window->_key_clear_queue)) {
			fixed_array::push_back(window->_key_clear_queue, event.code);
		} else {
			TOGO_LOGF(
				"input_buffer: warning: discarded key-clear for key %u on window %p\n",
				unsigned_cast(event.code),
				event.window
			);
		}
	}
}

/// Poll events.
///
/// Returns true if an event was fetched.
///
/// @warning This must be called on the thread that created the
/// windows.
bool input_buffer::poll(
	InputBuffer& ib,
	InputEventType& type,
	InputEvent const*& event
) {
	if (!ib._buffer._consume_mode) {
		window::process_events(ib);
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
		TOGO_TEST_LOGF("input event: %p => ", event->window);
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
		case InputEventType::window_focus:
			TOGO_TEST_LOGF(
				"window_focus: focused = %s\n",
				event->window_focus.focused ? "true" : "false"
			);
			break;
		case InputEventType::window_close_request:
			TOGO_TEST_LOG("window_close_request\n");
			break;
		case InputEventType::window_resize:
			TOGO_TEST_LOGF(
				"window_resize: (%-4u, %-4u) -> (%-4u, %-4u)\n",
				event->window_resize.old_size.width,
				event->window_resize.old_size.height,
				event->window_resize.new_size.width,
				event->window_resize.new_size.height
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
			event->window->_mouse_button_states[
				unsigned_cast(event->mouse_button.button)
			] |= 1 << unsigned_cast(event->mouse_button.action);
		} else if (event->mouse_button.action == MouseButtonAction::release) {
			event->window->_mouse_button_states[
				unsigned_cast(event->mouse_button.button)
			]  = 1 << unsigned_cast(event->mouse_button.action);
		}
		break;

	case InputEventType::mouse_motion:
		event->window->_mouse_x = event->mouse_motion.x;
		event->window->_mouse_y = event->mouse_motion.y;
		break;

	default:
		break;
	};
	return true;
}

} // namespace togo
