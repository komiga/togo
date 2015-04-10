#line 2 "togo/core/utility/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Utility types.
@ingroup lib_core_utility
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_core_utility
	@{
*/

/// Endianness.
enum class Endian : unsigned {
	/// Little endian.
	little = TOGO_ENDIAN_LITTLE,
	/// Big endian.
	big = TOGO_ENDIAN_BIG,
	/// System endian.
	system = TOGO_ENDIAN_SYSTEM,
};

/// Array reference.
template<class T>
struct ArrayRef;

/** @} */ // end of doc-group lib_core_utility

} // namespace togo
