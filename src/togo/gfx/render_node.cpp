#line 2 "togo/gfx/render_node.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/command.hpp>
#include <togo/gfx/render_node.hpp>
#include <togo/gfx/renderer/types.hpp>

#include <cstring>

namespace togo {
namespace gfx {

/// Push command.
///
/// data is copied to the node's internal buffer.
///
/// An assertion will fail if the node does not have space for another
/// command.
/// An assertion will fail if key_user is not contained within the
/// user key bit space.
void render_node::push(
	gfx::RenderNode& node,
	u64 const key_user,
	gfx::CmdType const type,
	unsigned const data_size,
	void const* const data
) {
	TOGO_ASSERTE(
		TOGO_GFX_NODE_NUM_COMMANDS > node.num_commands &&
		(key_user & ~TOGO_GFX_KEY_USER_MASK) == 0
	);
	unsigned const next_buffer_size = node.buffer_size + sizeof(gfx::CmdType) + data_size;
	TOGO_ASSERTE(TOGO_GFX_NODE_BUFFER_SIZE >= next_buffer_size);

	auto* put = static_cast<void*>(node.buffer + node.buffer_size);
	auto& cmd_key = node.keys[node.num_commands];
	cmd_key.key = (node.sequence << TOGO_GFX_KEY_USER_BITS) | key_user;
	cmd_key.data = put;

	*static_cast<gfx::CmdType*>(put) = type;
	if (data_size > 0) {
		put = pointer_add(put, sizeof(gfx::CmdType));
		std::memcpy(put, data, data_size);
	}

	node.buffer_size = next_buffer_size;
	++node.num_commands;
}

} // namespace gfx
} // namespace togo
