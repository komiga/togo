#line 2 "togo/core/config.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Core configuration.
@ingroup lib_core_config

@defgroup lib_core_config Configuration
@ingroup lib_core
@details
*/

#pragma once

// AM‽ configuration
#include <am/config_values.hpp>

/** @cond INTERNAL */
#ifdef AM_CONFIG_IMPLICIT_LINEAR_INTERFACE
	#undef AM_CONFIG_IMPLICIT_LINEAR_INTERFACE
#endif
#define AM_CONFIG_FLOAT_PRECISION AM_PRECISION_MEDIUM
#define AM_CONFIG_VECTOR_TYPES AM_FLAG_TYPE_FLOAT
#define AM_CONFIG_MATRIX_TYPES AM_FLAG_TYPE_FLOAT
/** @endcond */ // INTERNAL

#include <am/config.hpp>

namespace togo {

/**
	@addtogroup lib_core_config
	@{
*/

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// Whether to disable assertions.
	///
	/// This is enabled if it is defined.
	#define TOGO_DISABLE_ASSERTIONS

	/// Whether to enable debugging facilities.
	///
	/// This is enabled if it is defined. It is automatically enabled
	/// if DEBUG is defined or if NDEBUG is not defined.
	/// If assertions are enabled (default), debug assertions will be
	/// defined when this is enabled.
	#define TOGO_DEBUG

	/// Whether to use stdlib constraints to ensure types are being
	/// used properly.
	///
	/// This is enabled if it is defined.
	#define TOGO_USE_CONSTRAINTS

	/// Mark function as part of the generated interface when running igen.
	#define IGEN_INTERFACE

	/// Mark function as private part of the generated interface when
	/// running igen.
	///
	/// This hides the function from documentation.
	#define IGEN_PRIVATE

	/// Set parameter default value when running igen.
	///
	/// The default value will be kept only for the generated header.
	#define IGEN_DEFAULT(x)
#else
	#if !defined(TOGO_DEBUG) && (defined(DEBUG) || !defined(NDEBUG))
		#define TOGO_DEBUG
	#endif
	#if defined(IGEN_RUNNING)
		#define IGEN_INTERFACE __attribute__((annotate("igen_interface")))
		#define IGEN_PRIVATE __attribute__((annotate("igen_private")))
		#define IGEN_DEFAULT(x) __attribute__((annotate("igen_default:" #x)))
	#else
		#define IGEN_INTERFACE
		#define IGEN_PRIVATE
		#define IGEN_DEFAULT(x)
	#endif
#endif // defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)

/** @name System detection */ /// @{

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// Defined if the target platform is Linux.
	#define TOGO_PLATFORM_LINUX
	/// Defined if the target platform is MacOS.
	#define TOGO_PLATFORM_MACOS
	/// Defined if the target platform is Windows.
	#define TOGO_PLATFORM_WINDOWS

	/// Defined if the target platform is POSIX-compliant.
	///
	/// This is defined if #TOGO_PLATFORM_LINUX or #TOGO_PLATFORM_MACOS
	/// are defined.
	#define TOGO_PLATFORM_IS_POSIX

	/// Defined if the target processor is x86.
	#define TOGO_ARCH_X86
	/// Defined if the target processor is x86_64.
	#define TOGO_ARCH_X86_64
#else
	#if defined(__linux__)
		#define TOGO_PLATFORM_LINUX
	#elif defined(macintosh) || defined(Macintosh) || \\
		(defined(__APPLE__) && defined(__MACH__))

		#define TOGO_PLATFORM_MACOS
	#elif defined(_WIN32)
		#define TOGO_PLATFORM_WINDOWS
	#else
		#error "unrecognized platform"
	#endif

	#if defined(TOGO_PLATFORM_LINUX) || defined(TOGO_PLATFORM_MACOS)
		#define TOGO_PLATFORM_IS_POSIX
	#endif

	#if defined(__amd64__) || defined(__x86_64__) || \
		defined(_M_AMD64) || defined(_M_X64)

		#define TOGO_ARCH_X86_64
	#elif defined(__i386__) || defined(_M_IX86)
		#define TOGO_ARCH_X86
	#else
		#error "unrecognized processor architecture"
	#endif
#endif // defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)

/// @} // end of name-group System detection

/** @name Compiler detection */ /// @{

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// Defined if the compiler is Clang.
	#define TOGO_COMPILER_CLANG
	/// Defined if the compiler is GCC.
	#define TOGO_COMPILER_GCC
	/// Defined if the compiler is MSVC.
	#define TOGO_COMPILER_MSVC
#else
	#if defined(__clang__)
		#define TOGO_COMPILER_CLANG
	#elif defined(__GNUC__)
		#define TOGO_COMPILER_GCC
	#elif defined(_MSC_VER)
		#define TOGO_COMPILER_MSVC
	#else
		#error "unrecognized compiler"
	#endif
#endif // defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)

/// @} // end of name-group Compiler detection

/** @} */ // end of doc-group lib_core_config

} // namespace togo