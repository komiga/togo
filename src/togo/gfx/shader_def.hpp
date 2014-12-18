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
#include <togo/utility/utility.hpp>
#include <togo/collection/types.hpp>
#include <togo/gfx/types.hpp>

namespace togo {
namespace gfx {
namespace shader_def {

/**
	@addtogroup gfx_shader_def
	@{
*/

/// Source type.
inline u32 type(gfx::ShaderDef const& def) {
	return def.properties & gfx::ShaderDef::TYPE_MASK;
}

/// Source language.
inline u32 language(gfx::ShaderDef const& def) {
	return def.properties & gfx::ShaderDef::LANG_MASK;
}

/// Shared source.
inline StringRef shared_source(gfx::ShaderDef const& def) {
	return StringRef{array::begin(def.data), def.data_offsets[0]};
}

/// Source by stage.
inline StringRef stage_source(
	gfx::ShaderDef const& def,
	gfx::ShaderStage::Type const stage_type
) {
	auto const offset = def.data_offsets[unsigned_cast(stage_type)];
	return StringRef{
		array::begin(def.data) + offset,
		def.data_offsets[unsigned_cast(stage_type) + 1] - offset
	};
}

/// Param block name from data buffer by index.
inline StringRef param_block_name(
	gfx::ShaderDef const& def,
	unsigned index
) {
	index += gfx::ShaderDef::IDX_PARAM_NAMES;
	auto const offset = def.data_offsets[index];
	return StringRef{
		array::begin(def.data) + offset,
		def.data_offsets[index + 1] - offset
	};
}

/// Patch param block names.
///
/// This is used to patch param block definitions after deserialization.
inline void patch_param_block_names(gfx::ShaderDef& def) {
	unsigned index = 0;
	for (auto& pb_def : def.fixed_param_blocks) {
		pb_def.name = gfx::shader_def::param_block_name(def, index);
		++index;
	}
	for (auto& pb_def : def.draw_param_blocks) {
		pb_def.name = gfx::shader_def::param_block_name(def, index);
		++index;
	}
}

/** @} */ // end of doc-group gfx_shader_def

} // namespace shader_def
} // namespace gfx
} // namespace togo
