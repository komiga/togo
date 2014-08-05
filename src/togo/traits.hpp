#line 2 "togo/traits.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file traits.hpp
@brief Type traits.
@ingroup utility
*/

#pragma once

#include <togo/config.hpp>

namespace togo {

/**
	@addtogroup utility
	@{
*/

/** @name Type traits */ /// @{

/// Type with static constexpr value equal to false.
struct false_type {
	static constexpr bool const value = false;
};

/// Type with static constexpr value equal to true.
struct true_type {
	static constexpr bool const value = true;
};

/** @cond INTERNAL */
namespace {
	template<bool, class>
	struct enable_if_impl;

	template<class T>
	struct enable_if_impl<false, T> {};

	template<class T>
	struct enable_if_impl<true, T> {
		using type = T;
	};
} // anonymous namespace
/** @endcond */ // INTERNAL

/// SFINAE enabler type alias.
template<bool C, class T = void>
using enable_if = typename enable_if_impl<C, T>::type;

namespace {

template<class T>
struct remove_ref_impl {
	using type = T;
};

template<class T>
struct remove_ref_impl<T&> {
	using type = T;
};

} // anonymous namespace

/// Remove reference qualification from type.
template<class T>
using remove_ref = typename remove_ref_impl<T>::type;

namespace {

template<class T>
struct remove_cvr_impl {
	using type = T;
};

template<class T>
struct remove_cvr_impl<T const> {
	using type = T;
};

template<class T>
struct remove_cvr_impl<T volatile> {
	using type = T;
};

template<class T>
struct remove_cvr_impl<T const volatile> {
	using type = T;
};

} // anonymous namespace

/// Remove const, volatile, and reference qualifications from type.
template<class T>
using remove_cvr = typename remove_cvr_impl<remove_ref<T>>::type;

namespace {

template<class T>
struct is_signed_impl : false_type {};

template<> struct is_signed_impl<signed char> : true_type {};
template<> struct is_signed_impl<signed short> : true_type {};
template<> struct is_signed_impl<signed int> : true_type {};
template<> struct is_signed_impl<signed long> : true_type {};
template<> struct is_signed_impl<signed long long> : true_type {};

} // anonymous namespace

/// Type with static constexpr value equal to the true if T is
/// a signed integral.
template<class T>
using is_signed = is_signed_impl<remove_cvr<T>>;

/// @}

/// Enum-class bitwise operator enabler.
///
/// Specialize this class deriving from true_type to enable bit-wise
/// operators for an enum-class.
template<class>
struct enable_enum_bitwise_ops : false_type {};

/** @} */ // end of doc-group utility

} // namespace togo
