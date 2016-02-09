#line 2 "togo/core/hash/hash.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Hash interface.
@ingroup lib_core_hash
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/hash/types.hpp>

// TODO: Build constexpr version of MurmurHash2 or MurmurHash3
// (both lengths) and use it instead of FNV-1a

namespace togo {

namespace hash {

/**
	@addtogroup lib_core_hash
	@{
*/

/// Number of bytes consumed.
template<class H>
inline unsigned size(H const& s) {
	return s.size;
}

/// Whether any bytes have been consumed.
template<class H>
inline bool any(H const& s) {
	return hash::size(s) > 0;
}

/// Whether no bytes have been consumed.
template<class H>
inline bool empty(H const& s) {
	return hash::size(s) == 0;
}

namespace {

template<hash::Size S>
struct FNVInternals;

template<>
struct FNVInternals<hash::HS32> {
	static constexpr u32 const prime = 0x01000193;
	static constexpr u32 const offset_basis = 0x811c9dc5;
};

template<>
struct FNVInternals<hash::HS64> {
	static constexpr u64 const prime = 0x00000100000001b3;
	static constexpr u64 const offset_basis = 0xcbf29ce484222325;
};

} // anonymous namespace

/// Initialize hasher.
template<hash::Size S>
inline FNV1a<S>::FNV1a() {
	init(*this);
}

/// Initialize hasher.
template<hash::Size S>
inline void init(FNV1a<S>& s) {
	s.value = FNVInternals<S>::offset_basis;
	s.size = 0;
}

/// Add bytes to hasher.
template<hash::Size S>
inline void add(
	FNV1a<S>& s,
	u8 const* const data,
	unsigned const size
) {
	for (unsigned i = 0; i < size; ++i) {
		s.value ^= data[i];
		s.value *= FNVInternals<S>::prime;
	}
	s.size += size;
}

/// Value of hasher.
template<hash::Size S>
inline typename FNV1a<S>::Value value(FNV1a<S> const& s) {
	return hash::empty(s) ? FNV1a<S>::identity : s.value;
}

template<hash::Size S>
inline typename FNV1a<S>::Value FNV1a<S>::calc(
	u8 const* const data,
	unsigned const size
) {
	FNV1a<S> s;
	hash::init(s);
	hash::add(s, data, size);
	return hash::value(s);
}

template<hash::Size S>
constexpr typename FNV1a<S>::Value FNV1a<S>::calc_ce_seq(
	char const* const data,
	unsigned const size,
	unsigned const index,
	typename FNV1a<S>::Value const value
) {
	return (index < size)
		? calc_ce_seq(
			data, size,
			index + 1u,
			(value ^ data[index]) * FNVInternals<S>::prime
		)
		: value
	;
}

template<hash::Size S>
constexpr typename FNV1a<S>::Value FNV1a<S>::calc_ce(
	char const* const data,
	unsigned const size
) {
	return size == 0
		? FNV1a<S>::identity
		: calc_ce_seq(data, size, 0, FNVInternals<S>::offset_basis)
	;
}

/// Add string to hasher.
template<class H>
inline void add(
	H& s,
	char const* const data,
	unsigned size
) {
	hash::add(s, reinterpret_cast<u8 const*>(data), size);
}

/// Add string to hasher.
template<class H>
inline void add(H& s, StringRef const& str) {
	hash::add(s, str.data, str.size);
}

/// Calculate hash.
template<class H>
inline typename H::Value calc(
	char const* const data,
	unsigned const size
) {
	return H::calc(reinterpret_cast<u8 const*>(data), size);
}

/// Calculate hash.
template<class H>
inline typename H::Value calc(StringRef const& ref) {
	return H::calc(reinterpret_cast<u8 const*>(ref.data), ref.size);
}

/// Calculate hash.
template<class H>
inline constexpr typename H::Value calc_ce(
	char const* const data,
	unsigned const size
) {
	return H::calc_ce(data, size);
}

/// Calculate hash.
template<class H>
inline constexpr typename H::Value calc_ce(StringRef const& ref) {
	return H::calc_ce(ref.data, ref.size);
}

/// Calculate 32-bit hash.
inline hash32 calc32(
	char const* const data,
	unsigned const size
) {
	return calc<Default32>(data, size);
}

/// Calculate 32-bit hash from string reference.
inline hash32 calc32(StringRef const& ref) {
	return calc<Default32>(ref);
}

/// Calculate 32-bit hash (constexpr).
inline constexpr hash32 calc32_ce(
	char const* const data,
	unsigned const size
) {
	return calc_ce<Default32>(data, size);
}

/// Calculate 32-bit hash from string reference (constexpr).
inline constexpr hash32 calc32_ce(StringRef const& ref) {
	return calc_ce<Default32>(ref);
}

/// Calculate 64-bit hash.
inline hash64 calc64(
	char const* const data,
	unsigned const size
) {
	return calc<Default64>(data, size);
}

/// Calculate 64-bit hash from string reference.
inline hash64 calc64(StringRef const& ref) {
	return calc<Default64>(ref);
}

/// Calculate 64-bit hash (constexpr).
inline constexpr hash64 calc64_ce(
	char const* const data,
	unsigned const size
) {
	return calc_ce<Default64>(data, size);
}

/// Calculate 64-bit hash from string reference (constexpr).
inline constexpr hash64 calc64_ce(StringRef const& ref) {
	return calc_ce<Default64>(ref);
}

/** @} */ // end of doc-group lib_core_hash

} // namespace hash

/// 32-bit hash literal.
inline constexpr hash32
operator"" _hash32(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// 64-bit hash literal.
inline constexpr hash64
operator"" _hash64(
	char const* const data,
	std::size_t const size
) {
	return hash::calc64_ce(data, size);
}

} // namespace togo
