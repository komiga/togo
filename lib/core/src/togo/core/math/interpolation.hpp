#line 2 "togo/core/math/interpolation.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Interpolation interface.
@ingroup lib_core_math
@ingroup lib_core_math_interpolation

@defgroup lib_core_math_interpolation Interpolation
@ingroup lib_core_math
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/math/traits.hpp>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_interpolation
	@{
*/

/// Linearly interpolate between two values with individual weights.
///
/// @note w0 and w1 must be unit length and must add up to 1. If they do not
/// follow this constraint, the result can go outside [v0, v1].
template<class C>
inline C lerp_independent(
	C const& v0,
	math::value_type<C> const w0,
	C const& v1,
	math::value_type<C> const w1
) {
	return (v0 * w0) + (v1 * w1);
}

/// Linearly interpolate between two values.
///
/// @note t must be unit length. If t is outside the range [0, 1] (inclusive),
/// the result will be beyond v0 or v1.
template<class C>
inline C lerp(
	C const& v0,
	C const& v1,
	math::value_type<C> const t
) {
	using V = math::value_type<C>;
	return lerp_independent(
		v0, V{1} - t,
		v1, t
	);
}

/// Evaluate point in a cubic Bézier curve.
///
/// @note t must be in [0, 1] (inclusive).
template<class C>
inline C bezier_cubic(
	C const& v0,
	C const& v1,
	C const& v2,
	C const& v3,
	math::value_type<C> t
) {
	using V = math::value_type<C>;
	// Brute force:
	//return lerp(
	//	lerp(lerp(v0, v1, t), lerp(v1, v2, t), t),
	//	lerp(lerp(v1, v2, t), lerp(v2, v3, t), t),
	//	t
	//);

	// Reduction:
	// v0 * (1-t)^3 +
	// v1 * (1-t)^2 * 3*t +
	// v2 * 3*(1-t) * t^2 +
	// v3 * t^3
	V r = V{1} - t;
	C const i2 = v2 * V{3}*r * t*t;
	r *= r;
	return
		(v0 * r*r) +
		(v1 * r * V{3}*t) +
		i2 +
		(v3 * t*t*t)
	;
}

/** @} */ // end of doc-group lib_core_math_interpolation

} // namespace math
} // namespace togo
