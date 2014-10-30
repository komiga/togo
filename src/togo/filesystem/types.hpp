#line 2 "togo/filesystem/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Filesystem types.
@ingroup types
@ingroup filesystem
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/utility/traits.hpp>
#include <togo/utility/utility.hpp>
#include <togo/collection/types.hpp>
#include <togo/string/types.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/filesystem/directory_reader/posix.hpp>
#else
	#error "missing DirectoryReader implementation for target platform"
#endif

namespace togo {

/**
	@addtogroup filesystem
	@{
*/

/// Scope-bound working directory assignment.
///
/// Operations in initialization and deinitialization trigger
/// assertions on failure.
struct WorkingDirScope {
	FixedArray<char, 256> _prev_path;

	WorkingDirScope() = delete;
	WorkingDirScope(WorkingDirScope&&) = delete;
	WorkingDirScope(WorkingDirScope const&) = delete;
	WorkingDirScope& operator=(WorkingDirScope&&) = delete;
	WorkingDirScope& operator=(WorkingDirScope const&) = delete;

	~WorkingDirScope();
	WorkingDirScope(StringRef const& path);
};

/// Directory entry.
struct DirectoryEntry {
	enum class Type {
		file = 1 << 0,
		dir  = 1 << 1,

		all = file | dir,
	};

	Type type;
	StringRef path;
};

/// Directory reader.
struct DirectoryReader {
	u32 _options;
	DirectoryReaderImpl _impl;

	DirectoryReader(DirectoryReader&&) = delete;
	DirectoryReader(DirectoryReader const&) = delete;
	DirectoryReader& operator=(DirectoryReader&&) = delete;
	DirectoryReader& operator=(DirectoryReader const&) = delete;

	DirectoryReader();
	~DirectoryReader();
};

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<DirectoryEntry::Type> : true_type {};
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group filesystem

} // namespace togo
