#line 2 "togo/resource/resource.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Resource interface.
@ingroup resource
*/

#pragma once

#include <togo/config.hpp>
#include <togo/string/types.hpp>
#include <togo/hash/hash.hpp>
#include <togo/resource/types.hpp>

namespace togo {

namespace resource {

/**
	@addtogroup resource
	@{
*/

/// Calculate hash of resource type.
inline ResourceType hash_type(StringRef const& type) {
	return hash::calc32(type);
}

/// Calculate hash of resource name.
inline ResourceNameHash hash_name(StringRef const& name) {
	return hash::calc64(name);
}

/// Calculate hash of resource package name.
inline ResourcePackageNameHash hash_package_name(StringRef const& name) {
	return hash::calc32(name);
}

/// Parse resource path.
///
/// Tags are sorted by hash value.
/// Returns false if the given path is malformed.
bool parse_path(
	StringRef const& path,
	ResourcePathParts& pp
);

/// Get compiled path.
void compiled_path(
	ResourceCompiledPath& path,
	u32 id
);

/** @} */ // end of doc-group resource

} // namespace resource

inline ResourceCompiledPath::ResourceCompiledPath()
	: id(0)
	, _data()
{}

inline u32 ResourceCompiledPath::size() const {
	return id == 0 ? 0 : string::size(_data);
}

inline char const* ResourceCompiledPath::data() const {
	return fixed_array::begin(_data);
}

inline ResourceCompiledPath::operator StringRef() const {
	return {data(), size()};
}

} // namespace togo
