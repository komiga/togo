#line 2 "togo/game/tool_build/package_compiler.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/tool_build/config.hpp>
#include <togo/game/tool_build/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/collection/hash_map.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/filesystem/filesystem.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/file_stream.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/core/serialization/array.hpp>
#include <togo/core/serialization/string.hpp>
#include <togo/game/resource/resource.hpp>
#include <togo/game/serialization/resource/resource_metadata.hpp>
#include <togo/game/tool_build/package_compiler.hpp>

namespace togo {
namespace tool_build {

namespace {
	template<class T, bool C>
	struct SerialAuxiliary {
		using value_type = type_if<C, T const, T>;

		value_type& value;
	};

	template<class T>
	inline SerialAuxiliary<remove_cv<T>, is_const<T>::value>
	make_serial_auxiliary(T& value) {
		return {value};
	}
} // anonymous namespace

// Resource metadata
template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, ResourceCompilerMetadata& value_unsafe) {
	ser
		% serializer_cast_safe<Ser>(
			static_cast<ResourceMetadata&>(value_unsafe)
		)
	;
}

// Compiler metadata
template<class Ser, bool C>
inline void
serialize(
	serializer_tag,
	Ser& ser,
	SerialAuxiliary<ResourceCompilerMetadata, C>&& value_unsafe
) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe.value);
	ser
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
	, _name_hash(PKG_NAME_NULL)
	, _lookup(allocator)
	, _manifest(allocator)
	, _name()
	, _path()
{
	string::copy(_path, path);
	string::trim_trailing_slashes(_path);
}

/// Create a package stub.
///
/// This will fail if the path already exists or if any part of the
/// package couldn't be created.
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
	kvs::push_back(k_properties, KVS{"build_parity", false, bool_tag{}});
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
		% u32{SER_FORMAT_VERSION_PKG_MANIFEST}
		// number of entries
		% u32{0u}
	;
	stream.close();
	}

	{// Create compiler_metadata
	FileWriter stream{};
	if (!stream.open(".package/compiler_metadata", false)) {
		TOGO_LOG_ERRORF(
			"failed to create compiler_metadata for package at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}
	BinaryOutputSerializer ser{stream};
	ser
		% u32{SER_FORMAT_VERSION_PKG_COMPILER_METADATA}
	;
	stream.close();
	}
	return true;
}

/// Find lookup node by resource name.
PackageCompiler::LookupNode* package_compiler::find_node(
	PackageCompiler& pkg,
	ResourceNameHash const name_hash
) {
	return hash_map::find_node(pkg._lookup, name_hash);
}

/// Find resource ID by identity.
///
/// 0 is returned if a resource was not found.
/// If tags_lenient is true, a tag-less entry will match if there is
/// no resource with tags_hash exactly.
u32 package_compiler::find_resource_id(
	PackageCompiler const& pkg,
	ResourceType const type,
	ResourceNameHash const name_hash,
	ResourceTagsHash const tags_hash,
	bool const tags_lenient
) {
	auto const* node = hash_map::find_node(pkg._lookup, name_hash);
	u32 id_lenient = 0;
	for (; node; node = hash_map::next_node(pkg._lookup, node)) {
		auto const& metadata = pkg._manifest[node->value - 1];
		if (
			metadata.id == 0 ||
			type != metadata.type
		) {
			continue;
		}
		if (tags_hash == metadata.tags_hash) {
			return metadata.id;
		} else if (
			tags_lenient &&
			metadata.tags_hash == RES_TAGS_NULL
		) {
			id_lenient = metadata.id;
		}
	}
	return id_lenient;
}

/// Add resource.
u32 package_compiler::add_resource(
	PackageCompiler& pkg,
	StringRef const& path,
	ResourcePathParts const& path_parts
) {
	TOGO_ASSERT(
		package_compiler::find_resource_id(pkg, path_parts, false) == 0,
		"resource already exists in package"
	);

	array::increase_size(pkg._manifest, 1);
	auto& metadata = array::back(pkg._manifest);
	metadata.id = array::size(pkg._manifest);
	metadata.name_hash = path_parts.name_hash;
	metadata.tags_hash = path_parts.tags_hash;
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

/// Remove resource by ID.
///
/// This assumes the working directory is already at the package.
void package_compiler::remove_resource(PackageCompiler& pkg, u32 const id) {
	TOGO_ASSERTE(id > 0 && id <= array::size(pkg._manifest));
	auto& metadata = pkg._manifest[id - 1];
	TOGO_ASSERT(
		metadata.id != 0,
		"resource slot already removed"
	);

	{// Remove lookup node
	auto const* node = hash_map::find_node(pkg._lookup, metadata.name_hash);
	for (; node; node = hash_map::next_node(pkg._lookup, node)) {
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
		ResourceCompiledPath compiled_path{};
		resource::set_compiled_path(compiled_path, metadata.id);
		if (
			filesystem::is_file(compiled_path) &&
			!filesystem::remove_file(compiled_path)
		) {
			TOGO_LOG_ERRORF(
				"failed to remove compiled resource file: '%.*s'\n",
				compiled_path.size(), compiled_path.data()
			);
		}
	}

	// Leave hole at ID
	metadata.id = 0;
	metadata.name_hash = RES_NAME_NULL;
	metadata.tags_hash = RES_TAGS_NULL;
	metadata.type = RES_TYPE_NULL;
	metadata.data_format_version = 0;
	metadata.data_offset = 0;
	metadata.data_size = 0;
	metadata.last_compiled = 0;
	fixed_array::clear(metadata.path);

	package_compiler::set_manifest_modified(pkg, true);
}

/// Read package data.
void package_compiler::read(PackageCompiler& pkg) {
	pkg._name_hash = RES_NAME_NULL;
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
		format_version == SER_FORMAT_VERSION_PKG_MANIFEST,
		"'%.*s': manifest version %u unsupported",
		path.size, path.data, format_version
	);

	ser % make_ser_collection<u32>(pkg._manifest);
	stream.close();
	}

	{// Read compiler_metadata
	FileReader stream{};
	TOGO_ASSERTF(
		stream.open(".package/compiler_metadata"),
		"'%.*s': failed to open compiler_metadata for reading",
		path.size, path.data
	);

	BinaryInputSerializer ser{stream};
	u32 format_version = 0;
	ser % format_version;
	TOGO_ASSERTF(
		format_version == SER_FORMAT_VERSION_PKG_COMPILER_METADATA,
		"'%.*s': compiler_metadata version %u unsupported",
		path.size, path.data, format_version
	);

	for (auto& metadata : pkg._manifest) {
		ser % make_serial_auxiliary(metadata);
	}
	stream.close();
	}

	for (unsigned i = 0; i < array::size(pkg._manifest); ++i) {
		auto& metadata = pkg._manifest[i];
		if (metadata.type != RES_TYPE_NULL) {
			metadata.id = i + 1;
			hash_map::push(pkg._lookup, metadata.name_hash, metadata.id);
		} else {
			metadata.id = 0;
		}
	}
}

/// Write modified package data and mark it as not modified.
bool package_compiler::write(PackageCompiler& pkg) {
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

/// Write package properties.
bool package_compiler::write_properties(PackageCompiler& pkg) {
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

/// Write package manifest.
bool package_compiler::write_manifest(PackageCompiler& pkg) {
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
		% u32{SER_FORMAT_VERSION_PKG_MANIFEST}
		% make_ser_collection<u32>(pkg._manifest);
	;
	stream.close();
	}

	{// Write compiler_metadata
	FileWriter stream{};
	if (!stream.open(".package/compiler_metadata", false)) {
		TOGO_LOG_ERRORF(
			"failed to open compiler_metadata for package at '%.*s'\n",
			path.size, path.data
		);
		return false;
	}

	BinaryOutputSerializer ser{stream};
	ser
		% u32{SER_FORMAT_VERSION_PKG_COMPILER_METADATA}
	;
	for (auto const& metadata : pkg._manifest) {
		ser % make_serial_auxiliary(metadata);
	}
	stream.close();
	}

	package_compiler::set_manifest_modified(pkg, false);
	return true;
}

/// Build package.
bool package_compiler::build(PackageCompiler& pkg, StringRef const& output_path) {
	StringRef const path{pkg._path};
	TOGO_ASSERTF(
		filesystem::is_directory(path),
		"'%.*s': package path does not exist",
		path.size, path.data
	);

	FileWriter stream{};
	if (!stream.open(output_path, false)) {
		TOGO_LOG_ERRORF(
			"failed to open output package file: '%.*s'\n",
			output_path.size, output_path.data
		);
		return false;
	}

	WorkingDirScope wd_scope{path};

	ResourceCompiledPath compiled_path{};
	u32 const offset_basis
		= 4 + 4
		// manifest
		+ (32 * array::size(pkg._manifest))
	;

	{// Calculate data offsets and sizes
	u32 size;
	u32 offset = offset_basis;
	for (auto& metadata : pkg._manifest) {
		if (metadata.id == 0) {
			metadata.data_offset = 0;
			metadata.data_size = 0;
			continue;
		}

		resource::set_compiled_path(compiled_path, metadata.id);
		TOGO_ASSERTE(filesystem::is_file(compiled_path));
		size = static_cast<u32>(filesystem::file_size(compiled_path));
		metadata.data_offset = offset;
		metadata.data_size = size;
		offset += size;
	}}

	{// Serialize
	// Serial form:
	//    FORMAT_VERSION
	//    manifest
	//    <resource data>
	BinaryOutputSerializer ser{stream};
	ser
		% u32{SER_FORMAT_VERSION_PKG_MANIFEST}
		% make_ser_collection<u32>(pkg._manifest)
	;
	TOGO_ASSERTE(offset_basis == io::position(stream));

	enum : u32 {
		tmp_buffer_size = 1 * 1024 * 1024,
	};
	StringRef rpath{};
	FileReader compiled_stream{};
	unsigned read_size;
	void* const tmp_buffer = memory::scratch_allocator().allocate(tmp_buffer_size);
	for (auto const& metadata : pkg._manifest) {
		if (metadata.id == 0) {
			continue;
		}

		resource::set_compiled_path(compiled_path, metadata.id);
		rpath = metadata.path;
		if (!compiled_stream.open(compiled_path)) {
			TOGO_LOG_ERRORF(
				"failed to open compiled resource file for '%.*s': '%.*s'\n",
				rpath.size, rpath.data,
				compiled_path.size(), compiled_path.data()
			);
			memory::scratch_allocator().deallocate(tmp_buffer);
			return false;
		}

		TOGO_ASSERTE(metadata.data_offset == io::position(stream));
		do {
			TOGO_ASSERTE(!io::read(
				compiled_stream,
				tmp_buffer, tmp_buffer_size,
				&read_size
			).fail());
			TOGO_ASSERTE(io::write(stream, tmp_buffer, read_size));
		} while (io::status(compiled_stream));
		TOGO_ASSERTE(metadata.data_size == io::position(compiled_stream));
		compiled_stream.close();
	}
	memory::scratch_allocator().deallocate(tmp_buffer);
	}
	stream.close();

	package_compiler::set_properties_modified(pkg, pkg._build_parity != true);
	pkg._build_parity = true;
	return true;
}

/// Remove empty metadata entries.
bool package_compiler::compact(PackageCompiler& pkg) {
	bool modified = false;
	for (unsigned i = 0; i < array::size(pkg._manifest);) {
		auto const& metadata = pkg._manifest[i];
		if (metadata.id == 0) {
			// Retain relative order
			array::remove(pkg._manifest, i);
			modified = true;
			continue;
		} else if (metadata.id != i + 1) {
			modified = true;
		}
		++i;
	}
	if (modified) {
		WorkingDirScope wd_scope{package_compiler::path(pkg)};
		hash_map::clear(pkg._lookup);
		unsigned id = 1;
		ResourceCompiledPath compiled_path{};
		ResourceCompiledPath compiled_path_to{};
		for (auto& metadata : pkg._manifest) {
			TOGO_DEBUG_ASSERTE(
				metadata.id != 0 &&
				metadata.name_hash != RES_NAME_NULL
			);
			if (metadata.id != id) {
				resource::set_compiled_path(compiled_path, metadata.id);
				resource::set_compiled_path(compiled_path_to, id);
				if (
					filesystem::is_file(compiled_path) &&
					!filesystem::move_file(compiled_path, compiled_path_to)
				) {
					TOGO_LOG_ERRORF(
						"failed to move compiled resource file: '%.*s' -> '%.*s'\n",
						compiled_path.size(), compiled_path.data(),
						compiled_path_to.size(), compiled_path_to.data()
					);
				}
				metadata.id = id;
				// TODO: Require recompile if dependencies were relocated
				//metadata.last_compiled = 0;
			}
			hash_map::push(pkg._lookup, metadata.name_hash, metadata.id);
			++id;
		}
		package_compiler::set_manifest_modified(pkg, true);
	}
	return modified;
}

} // namespace tool_build
} // namespace togo
