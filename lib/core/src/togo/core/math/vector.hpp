#line 2 "togo/core/math/vector.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Vector interface.
@ingroup lib_core_math
@ingroup lib_core_math_vector
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/interpolation.hpp>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_vector
	@{
*/

/** @cond INTERNAL */
#define TOGO_REQUIRE_FLOATING_POINT(C)									\
	static_assert(math::is_vector<C>::value, "");						\
	static_assert(math::is_floating_point<C>::value, "")
/** @endcond */

/// Calculate the length of a vector.
template<class C>
inline math::value_type<C> length(C const& v) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return C::operations::length(v);
}

/// Calculate the distance between two vectors.
template<class C>
inline math::value_type<C> distance(C const& v, C const& r) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return C::operations::distance(v, r);
}

/// Calculate the dot product of two vectors.
template<class C>
inline math::value_type<C> dot(C const& v, C const& r) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return C::operations::dot(v, r);
}

/// Calculate the cross product of two 3-dimensional vectors.
template<class T>
inline math::TVec3<T> cross(math::TVec3<T> const& v, math::TVec3<T> const& r) {
	TOGO_REQUIRE_FLOATING_POINT(math::TVec3<T>);
	return math::TVec3<T>::operations::cross(v, r);
}

/// Normalize a vector.
template<class C>
inline C normalize(C const& v) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return C::operations::normalize(v);
}

/// Orient a normal to point away from a surface.
template<class C>
inline C faceforward(C const& n, C const& i, C const& ng) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return C::operations::faceforward(n, i, ng);
}

/// Calculate the reflection direction across a surface.
template<class C>
inline C reflect(C const& i, C const& n) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return C::operations::reflect(i, n);
}

/// Calculate the refraction direction across a surface.
template<class C>
inline C refract(C const& i, C const& n, math::value_type<C> const& eta) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return C::operations::refract(i, n, eta);
}

/// Linearly interpolate between two values.
template<class C>
inline C mix(C const& x, C const& y, math::value_type<C> const& a) {
	TOGO_REQUIRE_FLOATING_POINT(C);
	return math::lerp(x, y, a);
}

#undef TOGO_REQUIRE_FLOATING_POINT

/** @} */ // end of doc-group lib_core_math_vector

} // namespace math
} // namespace togo
