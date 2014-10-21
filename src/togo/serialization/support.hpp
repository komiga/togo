#line 2 "togo/serialization/support.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Serialization support.
@ingroup serialization
*/

#pragma once

#include <togo/config.hpp>
#include <togo/traits.hpp>
#include <togo/serialization_types.hpp>

#include <type_traits>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

// traits and utilities

/// Boolean constant of true if Ser is an input serializer.
template<class Ser>
struct is_input_serializer
	: constant_type<
		bool,
		std::is_base_of<
			InputSerializer<remove_cvr<Ser>>,
			remove_cvr<Ser>
		>::value
	>
{};

/// Boolean constant of true if Ser is an output serializer.
template<class Ser>
struct is_output_serializer
	: constant_type<
		bool,
		std::is_base_of<
			OutputSerializer<remove_cvr<Ser>>,
			remove_cvr<Ser>
		>::value
	>
{};

/// Boolean constant of true if Ser is a serializer.
template<class Ser>
struct is_serializer
	: constant_type<
		bool,
		is_input_serializer <Ser>::value ||
		is_output_serializer<Ser>::value
	>
{};

/// Binary serialization enabler.
///
/// Specialize this class deriving from true_type to enable
/// direct-copy binary serialization for a type.
template<class>
struct enable_binary_serialization : false_type {};

/// Boolean constant of true if binary serialization of T was
/// explicitly enabled.
template<class T>
using is_binary_serializable_explicitly = enable_binary_serialization<remove_cv<T>>;

/// Boolean constant of true if T is arithmetic or explicitly
/// binary serializable.
template<class T>
using is_binary_serializable = constant_type<
	bool,
	is_arithmetic<T>::value ||
	is_binary_serializable_explicitly<T>::value
>;

/// Make serialize() value const-safe by serializer type.
template<class Ser, class T>
inline type_if<is_output_serializer<Ser>::value, T const&, T&>
serializer_cast_safe(T& value) {
	static_assert(
		is_serializer<Ser>::value,
		"Ser must be a serializer"
	);

	return value;
}

/// Make serial buffer configuration.
template<class T>
inline SerBuffer<is_const<T>::value>
make_ser_buffer(T* const ptr, unsigned const size) {
	return {ptr, size};
}

/// Make serial proxy configuration.
///
/// S is used as the serial form of the value.
/// S must be an arithmetic value.
template<class S, class T>
inline SerProxy<S, T>
make_ser_proxy(T& value) {
	return {value};
}

/// Make serial sequence configuration.
template<class T>
inline SerSequence<T>
make_ser_sequence(T* const ptr, unsigned const size) {
	return {ptr, size};
}

/// Make serial collection configuration.
///
/// S is used as the serial form of the size of the collection.
/// S must be an unsigned integral.
template<class S, class T>
inline SerCollection<S, remove_cv<T>, is_const<T>::value>
make_ser_collection(T& value) {
	return {value};
}

/// Make serial string configuration.
///
/// S is used as the serial form of the size of the collection.
/// S must be an unsigned integral.
template<class S, class T>
inline SerString<S, remove_cv<T>, is_const<T>::value>
make_ser_string(T& value) {
	return {value};
}

/** @cond INTERNAL */

// SerProxy

template<class Ser, class S, class T>
inline void read(serializer_tag, Ser& ser, SerProxy<S, T>&& value) {
	S serial{};
	ser % serial;
	value.ref = static_cast<T>(serial);
}

template<class Ser, class S, class T>
inline void write(serializer_tag, Ser& ser, SerProxy<S, T> const& value) {
	ser % static_cast<S>(value.ref);
}

template<class Ser, class T>
inline enable_if<std::is_enum<T>::value, void>
serialize(serializer_tag, Ser& ser, T& value_unsafe) {
	using base_type = typename std::underlying_type<remove_cv<T>>::type;
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser % make_ser_proxy<base_type>(value);
}

// SerSequence

// NB: Binary-serializable sequences are handled specially in the
// serializer

template<class Ser, class T>
inline enable_if<!is_binary_serializable<T>::value, void>
serialize(serializer_tag, Ser& ser, SerSequence<T>& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	auto* const end = value.ptr + value.size;
	for (auto* it = value.ptr; it != end; ++it) {
		ser % *it;
	}
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serialization

} // namespace togo
