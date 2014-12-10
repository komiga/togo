#line 2 "togo/resource/resource_package.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/hash/hash.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/io/io.hpp>
#include <togo/io/file_stream.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource.hpp>
#include <togo/resource/resource_package.hpp>
#include <togo/resource/resource_manager.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/binary_serializer.hpp>
#include <togo/serialization/array.hpp>
#include <togo/serialization/resource/resource_metadata.hpp>

namespace togo {

ResourcePackage::ResourcePackage(
	StringRef const& name,
	StringRef const& path,
	Allocator& allocator
)
	: _name_hash(resource::hash_package_name(name))
	, _open_resource_id(0)
	, _stream()
	, _lookup(allocator)
	, _manifest(allocator)
	, _name()
	, _path()
{
	string::copy(_name, name);
	string::copy(_path, path);
}

void resource_package::open(
	ResourcePackage& pkg,
	ResourceManager const& rm
) {
	TOGO_ASSERT(!pkg._stream.is_open(), "package is already open");

	StringRef const name{pkg._name};
	StringRef const path{pkg._path};
	TOGO_ASSERTF(
		pkg._stream.open(path),
		"failed to open package '%.*s' at '%.*s'",
		name.size, name.data,
		path.size, path.data
	);

	BinaryInputSerializer ser{pkg._stream};
	u32 format_version = 0;
	ser % format_version;
	TOGO_ASSERTF(
		format_version == SER_FORMAT_VERSION_PKG_MANIFEST,
		"manifest version %u unsupported from package '%.*s' at '%.*s'",
		format_version,
		name.size, name.data,
		path.size, path.data
	);

	ser % make_ser_collection<u32>(pkg._manifest);
	for (u32 i = 0; i < array::size(pkg._manifest); ++i) {
		auto& metadata = pkg._manifest[i];
		if (metadata.type == RES_TYPE_NULL) {
			metadata.id = 0;
			continue;
		}
		metadata.id = i + 1;
		hash_map::push(pkg._lookup, metadata.name_hash, metadata.id);
		TOGO_ASSERTF(
			resource_manager::has_handler(rm, metadata.type),
			"no handler registered for resource %16lx's type %08x",
			metadata.name_hash, metadata.type
		);
	}
}

void resource_package::close(
	ResourcePackage& pkg
) {
	TOGO_ASSERT(pkg._stream.is_open(), "package is already closed");
	pkg._stream.close();
}

ResourceMetadata const& resource_package::resource_metadata(
	ResourcePackage const& pkg,
	u32 const id
) {
	TOGO_ASSERTE(id > 0 && id <= array::size(pkg._manifest));
	return pkg._manifest[id - 1];
}

ResourcePackage::LookupNode* resource_package::find_resource(
	ResourcePackage& pkg,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto* const node = hash_map::get_node(pkg._lookup, name_hash);
	return (node && pkg._manifest[node->value - 1].type == type) ? node : nullptr;
}

IReader* resource_package::open_resource_stream(
	ResourcePackage& pkg,
	u32 const id
) {
	TOGO_ASSERT(pkg._stream.is_open(), "package is not open");
	TOGO_ASSERT(pkg._open_resource_id == 0, "a resource stream is already open");
	TOGO_ASSERT(id > 0 && id <= array::size(pkg._manifest), "invalid ID");
	auto const& metadata = pkg._manifest[id - 1];
	TOGO_ASSERTE(io::seek_to(pkg._stream, metadata.data_offset));
	pkg._open_resource_id = id;
	return &pkg._stream;
}

void resource_package::close_resource_stream(
	ResourcePackage& pkg
) {
	TOGO_ASSERT(pkg._stream.is_open(), "package is not open");
	TOGO_ASSERT(pkg._open_resource_id != 0, "invalid ID");
	#if defined(TOGO_DEBUG)
		auto const& metadata = pkg._manifest[pkg._open_resource_id - 1];
		auto const stream_pos = io::position(pkg._stream);
		TOGO_DEBUG_ASSERTE(
			stream_pos >= metadata.data_offset &&
			stream_pos <= metadata.data_offset + metadata.data_size
		);
	#endif
	pkg._open_resource_id = 0;
}

} // namespace togo
