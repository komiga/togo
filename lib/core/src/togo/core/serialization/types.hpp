#line 2 "togo/core/serialization/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Serialization types.
@ingroup lib_core_types
@ingroup lib_core_serialization
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>

#include <type_traits>

namespace togo {

/**
	@addtogroup lib_core_serialization
	@{
*/

// Forward declarations
class IReader; // external
class IWriter; // external

/// Serializer function tag.
enum class serializer_tag {};

/// Serial configuration for a buffer.
template<bool C>
struct SerBuffer {
	using value_type = type_if<C, void const*, void*>;

	value_type ptr;
	unsigned size;
};

/// Serial configuration for an arithmetic or enum value.
template<class S, class T>
struct SerProxy {
	static_assert(
		is_arithmetic<S>::value,
		"serial type (S) must be an arithmetic type"
	);
	static_assert(
		is_arithmetic<T>::value ||
		std::is_enum<T>::value ||
		is_same<remove_cv<T>, bool>::value,
		"value type (T) must be an arithmetic, enum, or boolean type"
	);

	T& ref;
};

/// Serial configuration for a sequence.
template<class T>
struct SerSequence {
	static_assert(
		!is_same<void, remove_cv<T>>::value,
		"T cannot be void"
	);

	T* ptr;
	unsigned size;
};

/// Serial configuration for a collection.
template<class S, class T, bool C = false>
struct SerCollection {
	static_assert(
		is_unsigned<S>::value,
		"size type (S) must be an unsigned integral type"
	);

	using value_type = type_if<C, T const, T>;

	value_type& ref;
};

/// Serial configuration for a string.
///
/// This assumes string data is NUL-terminated. The max value of S
/// can be one less than the capacity of the string.
template<class S, class T, bool C = false>
struct SerString {
	static_assert(
		is_unsigned<S>::value,
		"size type (S) must be an unsigned integral type"
	);

	using value_type = type_if<C, T const, T>;

	value_type& ref;
};

/**
	@addtogroup lib_core_serializer
	@{
*/

/// Base serializer.
template<class Impl>
struct BaseSerializer {
	Impl& impl();
};

/// Base input serializer.
template<class Impl>
struct InputSerializer
	: BaseSerializer<Impl>
{};

/// Base output serializer.
template<class Impl>
struct OutputSerializer
	: BaseSerializer<Impl>
{};

/** @} */ // end of doc-group lib_core_serializer

/**
	@addtogroup lib_core_binary_serializer
	@{
*/

/// Binary input serializer.
struct BinaryInputSerializer
	: InputSerializer<BinaryInputSerializer>
{
	IReader& _stream;
	Endian _endian;

	BinaryInputSerializer() = delete;

	~BinaryInputSerializer() = default;
	BinaryInputSerializer(BinaryInputSerializer const&) = default;
	BinaryInputSerializer(BinaryInputSerializer&&) = default;
	BinaryInputSerializer& operator=(BinaryInputSerializer const&) = default;
	BinaryInputSerializer& operator=(BinaryInputSerializer&&) = default;

	BinaryInputSerializer(IReader& stream, Endian endian = Endian::little);
};

/// Binary output serializer.
struct BinaryOutputSerializer
	: OutputSerializer<BinaryOutputSerializer>
{
	IWriter& _stream;
	Endian _endian;

	BinaryOutputSerializer() = delete;

	~BinaryOutputSerializer() = default;
	BinaryOutputSerializer(BinaryOutputSerializer const&) = default;
	BinaryOutputSerializer(BinaryOutputSerializer&&) = default;
	BinaryOutputSerializer& operator=(BinaryOutputSerializer const&) = default;
	BinaryOutputSerializer& operator=(BinaryOutputSerializer&&) = default;

	BinaryOutputSerializer(IWriter& stream, Endian endian = Endian::little);
};

/** @} */ // end of doc-group lib_core_binary_serializer

/** @} */ // end of doc-group lib_core_serialization

} // namespace togo
