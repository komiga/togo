#line 2 "togo/core/utility/traits.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Type traits.
@ingroup lib_core_utility
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_core_utility
	@{
*/

/** @name Type traits */ /// @{

/// Type wrapping static constexpr value.
template<class T, T V>
struct constant_type {
	static constexpr T const value = V;
};

/// Boolean constant of false.
using false_type = constant_type<bool, false>;

/// Boolean constant of true.
using true_type = constant_type<bool, true>;

namespace {
	template<bool, class, class>
	struct type_if_impl;

	template<class T, class E>
	struct type_if_impl<false, T, E> {
		using type = E;
	};

	template<class T, class E>
	struct type_if_impl<true, T, E> {
		using type = T;
	};
} // anonymous namespace

/// Type alias to T if C is true, or E if C is false.
template<bool C, class T, class E>
using type_if = typename type_if_impl<C, T, E>::type;

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

template<class T> struct is_const_impl : false_type {};
template<class T> struct is_const_impl<T const> : true_type {};

} // anonymous namespace

/// Boolean constant of true if T is const.
template<class T>
using is_const = is_const_impl<T>;

namespace {

template<class T> struct remove_ptr_impl { using type = T; };
template<class T> struct remove_ptr_impl<T*> { using type = T; };
template<class T> struct remove_ptr_impl<T* const> { using type = T; };
template<class T> struct remove_ptr_impl<T* volatile> { using type = T; };
template<class T> struct remove_ptr_impl<T* const volatile> { using type = T; };

} // anonymous namespace

/// Remove pointer qualification from type.
///
/// If the pointer type is qualified, they are also removed.
template<class T>
using remove_ptr = typename remove_ptr_impl<T>::type;

namespace {

template<class T> struct remove_ref_impl { using type = T; };
template<class T> struct remove_ref_impl<T&> { using type = T; };
template<class T> struct remove_ref_impl<T&&> { using type = T; };

} // anonymous namespace

/// Remove reference (lvalue or rvalue) qualification from type.
template<class T>
using remove_ref = typename remove_ref_impl<T>::type;

namespace {

template<class T> struct remove_cv_impl { using type = T; };
template<class T> struct remove_cv_impl<T const> { using type = T; };
template<class T> struct remove_cv_impl<T volatile> { using type = T; };
template<class T> struct remove_cv_impl<T const volatile> { using type = T; };

} // anonymous namespace

/// Remove const and volatile qualifications from type.
template<class T>
using remove_cv = typename remove_cv_impl<T>::type;

/// Remove const, volatile, and reference qualifications from type.
template<class T>
using remove_cvr = remove_cv<remove_ref<T>>;

namespace {

template<class T> struct add_rvalue_ref_impl { using type = T&&; };
template<> struct add_rvalue_ref_impl<void> { using type = void; };
template<> struct add_rvalue_ref_impl<void const> { using type = void const; };
template<> struct add_rvalue_ref_impl<void volatile> { using type = void volatile; };
template<> struct add_rvalue_ref_impl<void const volatile> { using type = void const volatile; };

} // anonymous namespace

/// Add an rvalue reference to type.
template<class T>
using add_rvalue_ref = typename add_rvalue_ref_impl<T>::type;

namespace {

template<class T, class U> struct is_same_impl : false_type {};
template<class T> struct is_same_impl<T, T> : true_type {};

} // anonymous namespace

/// Boolean constant of true if T is the same as U.
template<class T, class U>
using is_same = is_same_impl<T, U>;

namespace {

template<class T> struct is_pointer_impl : false_type {};
template<class T> struct is_pointer_impl<T*> : true_type {};

} // anonymous namespace

/// Boolean constant of true if T is a pointer.
template<class T>
using is_pointer = is_pointer_impl<remove_cv<T>>;

namespace {

template<class T> struct is_lvalue_reference_impl : false_type {};
template<class T> struct is_lvalue_reference_impl<T&> : true_type {};

template<class T> struct is_rvalue_reference_impl : false_type {};
template<class T> struct is_rvalue_reference_impl<T&&> : true_type {};

} // anonymous namespace

/// Boolean constant of true if T is an lvalue reference.
template<class T>
using is_lvalue_reference = is_lvalue_reference_impl<T>;

/// Boolean constant of true if T is an rvalue reference.
template<class T>
using is_rvalue_reference = is_rvalue_reference_impl<T>;

/// Boolean constant of true if T is an lvalue or rvalue reference.
template<class T>
using is_reference = constant_type<bool,
	is_lvalue_reference<T>::value ||
	is_rvalue_reference<T>::value
>;

namespace {

template<class T>
struct is_signed_impl : false_type {};

#define IMPL_SPECIALIZE(T) \
	template<> struct is_signed_impl<T> : true_type {} //

IMPL_SPECIALIZE(signed char);
IMPL_SPECIALIZE(signed short);
IMPL_SPECIALIZE(signed int);
IMPL_SPECIALIZE(signed long);
IMPL_SPECIALIZE(signed long long);

#undef IMPL_SPECIALIZE

} // anonymous namespace

/// Boolean constant of true if T is a signed integral.
template<class T>
using is_signed = is_signed_impl<remove_cv<T>>;

namespace {

template<class T>
struct is_unsigned_impl : false_type {};

#define IMPL_SPECIALIZE(T) \
	template<> struct is_unsigned_impl<T> : true_type {} //

IMPL_SPECIALIZE(unsigned char);
IMPL_SPECIALIZE(unsigned short);
IMPL_SPECIALIZE(unsigned int);
IMPL_SPECIALIZE(unsigned long);
IMPL_SPECIALIZE(unsigned long long);

#undef IMPL_SPECIALIZE

} // anonymous namespace

/// Boolean constant of true if T is an unsigned integral.
template<class T>
using is_unsigned = is_unsigned_impl<remove_cv<T>>;

namespace {

template<class T>
struct is_floating_point_impl : false_type {};

#define IMPL_SPECIALIZE(T) \
	template<> struct is_floating_point_impl<T> : true_type {} //

IMPL_SPECIALIZE(float);
IMPL_SPECIALIZE(double);
IMPL_SPECIALIZE(long double);

#undef IMPL_SPECIALIZE

} // anonymous namespace

/// Boolean constant of true if T is a floating-point type.
template<class T>
using is_floating_point = is_floating_point_impl<remove_cv<T>>;

/// Boolean constant of true if T is an integral type.
///
/// Contrary to the stdlib, this does not consider bool and character
/// types other than char to be integral.
template<class T>
using is_integral = constant_type<
	bool,
	is_same<remove_cv<T>, char>::value ||
	is_unsigned<T>::value ||
	is_signed<T>::value
>;

/// Boolean constant of true if T is an integral or floating-point type.
template<class T>
using is_arithmetic = constant_type<
	bool,
	is_integral<T>::value ||
	is_floating_point<T>::value
>;

/// @}

/// Enum-class bitwise operator enabler.
///
/// Specialize this class deriving from true_type to enable bit-wise
/// operators for an enum-class.
template<class>
struct enable_enum_bitwise_ops : false_type {};

/// Allow a type to be used in a collection.
///
/// Specialize this class deriving from true_type to allow a non-POD
/// type to be used in a collection.
template<class>
struct allow_collection_value_type : false_type {};

/** @} */ // end of doc-group lib_core_utility

} // namespace togo
