#line 2 "togo/game/gfx/gfx.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Graphics interface.
@ingroup lib_game_gfx
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/game/gfx/types.hpp>

namespace togo {
namespace game {
namespace gfx {

/**
	@addtogroup lib_game_gfx
	@{
*/

/// Calculate hash of parameter block name.
inline gfx::ParamBlockNameHash hash_param_block_name(StringRef const& name) {
	return hash::calc32(name);
}

/// Calculate hash of generator name.
inline gfx::GeneratorNameHash hash_generator_name(StringRef const& name) {
	return hash::calc32(name);
}

/// Calculate hash of viewport name.
inline gfx::GeneratorNameHash hash_viewport_name(StringRef const& name) {
	return hash::calc32(name);
}

/// Test if a primitive is integral.
inline bool is_primitive_integral(gfx::Primitive p) {
	return p <= gfx::Primitive::u32;
}

/// Test if a primitive is floating-point.
inline bool is_primitive_fp(gfx::Primitive p) {
	return p == gfx::Primitive::f32 || p == gfx::Primitive::f64;
}

/** @} */ // end of doc-group lib_game_gfx

} // namespace gfx
} // namespace game
} // namespace togo
