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
#include <togo/io.hpp>
#include <togo/file_io.hpp>
#include <togo/kvs.hpp>
#include <togo/resource.hpp>
#include <togo/tool_build/package_compiler.hpp>

namespace togo {
namespace tool_build {

enum : u32 {
	PKG_MANIFEST_FORMAT_VERSION = 1u,
};

PackageCompiler::PackageCompiler(
	StringRef const& path,
	Allocator& allocator
)
	: _name_hash(""_resource_package_name)
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
	TOGO_ASSERTE(
		io::write_value(stream, u32{PKG_MANIFEST_FORMAT_VERSION}) &&
		io::write_value(stream, u32{0u})
	);
	stream.close();
	}
	return true;
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

	u32 format_version = 0;
	u32 num_entries = 0;
	TOGO_ASSERTE(io::read_value(stream, format_version));
	TOGO_ASSERTF(
		format_version == PKG_MANIFEST_FORMAT_VERSION,
		"'%.*s': manifest version %u unsupported",
		path.size, path.data, format_version
	);

	TOGO_ASSERTE(io::read_value(stream, num_entries));
	array::resize(pkg._metadata, num_entries);

	u8 path_size = 0;
	for (u32 id = 1; id <= num_entries; ++id) {
		ResourceMetadata& rmd = pkg._metadata[id - 1];
		TOGO_ASSERTE(io::read_value(stream, rmd.type));
		if (rmd.type == RES_TYPE_NULL) {
			// Empty slot
			rmd.id = 0;
			rmd.format_version = 0;
			rmd.name_hash = RES_NAME_NULL;
			rmd.last_compiled = 0;
			rmd.tags_collated = hash::IDENTITY64;
			fixed_array::clear(rmd.path);
			continue;
		}

		rmd.id = id;
		TOGO_ASSERTE(
			io::read_value(stream, rmd.format_version) &&
			io::read_value(stream, rmd.name_hash) &&
			io::read_value(stream, rmd.last_compiled) &&
			io::read_value(stream, rmd.tags_collated) &&
			io::read_value(stream, path_size)
		);
		fixed_array::resize(rmd.path, path_size + 1);
		TOGO_ASSERTE(io::read(stream, fixed_array::begin(rmd.path), path_size));
		fixed_array::back(rmd.path) = '\0';
	}
	stream.close();
	}
}

} // namespace tool_build
} // namespace togo
