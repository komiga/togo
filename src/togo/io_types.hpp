#line 2 "togo/io_types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file io_types.hpp
@brief IO types.
@ingroup types
@ingroup io
*/

#pragma once

#include <togo/config.hpp>

namespace togo {

/**
	@addtogroup io
	@{
*/

// Forward declarations
class IReader;
class IWriter;

/// IO stream status.
struct IOStatus {
	enum : unsigned {
		/// No flags.
		flag_none = 0,
		/// Fail flag.
		flag_fail = 1 << 0,
		/// End-of-stream flag.
		flag_eof  = 1 << 1,
	};

	unsigned _value;

	/// No errors.
	constexpr bool ok() const noexcept {
		return !_value;
	}

	/// An operation failed.
	constexpr bool fail() const noexcept {
		return _value & flag_fail;
	}

	/// An operation failed and reached the end of the stream.
	///
	/// Writers that have a bound stream size (i.e., non-growing) may
	/// set this flag.
	constexpr bool eof() const noexcept {
		return _value & flag_eof;
	}

	/// Same as ok().
	constexpr operator bool() const noexcept {
		return ok();
	}
};

/** @} */ // end of doc-group io

} // namespace togo
