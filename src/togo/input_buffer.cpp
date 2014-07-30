#line 2 "togo/input_buffer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/impl/gfx/display/private.hpp>
#include <togo/object_buffer.hpp>
#include <togo/input_buffer.hpp>

#undef TOGO_TEST_LOG_ENABLE
#if defined(TOGO_TEST_INPUT_BUFFER)
	#define TOGO_TEST_LOG_ENABLE 
#endif
#include <togo/log_test.hpp>

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
			TOGO_TEST_LOG("key\n");
			break;
		case InputEventType::mouse_button:
			TOGO_TEST_LOG("mouse_button\n");
			break;
		case InputEventType::mouse_motion:
			TOGO_TEST_LOG("mouse_motion\n");
			break;
		case InputEventType::display_focus:
			TOGO_TEST_LOG("display_focus\n");
			break;
		case InputEventType::display_close_request:
			TOGO_TEST_LOG("display_close_request\n");
			break;
		}
	#endif
	return true;
}

} // namespace togo
