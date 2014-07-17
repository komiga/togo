#line 2 "togo/gfx/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file gfx/types.hpp
@brief Graphics types.
@ingroup types
@ingroup gfx
*/

#pragma once

#include <togo/config.hpp>
#include <togo/traits.hpp>

namespace togo {
namespace gfx {

/**
	@addtogroup gfx
	@{
*/

/// Graphics configuration flags.
enum class ConfigFlags : unsigned {
	/// Empty flag.
	none = 0,
	/// Double-buffered display.
	double_buffered = 1 << 0,
};

/// Graphics configuration.
///
/// If either msaa_num_buffers or msaa_num_samples are 0, MSAA is
/// disabled.
struct Config {
	struct ColorBits {
		unsigned red;
		unsigned green;
		unsigned blue;
		unsigned alpha;
	};

	/// Color buffer bits.
	gfx::Config::ColorBits color_bits;

	/// Depth buffer bits.
	/// If this is 0, the depth buffer should be assumed unavailable.
	unsigned depth_bits;

	/// Stencil buffer bits.
	/// If this is 0, the stencil buffer should be assumed unavailable.
	unsigned stencil_bits;

	/// Number of MSAA buffers.
	unsigned msaa_num_buffers;
	/// Number of MSAA samples.
	unsigned msaa_num_samples;

	/// Miscellaneous flags.
	gfx::ConfigFlags flags;
};

/**
	@addtogroup gfx_display
	@{
*/

/**
	Display flags.
*/
enum class DisplayFlags : unsigned {
	/// Empty flag.
	none = 0,
	/// Center window.
	centered = 1 << 0,
	/// Border-less window.
	borderless = 1 << 1,
	/// Full-screen window.
	fullscreen = 1 << 2,
	/// Re-sizable window.
	resizable = 1 << 3,
};

/// Graphics display.
struct Display;

/** @} */ // end of doc-group gfx_display

/**
	@addtogroup gfx_context
	@{
*/

/**
	Context flags.
*/
enum class ContextFlags : unsigned {
	/// Empty flag.
	none = 0,
	/// Shared context.
	shared = 1 << 0,
};

/// Graphics context.
struct Context;

/** @} */ // end of doc-group gfx_context

/** @} */ // end of doc-group gfx

} // namespace gfx

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<gfx::ConfigFlags> : true_type {};

template<>
struct enable_enum_bitwise_ops<gfx::DisplayFlags> : true_type {};

template<>
struct enable_enum_bitwise_ops<gfx::ContextFlags> : true_type {};
/** @endcond */ // INTERNAL

} // namespace togo
