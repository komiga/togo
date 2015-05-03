#line 2 "togo/window/input/input_buffer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief InputBuffer interface.
@ingroup lib_window_input
@ingroup lib_window_input_buffer

@defgroup lib_window_input_buffer InputBuffer
@ingroup lib_window_input
@details
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/window/window/types.hpp>
#include <togo/window/input/types.hpp>
#include <togo/window/input/input_buffer.gen_interface>

namespace togo {

/// Construct with allocator for storage.
inline InputBuffer::InputBuffer(
	Allocator& allocator,
	u32 const init_capacity
)
	: _num_windows(0)
	, _windows()
	, _buffer(allocator, init_capacity)
{}

namespace input_buffer {

/**
	@addtogroup lib_window_input_buffer
	@{
*/

/// Number of windows.
inline unsigned num_windows(InputBuffer& ib) {
	return ib._num_windows;
}

/** @} */ // end of doc-group lib_window_input_buffer

} // namespace input_buffer

} // namespace togo
