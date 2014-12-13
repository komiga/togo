#line 2 "togo/gfx/shader_def.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ShaderDef interface.
@ingroup gfx
@ingroup gfx_shader_def
*/

#pragma once

#include <togo/config.hpp>
#include <togo/collection/types.hpp>
#include <togo/gfx/types.hpp>
#include <togo/memory/memory.hpp>

namespace togo {
namespace gfx {
namespace shader_def {

/**
	@addtogroup gfx_shader_def
	@{
*/

/// Shader source type.
inline u32 type(gfx::ShaderDef const& def) {
	return def.properties & gfx::ShaderDef::TYPE_MASK;
}

/// Shader source language.
inline u32 language(gfx::ShaderDef const& def) {
	return def.properties & gfx::ShaderDef::LANG_MASK;
}

/// Shared (prefix) source.
inline StringRef shared_source(gfx::ShaderDef const& def) {
	return array::any(def.data) && 0 != def.vertex_index
		? StringRef{
			array::begin(def.data),
			def.vertex_index
		}
		: ""
	;
}

/// Vertex shader source.
inline StringRef vertex_source(gfx::ShaderDef const& def) {
	return array::any(def.data) && def.vertex_index != def.fragment_index
		? StringRef{
			array::begin(def.data) + def.vertex_index,
			def.fragment_index - def.vertex_index
		}
		: ""
	;
}

/// Fragment shader source.
inline StringRef fragment_source(gfx::ShaderDef const& def) {
	return array::any(def.data) && def.fragment_index != array::size(def.data)
		? StringRef{
			array::begin(def.data) + def.fragment_index,
			static_cast<unsigned>(array::size(def.data) - def.fragment_index)
		}
		: ""
	;
}

/** @} */ // end of doc-group gfx_shader_def

} // namespace shader_def
} // namespace gfx
} // namespace togo
