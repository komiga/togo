#line 2 "togo/core/math/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Math types.
@ingroup lib_core_types
@ingroup lib_core_math
*/

#pragma once

#include <togo/core/config.hpp>

#include <am/linear/vector_types.hpp>
#include <am/linear/matrix_types.hpp>

namespace togo {

/**
	@addtogroup lib_core_math
	@{
*/

/** @name Vector types */ /// @{
/// 1-dimensional f32 vector.
using Vec1 = am::linear::vec1;
/// 2-dimensional f32 vector.
using Vec2 = am::linear::vec2;
/// 3-dimensional f32 vector.
using Vec3 = am::linear::vec3;
/// 4-dimensional f32 vector.
using Vec4 = am::linear::vec4;

/// 1-dimensional s32 vector.
using SVec1 = am::linear::ivec1;
/// 2-dimensional s32 vector.
using SVec2 = am::linear::ivec2;
/// 3-dimensional s32 vector.
using SVec3 = am::linear::ivec3;
/// 4-dimensional s32 vector.
using SVec4 = am::linear::ivec4;

/// 1-dimensional u32 vector.
using UVec1 = am::linear::uvec1;
/// 2-dimensional u32 vector.
using UVec2 = am::linear::uvec2;
/// 3-dimensional u32 vector.
using UVec3 = am::linear::uvec3;
/// 4-dimensional u32 vector.
using UVec4 = am::linear::uvec4;
/// @}

/** @name Matrix types */ /// @{
/// 2x2 f32 matrix.
using Mat2x2 = am::linear::mat2x2;
/// 2x3 f32 matrix.
using Mat2x3 = am::linear::mat2x3;
/// 2x4 f32 matrix.
using Mat2x4 = am::linear::mat2x4;

/// 3x2 f32 matrix.
using Mat3x2 = am::linear::mat3x2;
/// 3x3 f32 matrix.
using Mat3x3 = am::linear::mat3x3;
/// 3x4 f32 matrix.
using Mat3x4 = am::linear::mat3x4;

/// 4x2 f32 matrix.
using Mat4x2 = am::linear::mat4x2;
/// 4x3 f32 matrix.
using Mat4x3 = am::linear::mat4x3;
/// 4x4 f32 matrix.
using Mat4x4 = am::linear::mat4x4;
/// @}

/** @} */ // end of doc-group lib_core_math

} // namespace togo
