#line 2 "togo/image/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Image types.
@ingroup lib_image_types

@defgroup lib_image_types Types
@ingroup lib_image
@details
*/

#pragma once

#include <togo/image/config.hpp>
#include <togo/core/types.hpp>

namespace togo {

/**
	@addtogroup lib_image_types
	@{
*/

/// Color.
struct Color {
	/// Components.
	u8 r, g, b, a;

	/// Construct from components.
	Color(u8 r, u8 g, u8 b, u8 a = 255)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}

	/// Construct from packed.
	Color(u32 p)
		: r(p & 0xFF)
		, g((p >> 8 ) & 0xFF)
		, b((p >> 16) & 0xFF)
		, a((p >> 24) & 0xFF)
	{}

	/// Get components as packed value.
	u32 packed() const {
		return (r) | (g << 8) | (b << 16) | (a << 24);
	}

	/// Set components from packed value.
	void from_packed(u32 p) {
		r = p & 0xFF;
		g = (p >> 8 ) & 0xFF;
		b = (p >> 16) & 0xFF;
		a = (p >> 24) & 0xFF;
	}
};

/** @} */ // end of doc-group lib_image_types

} // namespace togo
