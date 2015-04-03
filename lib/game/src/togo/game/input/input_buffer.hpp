#line 2 "togo/game/input/input_buffer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief InputBuffer interface.
@ingroup lib_game_input
@ingroup lib_game_input_buffer

@defgroup lib_game_input_buffer InputBuffer
@ingroup lib_game_input
@details
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/input/types.hpp>
#include <togo/game/input/input_buffer.gen_interface>

namespace togo {

/// Construct with allocator for storage.
inline InputBuffer::InputBuffer(
	Allocator& allocator,
	u32 const init_capacity
)
	: _num_displays(0)
	, _displays()
	, _buffer(allocator, init_capacity)
{}

namespace input_buffer {

/**
	@addtogroup lib_game_input_buffer
	@{
*/

/// Number of displays.
inline unsigned num_displays(InputBuffer& ib) {
	return ib._num_displays;
}

/** @} */ // end of doc-group lib_game_input_buffer

} // namespace input_buffer
} // namespace togo
