#line 2 "togo/gfx/command_types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Command types.
@ingroup types
@ingroup gfx
@ingroup gfx_command
*/

#pragma once

#include <togo/config.hpp>
#include <togo/gfx/types.hpp>

namespace togo {
namespace gfx {

/**
	@addtogroup gfx_command
	@{
*/

/// Command types.
enum class CmdType : u8 {
	Callback,
	ClearBackbuffer,
	RenderBuffers,
};

template<class T>
struct CmdTypeProperties;

#define TOGO_GFX_CMD(type_name_)										\
	struct Cmd ## type_name_;											\
	template<> struct CmdTypeProperties<Cmd ## type_name_> {			\
		static constexpr auto const type = gfx::CmdType::type_name_;	\
	};																	\
	struct Cmd ## type_name_

/// renderer::clear_backbuffer() command.
TOGO_GFX_CMD(ClearBackbuffer) {};

/// renderer::render_buffers() command.
TOGO_GFX_CMD(RenderBuffers) {
	gfx::ShaderID shader_id;
	u32 num_draw_param_blocks;
	u32 num_buffers;
	gfx::ParamBlockBinding const* draw_param_blocks;
	gfx::BufferBindingID const* buffers;
};

#undef TOGO_GFX_CMD

/** @} */ // end of doc-group gfx_command

} // namespace gfx
} // namespace togo
