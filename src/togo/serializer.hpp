#line 2 "togo/serializer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Serializer interface.
@ingroup serialization
@ingroup serializer
*/

#pragma once

#include <togo/config.hpp>
#include <togo/traits.hpp>
#include <togo/serialization_types.hpp>

#include <type_traits>
#include <utility>

/** @cond INTERNAL */

namespace togo {
namespace detail {
	struct unimplemented {};
} // namespace detail
} // namespace togo

// Unimplemented captures (least-sticky acceptor)
template<class... P>
togo::detail::unimplemented serialize(togo::serializer_tag, P&&...);
template<class... P>
togo::detail::unimplemented read(togo::serializer_tag, P&&...);
template<class... P>
togo::detail::unimplemented write(togo::serializer_tag, P&&...);

/** @endcond */ // INTERNAL

namespace togo {

/**
	@addtogroup serializer
	@{
*/

namespace {

template<class Ser, class T>
struct has_serializer_serialize {
	using ser_type = remove_cvr<Ser>;
	using unimpl_capture_lvalue = is_same<
		detail::unimplemented,
		decltype(serialize(
			serializer_tag{},
			std::declval<ser_type&>(),
			std::declval<T&>()
		))
	>;
	using unimpl_capture_rvalue = is_same<
		detail::unimplemented,
		decltype(serialize(
			serializer_tag{},
			std::declval<ser_type&>(),
			std::declval<T&&>()
		))
	>;
	static constexpr bool const value =
		!unimpl_capture_lvalue::value ||
		!unimpl_capture_rvalue::value
	;
};

template<class Ser, class T>
struct has_serializer_read {
	using ser_type = remove_cvr<Ser>;
	using unimpl_capture_lvalue = is_same<
		detail::unimplemented,
		decltype(read(
			serializer_tag{},
			std::declval<ser_type&>(),
			std::declval<T&>()
		))
	>;
	using unimpl_capture_rvalue = is_same<
		detail::unimplemented,
		decltype(read(
			serializer_tag{},
			std::declval<ser_type&>(),
			std::declval<T&&>()
		))
	>;
	static constexpr bool const value =
		!unimpl_capture_lvalue::value ||
		!unimpl_capture_rvalue::value
	;
};

template<class Ser, class T>
struct has_serializer_write {
	using ser_type = remove_cvr<Ser>;
	using unimpl_capture = is_same<
		detail::unimplemented,
		decltype(write(
			serializer_tag{},
			std::declval<ser_type&>(),
			std::declval<T const&>()
		))
	>;
	static constexpr bool const value = !unimpl_capture::value;
};

template<class Ser, class T>
inline constexpr unsigned serializer_read_num_defined() {
	return
		has_serializer_read<Ser, T>::value +
		has_serializer_serialize<Ser, T>::value
	;
}

template<class Ser, class T>
inline constexpr unsigned serializer_write_num_defined() {
	return
		has_serializer_write<Ser, T>::value +
		has_serializer_serialize<Ser, T>::value
	;
}

} // anonymous namespace

namespace serializer {

template<class Ser, class T>
inline enable_if<has_serializer_serialize<Ser, T>::value>
read(Ser& ser, T&& value) {
	serialize(serializer_tag{}, ser, std::forward<T>(value));
}

template<class Ser, class T>
inline enable_if<has_serializer_read<Ser, T>::value>
read(Ser& ser, T&& value) {
	read(serializer_tag{}, ser, std::forward<T>(value));
}

template<class Ser, class T>
inline enable_if<has_serializer_serialize<Ser, T>::value>
write(Ser& ser, T const& value) {
	serialize(serializer_tag{}, ser, const_cast<remove_cvr<T>&>(value));
}

template<class Ser, class T>
inline enable_if<has_serializer_write<Ser, T>::value>
write(Ser& ser, T const& value) {
	write(serializer_tag{}, ser, value);
}

} // namespace serializer

// BaseSerializer implementation

template<class Impl>
inline Impl& BaseSerializer<Impl>::impl() {
	return static_cast<Impl&>(*this);
}

// InputSerializer implementation

/// Read value.
template<class Impl, class T>
inline enable_if<serializer_read_num_defined<Impl, T>() == 1, Impl&>
operator%(
	InputSerializer<Impl>& ser,
	T&& value
) {
	serializer::read(ser.impl(), std::forward<T>(value));
	return ser.impl();
}

/** @cond INTERNAL */
template<class Impl, class T>
inline enable_if<serializer_read_num_defined<Impl, T>() != 1, Impl&>
operator%(
	InputSerializer<Impl>& ser,
	T&& value
) {
	// To show call deduction failures
	(void)ser; (void)value;
	#if defined(TOGO_DEBUG)
		serializer::read(ser.impl(), std::forward<T>(value));
		return ser.impl();
	#endif

	static_assert(
		serializer_read_num_defined<Impl, T>() != 0,
		"no read-serialize function defined for T with this serializer"
	);
	static_assert(
		serializer_read_num_defined<Impl, T>() <= 1,
		"ambiguous serialization interface defined for T with this serializer"
		" (both read() and serialize() are defined)"
	);
}
/** @endcond */ // INTERNAL

// OutputSerializer implementation

/// Write value.
template<class Impl, class T>
inline enable_if<serializer_write_num_defined<Impl, T>() == 1, Impl&>
operator%(
	OutputSerializer<Impl>& ser,
	T const& value
) {
	serializer::write(ser.impl(), value);
	return ser.impl();
}

/** @cond INTERNAL */
template<class Impl, class T>
inline enable_if<serializer_write_num_defined<Impl, T>() != 1, Impl&>
operator%(
	OutputSerializer<Impl>& ser,
	T const& value
) {
	// To show call deduction failures
	(void)ser; (void)value;
	#if defined(TOGO_DEBUG)
		serializer::write(ser.impl(), value);
		return ser.impl();
	#endif

	static_assert(
		serializer_write_num_defined<Impl, T>() != 0,
		"no write-serialize function defined for T with this serializer"
	);
	static_assert(
		serializer_write_num_defined<Impl, T>() <= 1,
		"ambiguous serialization interface defined for T with this serializer"
		" (both write() and serialize() are defined)"
	);
}
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serializer

} // namespace togo
