#line 2 "togo/tool_build/package_compiler.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/string/string.hpp>
#include <togo/filesystem/filesystem.hpp>
#include <togo/io/file_stream.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/string.hpp>
#include <togo/serialization/array.hpp>
#include <togo/serialization/binary_serializer.hpp>
#include <togo/serialization/resource/resource_metadata.hpp>
#include <togo/kvs/kvs.hpp>
#include <togo/resource/resource.hpp>
#include <togo/resource/resource_metadata.hpp>
#include <togo/tool_build/package_compiler.hpp>

namespace togo {
namespace tool_build {

enum : u32 {
	PKG_MANIFEST_FORMAT_VERSION = 2u,
};

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, ResourceCompilerMetadata& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% serializer_cast_safe<Ser>(
			static_cast<ResourceMetadata&>(value_unsafe)
		)
		% value.last_compiled
		% make_ser_string<u8>(value.path)
	;
}

PackageCompiler::PackageCompiler(
	StringRef const& path,
	Allocator& allocator
)
	: _properties_modified(false)
	, _manifest_modified(false)
	, _build_parity(false)
	, _name_hash(""_resource_package_name)
	, _lookup(allocator)
	, _manifest(allocator)
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

	WorkingDirScope wd_scope{path};

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
	kvs::push_back(k_properties, KVS{"build_parity", false});
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
		auto const& metadata = pkg._manifest[node->value - 1];
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

	array::resize(pkg._manifest, array::size(pkg._manifest) + 1);
	auto& metadata = array::back(pkg._manifest);
	metadata.id = array::size(pkg._manifest);
	metadata.name_hash = path_parts.name_hash;
	metadata.tags_collated = path_parts.tags_collated;
	metadata.type = path_parts.type_hash;
	metadata.data_format_version = 0;
	metadata.data_offset = 0;
	metadata.data_size = 0;
	metadata.last_compiled = 0;
	fixed_array::clear(metadata.path);
	string::copy(metadata.path, path);
	hash_map::push(pkg._lookup, metadata.name_hash, metadata.id);

	package_compiler::set_manifest_modified(pkg, true);
	return metadata.id;
}

void package_compiler::remove_resource(
	PackageCompiler& pkg,
	u32 const id
) {
	TOGO_ASSERTE(id > 0 && id <= array::size(pkg._manifest));
	auto& metadata = pkg._manifest[id - 1];
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
		resource_metadata::compiled_path(metadata, output_path);
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
	metadata.name_hash = 0;
	metadata.tags_collated = 0;
	metadata.type = RES_TYPE_NULL;
	metadata.data_format_version = 0;
	metadata.data_offset = 0;
	metadata.data_size = 0;
	metadata.last_compiled = 0;
	fixed_array::clear(metadata.path);

	package_compiler::set_manifest_modified(pkg, true);
}

void package_compiler::read(
	PackageCompiler& pkg
) {
	pkg._name_hash = ""_resource_package_name;
	string::copy(pkg._name, "");
	hash_map::clear(pkg._lookup);
	array::clear(pkg._manifest);

	StringRef const path{pkg._path};
	TOGO_ASSERTF(
		filesystem::is_directory(path),
		"'%.*s': package path does not exist",
		path.size, path.data
	);
	WorkingDirScope wd_scope{path};

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

	KVS const* const k_build_parity = kvs::find(k_root, "build_parity");
	if (k_build_parity && kvs::is_boolean(*k_build_parity)) {
		pkg._build_parity = kvs::boolean(*k_build_parity);
	} else {
		pkg._build_parity = false;
		TOGO_LOGF(
			"'%.*s': warning: expected boolean 'build_parity' property\n",
			path.size, path.data
		);
	}}

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

	ser % make_ser_collection<u32>(pkg._manifest);
	stream.close();

	for (u32 i = 0; i < array::size(pkg._manifest); ++i) {
		auto& rmd = pkg._manifest[i];
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
	if (
		package_compiler::properties_modified(pkg) &&
		!package_compiler::write_properties(pkg)
	) {
		return false;
	}

	if (
		package_compiler::manifest_modified(pkg) &&
		!package_compiler::write_manifest(pkg)
	) {
		return false;
	}
	return true;
}

bool package_compiler::write_properties(
	PackageCompiler& pkg
) {
	StringRef const path{pkg._path};
	TOGO_ASSERTF(
		filesystem::is_directory(path),
		"'%.*s': package path does not exist",
		path.size, path.data
	);
	WorkingDirScope wd_scope{path};

	{// Write properties
	KVS k_properties{KVSType::node};
	kvs::push_back(k_properties, KVS{"name", pkg._name});
	kvs::push_back(k_properties, KVS{"build_parity", pkg._build_parity, bool_tag{}});
	if (!kvs::write_file(k_properties, ".package/properties")) {
		TOGO_LOG_ERRORF(
			"failed to write properties for package at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}}

	package_compiler::set_properties_modified(pkg, false);
	return true;
}

bool package_compiler::write_manifest(
	PackageCompiler& pkg
) {
	StringRef const path{pkg._path};
	TOGO_ASSERTF(
		filesystem::is_directory(path),
		"'%.*s': package path does not exist",
		path.size, path.data
	);
	WorkingDirScope wd_scope{path};

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
		% make_ser_collection<u32>(pkg._manifest);
	;
	stream.close();
	}

	package_compiler::set_manifest_modified(pkg, false);
	return true;
}

} // namespace tool_build
} // namespace togo
