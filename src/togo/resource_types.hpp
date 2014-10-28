#line 2 "togo/resource_types.hpp"
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
#include <togo/traits.hpp>
#include <togo/string_types.hpp>
#include <togo/hash.hpp>
#include <togo/memory_types.hpp>
#include <togo/collection_types.hpp>
#include <togo/io_types.hpp>
#include <togo/file_io.hpp>

namespace togo {

/**
	@addtogroup resource
	@{
*/

/// Resource type.
using ResourceType = hash32;

/// Resource name hash.
using ResourceNameHash = hash64;

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

	/// gfx::RenderConfig.
	RES_TYPE_RENDER_CONFIG = "render_config"_resource_type,
};

/// Resource names.
enum : ResourceNameHash {
	/// Null name.
	RES_NAME_NULL = ""_resource_name,
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
	hash64 tags_collated;
	StringRef type{};
	StringRef name{};
	FixedArray<Tag, 8> tags;
};

// Forward declarations
struct ResourcePackage;
struct ResourceHandler;
struct ResourceManager;

/**
	@addtogroup resource_package
	@{
*/

/// Resource package.
struct ResourcePackage {
	struct Entry {
		ResourceType type;
		u8 path_size;
		char path[256];
	};

	using EntryNode = HashMapNode<ResourceNameHash, Entry>;

	hash64 _root_hash;
	FileReader _data_stream;
	HashMap<ResourceNameHash, Entry> _entries;
	FixedArray<char, 128> _root;

	ResourcePackage() = delete;
	ResourcePackage(ResourcePackage const&) = delete;
	ResourcePackage(ResourcePackage&&) = delete;
	ResourcePackage& operator=(ResourcePackage const&) = delete;
	ResourcePackage& operator=(ResourcePackage&&) = delete;

	~ResourcePackage() = default;

	ResourcePackage(
		StringRef const& root,
		Allocator& allocator
	);
};

/** @} */ // end of doc-group resource_package

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

	ResourceManager() = delete;
	ResourceManager(ResourceManager const&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager const&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

	~ResourceManager();
	ResourceManager(Allocator& allocator);
};

/** @} */ // end of doc-group resource_manager

/** @} */ // end of doc-group resource

} // namespace togo
