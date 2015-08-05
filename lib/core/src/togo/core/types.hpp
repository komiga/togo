#line 2 "togo/core/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Core types.
@ingroup lib_core_types

@defgroup lib_core_types Types
@ingroup lib_core
@details
*/

#pragma once

#include <togo/core/config.hpp>

#include <cstddef>
#include <cstdint>

namespace togo {

/**
	@addtogroup lib_core_types
	@{
*/

namespace basic_types {

/** @name Signed integral types */ /// @{
/// Fixed-width signed integral.
using s8		= std::int8_t;
using s16		= std::int16_t;
using s32		= std::int32_t;
using s32_fast	= std::int_fast32_t;
using s64		= std::int64_t;
using s64_fast	= std::int_fast64_t;
/// @}

/** @name Unsigned integral types */ /// @{
/// Fixed-width unsigned integral.
using u8		= std::uint8_t;
using u16		= std::uint16_t;
using u32		= std::uint32_t;
using u32_fast	= std::uint_fast32_t;
using u64		= std::uint64_t;
using u64_fast	= std::uint_fast64_t;
/// @}

/** @name Floating-point types */ /// @{
/// Fixed-width floating-point.
using f32 = float;
using f64 = double;
/// @}

/** @name Hash types */ /// @{
/// 32-bit hash.
using hash32 = u32;
/// 64-bit hash.
using hash64 = u64;
/// @}

} // namespace basic_types

using namespace basic_types;

/** @} */ // end of doc-group lib_core_types

} // namespace togo
