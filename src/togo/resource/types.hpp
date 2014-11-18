#line 2 "togo/resource/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Resource types.
@ingroup types
@ingroup resource
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/utility/traits.hpp>
#include <togo/string/types.hpp>
#include <togo/hash/types.hpp>
#include <togo/hash/hash.hpp>
#include <togo/memory/types.hpp>
#include <togo/collection/types.hpp>
#include <togo/io/types.hpp>
#include <togo/io/file_stream.hpp>

namespace togo {

/**
	@addtogroup resource
	@{
*/

/// Format versions.
enum : u32 {
	/// TestResource format version.
	SER_FORMAT_VERSION_TEST_RESOURCE = 1,

	/// ResourcePackage manifest format version.
	SER_FORMAT_VERSION_PKG_MANIFEST = 3,
};

/// Resource type.
using ResourceType = hash32;

/// Resource name hash.
using ResourceNameHash = hash64;

/// Combined resource tags hash.
using ResourceTagsHash = hash64;

/// Combined resource tags hash combiner.
using ResourceTagsHashCombiner = HashCombiner64;

/// Package name hash.
using ResourcePackageNameHash = hash32;

/** @cond INTERNAL */
static_assert(
	is_same<ResourceType, hash32>::value,
	"changed ResourceType type breaks binary formats,"
	" hash functions, and likely other things"
);
static_assert(
	is_same<ResourceNameHash, hash64>::value,
	"changed ResourceNameHash type breaks binary formats,"
	" hash functions, and likely other things"
);
static_assert(
	is_same<ResourceTagsHash, hash64>::value,
	"changed ResourceTagsHash type breaks binary formats,"
	" hash functions, and likely other things"
);
/** @endcond */ // INTERNAL

/// Resource type hash literal.
inline constexpr ResourceType
operator"" _resource_type(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// Resource name hash literal.
inline constexpr ResourceNameHash
operator"" _resource_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc64_ce(data, size);
}

/// Combined resource tags hash literal.
///
/// Note that this only takes a single string. Tags should be sorted
/// and separator-less for the return value of this literal to be
/// compatible with the runtime combiner-based hash function.
inline constexpr ResourceTagsHash
operator"" _resource_tags(
	char const* const data,
	std::size_t const size
) {
	return hash::calc64_ce(data, size);
}

/// Package name hash literal.
inline constexpr ResourcePackageNameHash
operator"" _resource_package_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// Resource types.
enum : ResourceType {
	/// Non-type.
	RES_TYPE_NULL = ""_resource_type,

	/// TestResource.
	RES_TYPE_TEST = "test"_resource_type,

	/// gfx::RenderConfig.
	RES_TYPE_RENDER_CONFIG = "render_config"_resource_type,
};

/// Resource names.
enum : ResourceNameHash {
	/// Null name.
	RES_NAME_NULL = ""_resource_name,
};

/// Combined resource tags.
enum : ResourceTagsHash {
	/// Null name.
	RES_TAGS_NULL = ""_resource_tags,
};

/// Resource package names.
enum : ResourcePackageNameHash {
	/// Null name.
	PKG_NAME_NULL = ""_resource_package_name,
};

/// Resource path parts.
struct ResourcePathParts {
	struct Tag {
		StringRef name{};
		hash32 hash;
	};

	ResourceType type_hash;
	ResourceNameHash name_hash;
	ResourceTagsHash tags_hash;
	StringRef type{};
	StringRef name{};
	FixedArray<Tag, 8> tags;
};

/// Path to compiled resource.
struct ResourceCompiledPath {
	u32 id;
	FixedArray<char, 24> _data{};

	~ResourceCompiledPath() = default;

	ResourceCompiledPath(ResourceCompiledPath const&) = delete;
	ResourceCompiledPath(ResourceCompiledPath&&) = delete;
	ResourceCompiledPath& operator=(ResourceCompiledPath const&) = delete;
	ResourceCompiledPath& operator=(ResourceCompiledPath&&) = delete;

	ResourceCompiledPath();

	operator StringRef() const;

	u32 size() const;
	char const* data() const;
};

/// Resource metadata.
struct ResourceMetadata {
	u32 id;

	// Serial
	ResourceNameHash name_hash;
	ResourceTagsHash tags_hash;
	ResourceType type;
	u32 data_format_version;
	u32 data_offset;
	u32 data_size;
};

/// Test resource.
struct TestResource {
	s64 x;
};

// Forward declarations
struct ResourceHandler;
struct ResourcePackage;
struct ResourceManager;

/**
	@addtogroup resource_handler
	@{
*/

/// Resource handler.
struct ResourceHandler {
	/// Load a resource.
	///
	/// Returns pointer to resource, or nullptr on error.
	using load_func_type = void* (
		void* /*type_data*/,
		ResourceManager& /*manager*/,
		ResourceNameHash /*name_hash*/,
		IReader& /*stream*/
	);

	/// Unload a resource.
	using unload_func_type = void (
		void* /*type_data*/,
		ResourceManager& /*manager*/,
		ResourceNameHash /*name_hash*/,
		void* /*resource*/
	);

	ResourceType type;
	load_func_type* func_load;
	unload_func_type* func_unload;
};

/** @} */ // end of doc-group resource_handler

/**
	@addtogroup resource_package
	@{
*/

/// Resource package.
struct ResourcePackage {
	using LookupNode = HashMapNode<ResourceNameHash, u32>;

	ResourcePackageNameHash _name_hash;
	u32 _open_resource_id;
	FileReader _stream;
	HashMap<ResourceNameHash, u32> _lookup;
	Array<ResourceMetadata> _manifest;
	FixedArray<char, 48> _name;
	FixedArray<char, 256> _path;

	ResourcePackage() = delete;
	ResourcePackage(ResourcePackage const&) = delete;
	ResourcePackage(ResourcePackage&&) = delete;
	ResourcePackage& operator=(ResourcePackage const&) = delete;
	ResourcePackage& operator=(ResourcePackage&&) = delete;

	~ResourcePackage() = default;

	ResourcePackage(
		StringRef const& name,
		StringRef const& path,
		Allocator& allocator
	);
};

/** @} */ // end of doc-group resource_package

/**
	@addtogroup resource_manager
	@{
*/

/// Resource manager.
struct ResourceManager {
	struct BoundHandler {
		ResourceHandler handler;
		void* type_data;
	};

	struct TypedResource {
		void* value;
		ResourceType type;
	};

	HashMap<ResourceType, BoundHandler> _handlers;
	HashMap<ResourceNameHash, TypedResource> _resources;
	Array<ResourcePackage*> _packages;
	FixedArray<char, 128> _base_path;

	ResourceManager() = delete;
	ResourceManager(ResourceManager const&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager const&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

	~ResourceManager();
	ResourceManager(
		StringRef const base_path,
		Allocator& allocator
	);
};

/** @} */ // end of doc-group resource_manager

/** @} */ // end of doc-group resource

} // namespace togo
