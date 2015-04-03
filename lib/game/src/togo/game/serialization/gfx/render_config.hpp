#line 2 "togo/game/serialization/gfx/render_config.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief gfx::RenderConfig serialization.
@ingroup serialization
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/fixed_array.hpp>
#include <togo/core/serialization/string.hpp>
#include <togo/game/gfx/renderer/types.hpp>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

enum : u32 {
	SER_FORMAT_VERSION_RENDER_CONFIG = 3,
};

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, gfx::GeneratorUnit& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser % value.name_hash;
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
		% make_ser_collection<u8>(value.viewports)
		% make_ser_collection<u8>(value.pipes)
	;
	// NB: GeneratorUnit data is stored immediately after pipes in
	// sequential order. Units must be fixed up externally.
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serialization

} // namespace togo
