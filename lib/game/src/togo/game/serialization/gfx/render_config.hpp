#line 2 "togo/game/serialization/gfx/render_config.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief gfx::RenderConfig serialization.
@ingroup lib_game_serialization
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/fixed_array.hpp>
#include <togo/core/serialization/array.hpp>
#include <togo/core/serialization/string.hpp>
#include <togo/game/gfx/renderer/types.hpp>

namespace togo {
namespace game {

/**
	@addtogroup lib_game_serialization
	@{
*/

/** @cond INTERNAL */

enum : u32 {
	SER_FORMAT_VERSION_RENDER_CONFIG = 5,
};

namespace gfx {

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::RenderTargetSpec& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.properties
		% value.format
		% value.dim_x
		% value.dim_y
	;
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::GeneratorUnit& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser % value.name_hash;
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::RenderConfigResource& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.name_hash
		% make_ser_string<u8>(value.name)
		% value.properties
	;
	switch (value.properties & gfx::RenderConfigResource::MASK_TYPE) {
	case gfx::RenderConfigResource::TYPE_RENDER_TARGET:
		ser % value.data.render_target;
		break;

	default:
		TOGO_ASSERT(false, "render config resource type not recognized");
		break;
	}
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::Layer& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.name_hash
		% make_ser_string<u8>(value.name)
		% make_ser_collection<u8>(value.rts)
		% value.dst
		% make_ser_proxy<u8>(value.order)
		% make_ser_proxy<u8>(value.seq_base)
		% make_ser_collection<u8>(value.layout)
	;
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::Pipe& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.name_hash
		% make_ser_string<u8>(value.name)
		% make_ser_collection<u8>(value.layers)
	;
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::Viewport& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.name_hash
		% make_ser_string<u8>(value.name)
		% value.pipe
		% value.output_rt
		% value.output_dst
	;
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::RenderConfig& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% make_ser_collection<u8>(value.shared_resources)
		% make_ser_collection<u8>(value.viewports)
		% make_ser_collection<u8>(value.pipes)
	;
}

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::PackedRenderConfig& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.config
		% make_ser_collection<u32>(value.unit_data)
	;
}

} // namespace gfx

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group lib_game_serialization

} // namespace game
} // namespace togo
