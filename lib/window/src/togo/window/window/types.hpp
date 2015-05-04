#line 2 "togo/window/window/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Window types.
@ingroup lib_window_types
@ingroup lib_window_window
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/utility.hpp>

namespace togo {

/**
	@addtogroup lib_window_window
	@{
*/

/// Window OpenGL configuration.
///
/// If either msaa_num_buffers or msaa_num_samples are 0, MSAA is
/// disabled.
struct WindowOpenGLConfig {
	/// Flags.
	enum class Flags : unsigned {
		/// Empty flag.
		none = 0,
		/// Double-buffered window.
		double_buffered = 1 << 0,
	};

	struct ColorBits {
		unsigned red;
		unsigned green;
		unsigned blue;
		unsigned alpha;
	};

	/// Color buffer bits.
	ColorBits color_bits;

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
	Flags flags;
};

/// Window flags.
enum class WindowFlags : unsigned {
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

	owned_by_input_buffer = 1 << 4,
};

/// Window swap modes.
enum class WindowSwapMode : unsigned {
	/// No screen synchronization.
	immediate = 0,

	/// Wait for at least one refresh cycle before swapping.
	wait_refresh,
};

/// Window.
struct Window;

/** @} */ // end of doc-group lib_window_window

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<WindowOpenGLConfig::Flags> : true_type {};

template<>
struct enable_enum_bitwise_ops<WindowFlags> : true_type {};
/** @endcond */ // INTERNAL

} // namespace togo
