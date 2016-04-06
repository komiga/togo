#line 2 "togo/game/resource/resource_package.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/hash_map.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/file_stream.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/core/serialization/array.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource.hpp>
#include <togo/game/resource/resource_package.hpp>
#include <togo/game/resource/resource_manager.hpp>
#include <togo/game/serialization/resource/resource.hpp>

namespace togo {
namespace game {

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

/// Open package.
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
		auto& metadata = pkg._manifest[i].metadata;
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

/// Close package.
void resource_package::close(
	ResourcePackage& pkg
) {
	TOGO_ASSERT(pkg._stream.is_open(), "package is already closed");
	pkg._stream.close();
}

/// Resource for ID.
///
/// An assertion will fail if the ID is invalid.
Resource const& resource_package::resource(
	ResourcePackage const& pkg,
	u32 const id
) {
	TOGO_ASSERT(id > 0 && id <= array::size(pkg._manifest), "invalid ID");
	auto& resource = pkg._manifest[id - 1];
	TOGO_DEBUG_ASSERT(resource.metadata.id != 0, "null entry");
	return resource;
}

/// Find lookup node by resource name.
ResourcePackage::LookupNode* resource_package::find_node(
	ResourcePackage& pkg,
	ResourceNameHash const name_hash
) {
	return hash_map::find_node(pkg._lookup, name_hash);
}

/// Open resource stream by ID.
///
/// An assertion will fail if resource stream couldn't be opened.
/// An assertion will fail if there is already an open stream.
IReader* resource_package::open_resource_stream(
	ResourcePackage& pkg,
	u32 const id
) {
	TOGO_ASSERT(pkg._stream.is_open(), "package is not open");
	TOGO_ASSERT(pkg._open_resource_id == 0, "a resource stream is already open");
	auto const& metadata = resource_package::resource(pkg, id).metadata;
	TOGO_ASSERTE(io::seek_to(pkg._stream, metadata.data_offset));
	pkg._open_resource_id = id;
	return &pkg._stream;
}

/// Close current resource stream.
///
/// An assertion will fail if there is no open stream.
void resource_package::close_resource_stream(
	ResourcePackage& pkg
) {
	TOGO_ASSERT(pkg._stream.is_open(), "package is not open");
	TOGO_ASSERT(pkg._open_resource_id != 0, "no resource stream is open");
	#if defined(TOGO_DEBUG)
		auto const& metadata = resource_package::resource(
			pkg, pkg._open_resource_id
		).metadata;
		auto const stream_pos = io::position(pkg._stream);
		TOGO_DEBUG_ASSERTE(
			stream_pos >= metadata.data_offset &&
			stream_pos <= metadata.data_offset + metadata.data_size
		);
	#endif
	pkg._open_resource_id = 0;
}

} // namespace game
} // namespace togo
