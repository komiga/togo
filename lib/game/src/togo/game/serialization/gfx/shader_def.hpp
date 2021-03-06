#line 2 "togo/game/serialization/gfx/shader_def.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief gfx::ShaderDef serialization.
@ingroup lib_game_serialization
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/fixed_array.hpp>
#include <togo/core/serialization/array.hpp>
#include <togo/game/gfx/types.hpp>

namespace togo {
namespace game {

/**
	@addtogroup lib_game_serialization
	@{
*/

/** @cond INTERNAL */

enum : u32 {
	SER_FORMAT_VERSION_SHADER_DEF = 2,
};

namespace gfx {

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::ParamBlockDef& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.index
		% value.name_hash
	;
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::ShaderDef& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.properties
		// Clearer binary form when data comes last
		% make_ser_collection<u8>(value.prelude)
		% make_ser_collection<u8>(value.data_offsets)
		% make_ser_collection<u8>(value.fixed_param_blocks)
		% make_ser_collection<u8>(value.draw_param_blocks)
		% make_ser_collection<u32>(value.data)
	;
}

} // namespace gfx

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group lib_game_serialization

} // namespace game
} // namespace togo
