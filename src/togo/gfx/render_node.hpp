#line 2 "togo/gfx/render_node.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief RenderNode interface.
@ingroup gfx
@ingroup gfx_render_node
*/

#pragma once

#include <togo/config.hpp>
#include <togo/utility/utility.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/command.hpp>

namespace togo {
namespace gfx {
namespace render_node {

/**
	@addtogroup gfx_render_node
	@{
*/

/// Push command.
///
/// data is copied to the node's internal buffer.
///
/// An assertion will fail if the node does not have space for another
/// command.
/// An assertion will fail if key_user is not contained within the
/// user key bit space.
void push(
	gfx::RenderNode& node,
	u64 key_user,
	gfx::CmdType type,
	unsigned data_size,
	void const* data
);

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

/** @} */ // end of doc-group gfx_render_node

} // namespace render_node
} // namespace gfx
} // namespace togo
