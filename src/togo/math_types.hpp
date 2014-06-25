/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file math_types.hpp
@brief Math types.
@ingroup types
@ingroup math
*/

#pragma once

#include <togo/config.hpp>

#include <am/linear/vector_types.hpp>
#include <am/linear/matrix_types.hpp>

namespace togo {

/**
	@addtogroup types
	@{
*/
/**
	@addtogroup math
	@{
*/

/** @name Vector types */ /// @{
/// 2-dimensional vector.
using Vec1 = am::linear::vec1;
/// 2-dimensional vector.
using Vec2 = am::linear::vec2;
/// 3-dimensional vector.
using Vec3 = am::linear::vec3;
/// 4-dimensional vector.
using Vec4 = am::linear::vec4;
/// @}

/** @name Matrix types */ /// @{
/// 2x2 matrix.
using Mat2x2 = am::linear::mat2x2;
/// 2x3 matrix.
using Mat2x3 = am::linear::mat2x3;
/// 2x4 matrix.
using Mat2x4 = am::linear::mat2x4;

/// 3x2 matrix.
using Mat3x2 = am::linear::mat3x2;
/// 3x3 matrix.
using Mat3x3 = am::linear::mat3x3;
/// 3x4 matrix.
using Mat3x4 = am::linear::mat3x4;

/// 4x2 matrix.
using Mat4x2 = am::linear::mat4x2;
/// 4x3 matrix.
using Mat4x3 = am::linear::mat4x3;
/// 4x4 matrix.
using Mat4x4 = am::linear::mat4x4;
/// @}

/** @} */ // end of doc-group math
/** @} */ // end of doc-group types

} // namespace togo
