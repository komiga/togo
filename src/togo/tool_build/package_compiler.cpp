#line 2 "togo/tool_build/package_compiler.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/fixed_array.hpp>
#include <togo/array.hpp>
#include <togo/hash_map.hpp>
#include <togo/string.hpp>
#include <togo/filesystem.hpp>
#include <togo/file_io.hpp>
#include <togo/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/string.hpp>
#include <togo/serialization/array.hpp>
#include <togo/binary_serializer.hpp>
#include <togo/kvs.hpp>
#include <togo/resource.hpp>
#include <togo/tool_build/resource_metadata.hpp>
#include <togo/tool_build/package_compiler.hpp>

namespace togo {
namespace tool_build {

enum : u32 {
	PKG_MANIFEST_FORMAT_VERSION = 1u,
};

template<class Ser>
inline void read(serializer_tag, Ser& ser, ResourceMetadata& value) {
	ser % value.type;
	fixed_array::clear(value.path);
	if (value.type != RES_TYPE_NULL) {
		ser
			% value.format_version
			% value.name_hash
			% value.last_compiled
			% value.tags_collated
			% make_ser_string<u8>(value.path)
		;
	} else {
		// Empty slot
		value.format_version = 0;
		value.name_hash = RES_NAME_NULL;
		value.last_compiled = 0;
		value.tags_collated = hash::IDENTITY64;
	}
}

template<class Ser>
inline void write(serializer_tag, Ser& ser, ResourceMetadata const& value) {
	ser % value.type;
	if (value.type != RES_TYPE_NULL) {
		ser
			% value.format_version
			% value.name_hash
			% value.last_compiled
			% value.tags_collated
			% make_ser_string<u8>(value.path)
		;
	}
}

PackageCompiler::PackageCompiler(
	StringRef const& path,
	Allocator& allocator
)
	: _modified(false)
	, _name_hash(""_resource_package_name)
	, _lookup(allocator)
	, _metadata(allocator)
	, _name()
	, _path()
{
	string::copy(_path, path);
	string::trim_trailing_slashes(_path);
}

bool package_compiler::create_stub(
	StringRef const& path,
	StringRef const& name
) {
	TOGO_ASSERTE(path.any());
	TOGO_ASSERTE(name.any());

	bool const path_already_exists = filesystem::is_directory(path);
	if (!path_already_exists && !filesystem::create_directory(path)) {
		TOGO_LOG_ERRORF(
			"create_stub: failed to create package root at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}

	WorkingDirScope dir_scope{path};

	if (path_already_exists && filesystem::is_directory(".package")) {
		TOGO_LOG_ERRORF(
			"create_stub: .package already exists in '%.*s'\n",
			path.size, path.data
		);
		return false;
	}

	if (!filesystem::create_directory(".package")) {
		TOGO_LOG_ERRORF(
			"create_stub: failed to create .package directory in '%.*s'\n",
			path.size, path.data
		);
		return false;
	}

	if (!filesystem::create_directory(".compiled")) {
		TOGO_LOG_ERRORF(
			"create_stub: failed to create .compiled directory in '%.*s'\n",
			path.size, path.data
		);
		return false;
	}

	{// Create properties
	KVS k_properties{KVSType::node};
	kvs::push_back(k_properties, KVS{"name", name});
	if (!kvs::write_file(k_properties, ".package/properties")) {
		TOGO_LOG_ERRORF(
			"failed to create properties for package at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}}

	{// Create manifest
	FileWriter stream{};
	if (!stream.open(".package/manifest", false)) {
		TOGO_LOG_ERRORF(
			"failed to create manifest for package at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}
	BinaryOutputSerializer ser{stream};
	ser
		% u32{PKG_MANIFEST_FORMAT_VERSION}
		// number of entries
		% u32{0u}
	;
	stream.close();
	}
	return true;
}

PackageCompiler::LookupNode* package_compiler::get_node(
	PackageCompiler& pkg,
	ResourceNameHash const name_hash
) {
	return hash_map::get_node(pkg._lookup, name_hash);
}

u32 package_compiler::find_resource_id(
	PackageCompiler const& pkg,
	ResourcePathParts const& path_parts
) {
	auto const* node = hash_map::get_node(pkg._lookup, path_parts.name_hash);
	for (; node; node = hash_map::get_next(pkg._lookup, node)) {
		auto const& metadata = pkg._metadata[node->value - 1];
		if (
			path_parts.type_hash == metadata.type &&
			path_parts.tags_collated == metadata.tags_collated
		) {
			return metadata.id;
		}
	}
	return 0;
}

u32 package_compiler::add_resource(
	PackageCompiler& pkg,
	StringRef const& path,
	ResourcePathParts const& path_parts
) {
	TOGO_ASSERT(
		package_compiler::find_resource_id(pkg, path_parts) == 0,
		"resource already exists in package"
	);

	array::resize(pkg._metadata, array::size(pkg._metadata) + 1);
	auto& metadata = array::back(pkg._metadata);
	metadata.id = array::size(pkg._metadata);
	metadata.type = path_parts.type_hash;
	metadata.format_version = 0;
	metadata.name_hash = path_parts.name_hash;
	metadata.last_compiled = 0;
	metadata.tags_collated = path_parts.tags_collated;
	fixed_array::clear(metadata.path);
	string::copy(metadata.path, path);
	hash_map::push(pkg._lookup, metadata.name_hash, metadata.id);

	package_compiler::set_modified(pkg, true);
	return metadata.id;
}

void package_compiler::remove_resource(
	PackageCompiler& pkg,
	u32 const id
) {
	TOGO_ASSERTE(id > 0 && id <= array::size(pkg._metadata));
	auto& metadata = pkg._metadata[id - 1];
	TOGO_ASSERT(
		metadata.id != 0,
		"resource slot already removed"
	);

	{// Remove lookup node
	auto const* node = hash_map::get_node(pkg._lookup, metadata.name_hash);
	for (; node; node = hash_map::get_next(pkg._lookup, node)) {
		if (node->value == id) {
			break;
		}
	}
	TOGO_ASSERT(
		node != nullptr,
		"no lookup node found for the resource ID"
	);
	hash_map::remove(pkg._lookup, node);
	}

	if (metadata.last_compiled != 0) {
		FixedArray<char, 24> output_path{};
		resource_metadata::output_path(metadata, output_path);
		if (
			filesystem::is_file(output_path) &&
			!filesystem::remove_file(output_path)
		) {
			TOGO_LOG_ERRORF(
				"failed to remove compiled resource file: '%.*s'\n",
				string::size(output_path),
				fixed_array::begin(output_path)
			);
		}
	}

	// Leave hole at ID
	metadata.id = 0;
	metadata.type = RES_TYPE_NULL;
	metadata.format_version = 0;
	metadata.name_hash = 0;
	metadata.last_compiled = 0;
	metadata.tags_collated = 0;
	fixed_array::clear(metadata.path);

	package_compiler::set_modified(pkg, true);
}

void package_compiler::read(
	PackageCompiler& pkg
) {
	pkg._name_hash = ""_resource_package_name;
	string::copy(pkg._name, "");
	hash_map::clear(pkg._lookup);
	array::clear(pkg._metadata);

	StringRef const path{pkg._path};
	TOGO_ASSERTF(
		filesystem::is_directory(path),
		"'%.*s': package path does not exist",
		path.size, path.data
	);
	WorkingDirScope dir_scope{path};

	{// Read properties
	KVS k_root{};
	TOGO_ASSERTF(
		kvs::read_file(k_root, ".package/properties"),
		"'%.*s': failed to read package properties",
		path.size, path.data
	);

	KVS const* const k_name = kvs::find(k_root, "name");
	TOGO_ASSERTF(
		k_name && kvs::is_string(*k_name) && kvs::string_size(*k_name) > 0,
		"'%.*s': 'name' must be a non-empty string",
		path.size, path.data
	);
	string::copy(pkg._name, kvs::string_ref(*k_name));
	pkg._name_hash = resource::hash_package_name(pkg._name);
	}

	{// Read manifest
	FileReader stream{};
	TOGO_ASSERTF(
		stream.open(".package/manifest"),
		"'%.*s': failed to open manifest for reading",
		path.size, path.data
	);

	BinaryInputSerializer ser{stream};
	u32 format_version = 0;
	ser % format_version;
	TOGO_ASSERTF(
		format_version == PKG_MANIFEST_FORMAT_VERSION,
		"'%.*s': manifest version %u unsupported",
		path.size, path.data, format_version
	);

	ser % make_ser_collection<u32>(pkg._metadata);
	stream.close();

	for (u32 i = 0; i < array::size(pkg._metadata); ++i) {
		ResourceMetadata& rmd = pkg._metadata[i];
		if (rmd.type != RES_TYPE_NULL) {
			rmd.id = i + 1;
			hash_map::push(pkg._lookup, rmd.name_hash, rmd.id);
		} else {
			rmd.id = 0;
		}
	}
	}
}

bool package_compiler::write(
	PackageCompiler& pkg
) {
	StringRef const path{pkg._path};
	TOGO_ASSERTF(
		filesystem::is_directory(path),
		"'%.*s': package path does not exist",
		path.size, path.data
	);
	WorkingDirScope dir_scope{path};

	{// Write properties
	KVS k_properties{KVSType::node};
	kvs::push_back(k_properties, KVS{"name", pkg._name});
	if (!kvs::write_file(k_properties, ".package/properties")) {
		TOGO_LOG_ERRORF(
			"failed to write properties for package at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}}

	{// Write manifest
	FileWriter stream{};
	if (!stream.open(".package/manifest", false)) {
		TOGO_LOG_ERRORF(
			"failed to open manifest for package at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}

	BinaryOutputSerializer ser{stream};
	ser
		% u32{PKG_MANIFEST_FORMAT_VERSION}
		% make_ser_collection<u32>(pkg._metadata);
	;
	stream.close();
	}

	package_compiler::set_modified(pkg, false);
	return true;
}

} // namespace tool_build
} // namespace togo
