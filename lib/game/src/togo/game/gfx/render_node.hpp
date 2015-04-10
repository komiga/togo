#line 2 "togo/game/gfx/render_node.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief RenderNode interface.
@ingroup lib_game_gfx
@ingroup lib_game_gfx_render_node

@defgroup lib_game_gfx_render_node RenderNode
@ingroup lib_game_gfx
@details
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/command.hpp>
#include <togo/game/gfx/render_node.gen_interface>

namespace togo {
namespace game {
namespace gfx {
namespace render_node {

/**
	@addtogroup lib_game_gfx_render_node
	@{
*/

/// Push command (generic helper).
template<class T>
inline void push(
	gfx::RenderNode& node,
	u64 const key_user,
	T const& data
) {
	render_node::push(
		node,
		key_user,
		CmdTypeProperties<T>::type,
		sizeof_empty<T>(), &data
	);
}

/** @} */ // end of doc-group lib_game_gfx_render_node

} // namespace render_node
} // namespace gfx
} // namespace game
} // namespace togo
