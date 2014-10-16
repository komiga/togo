#line 2 "togo/filesystem_types.hpp"
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
#include <togo/collection_types.hpp>
#include <togo/string_types.hpp>

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
	WorkingDirScope(WorkingDirScope const&) = delete;
	WorkingDirScope& operator=(WorkingDirScope const&) = delete;
	WorkingDirScope(WorkingDirScope&&) = delete;
	WorkingDirScope& operator=(WorkingDirScope&&) = delete;

	~WorkingDirScope();
	WorkingDirScope(StringRef const& path);
};

/** @} */ // end of doc-group filesystem

} // namespace togo
