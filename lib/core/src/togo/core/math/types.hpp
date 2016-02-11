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
#include <togo/core/types.hpp>

namespace togo {

/**
	@addtogroup lib_core_math
	@{
*/

/// Mathematical constant π (pi).
static constexpr float const MC_PI = 3.1415926535897932384626f;
/// Mathematical constant π/2 (pi/2).
static constexpr float const MC_PI_HALF = 1.5707963267948966192313f;
/// Mathematical constant τ (tau, 2 * MC_PI).
static constexpr float const MC_TAU = 6.2831853071795864769252f;

/** @} */ // end of doc-group lib_core_math

// Forward declarations
namespace math {

template<class T> struct TVec1;
template<class T> struct TVec2;
template<class T> struct TVec3;
template<class T> struct TVec4;

template<class T> struct TMat2x2;
template<class T> struct TMat2x3;
template<class T> struct TMat2x4;

template<class T> struct TMat3x2;
template<class T> struct TMat3x3;
template<class T> struct TMat3x4;

template<class T> struct TMat4x2;
template<class T> struct TMat4x3;
template<class T> struct TMat4x4;

} // namespace math

/**
	@addtogroup lib_core_math_vector
	@{
*/

/// 1-dimensional f32 vector.
using Vec1 = math::TVec1<f32>;
/// 2-dimensional f32 vector.
using Vec2 = math::TVec2<f32>;
/// 3-dimensional f32 vector.
using Vec3 = math::TVec3<f32>;
/// 4-dimensional f32 vector.
using Vec4 = math::TVec4<f32>;

/// 1-dimensional s32 vector.
using SVec1 = math::TVec1<s32>;
/// 2-dimensional s32 vector.
using SVec2 = math::TVec2<s32>;
/// 3-dimensional s32 vector.
using SVec3 = math::TVec3<s32>;
/// 4-dimensional s32 vector.
using SVec4 = math::TVec4<s32>;

/// 1-dimensional u32 vector.
using UVec1 = math::TVec1<u32>;
/// 2-dimensional u32 vector.
using UVec2 = math::TVec2<u32>;
/// 3-dimensional u32 vector.
using UVec3 = math::TVec3<u32>;
/// 4-dimensional u32 vector.
using UVec4 = math::TVec4<u32>;

/** @} */ // end of doc-group lib_core_math_vector

/**
	@addtogroup lib_core_math_matrix
	@{
*/

/// 2-by-2 f32 matrix.
using Mat2x2 = math::TMat2x2<f32>;
/// 2-by-3 f32 matrix.
using Mat2x3 = math::TMat2x3<f32>;
/// 2-by-4 f32 matrix.
using Mat2x4 = math::TMat2x4<f32>;

/// 3-by-2 f32 matrix.
using Mat3x2 = math::TMat3x2<f32>;
/// 3-by-3 f32 matrix.
using Mat3x3 = math::TMat3x3<f32>;
/// 3-by-4 f32 matrix.
using Mat3x4 = math::TMat3x4<f32>;

/// 4-by-2 f32 matrix.
using Mat4x2 = math::TMat4x2<f32>;
/// 4-by-3 f32 matrix.
using Mat4x3 = math::TMat4x3<f32>;
/// 4-by-4 f32 matrix.
using Mat4x4 = math::TMat4x4<f32>;

/** @} */ // end of doc-group lib_core_math_matrix

} // namespace togo
