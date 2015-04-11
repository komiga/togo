#line 2 "togo/core/serialization/array.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Array serialization.
@ingroup lib_core_serialization
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/serialization/support.hpp>

namespace togo {

/**
	@addtogroup lib_core_serialization
	@{
*/

/** @cond INTERNAL */

namespace {
	template<class T> struct is_md_vector : false_type {};
	template<> struct is_md_vector<Vec2> : true_type {};
	template<> struct is_md_vector<Vec3> : true_type {};
	template<> struct is_md_vector<Vec4> : true_type {};
} // anonymous namespace

template<class Ser, class T>
inline enable_if<is_same<remove_cv<T>, Vec1>::value, void>
serialize(serializer_tag, Ser& ser, T& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser % value.x;
}

template<class Ser, class T>
inline enable_if<is_md_vector<T>::value, void>
serialize(serializer_tag, Ser& ser, T& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser % make_ser_sequence(&value.x, T::size());
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group lib_core_serialization

} // namespace togo
