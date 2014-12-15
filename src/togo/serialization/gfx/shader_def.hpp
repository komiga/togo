#line 2 "togo/serialization/gfx/shader_def.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief gfx::ShaderDef serialization.
@ingroup serialization
*/

#pragma once

#include <togo/config.hpp>
#include <togo/gfx/types.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/array.hpp>
#include <togo/serialization/fixed_array.hpp>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

enum : u32 {
	SER_FORMAT_VERSION_SHADER_DEF = 2,
};


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
		% make_ser_collection<u8>(value.data_indices)
		% make_ser_collection<u8>(value.fixed_param_blocks)
		% make_ser_collection<u8>(value.draw_param_blocks)
		% make_ser_collection<u32>(value.data)
	;
}

/** @} */ // end of doc-group serialization

} // namespace togo
