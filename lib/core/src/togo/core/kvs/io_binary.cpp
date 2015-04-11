#line 2 "togo/core/kvs/io_binary.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/temp_allocator.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/file_stream.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/kvs/io/binary.ipp>
#include <togo/core/serialization/types.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>

namespace togo {

namespace {
enum : u32 {
	SER_FORMAT_VERSION_KVS = 1,
};
} // anonymous namespace

/// Read binary-format KVS from stream.
///
/// An assertion will fail if either the format version does not match the
/// deserializer or an IO error occurs.
IOStatus kvs::read_binary(
	KVS& root,
	IReader& stream,
	Endian const endian IGEN_DEFAULT(Endian::little)
) {
	kvs::set_type(root, KVSType::node);
	kvs::clear(root);

	TempAllocator<2048> allocator{};
	BinaryInputSerializer ser{stream, endian};
	u32 format_version;
	ser % format_version;
	TOGO_ASSERTF(
		format_version == SER_FORMAT_VERSION_KVS,
		"KVS format version mismatch: %u != %u\n",
		format_version, SER_FORMAT_VERSION_KVS
	);

	Array<char> scratch{allocator};
	array::reserve(scratch, 2048 - sizeof(void*));
	kvs_read_binary(root, ser, scratch);
	return io::status(stream);
}

/// Read binary-format KVS from file.
bool kvs::read_binary_file(
	KVS& root,
	StringRef const& path,
	Endian const endian IGEN_DEFAULT(Endian::little)
) {
	FileReader stream{};
	if (!stream.open(path)) {
		TOGO_LOG_ERRORF(
			"failed to read KVS binary from '%.*s': failed to open file\n",
			path.size, path.data
		);
		return false;
	}

	bool success = kvs::read_binary(root, stream, endian).ok();
	stream.close();
	return success;
}

/// Write binary-format KVS to stream.
///
/// root must be a node.
/// An assertion will fail if an IO error occurs.
IOStatus kvs::write_binary(
	KVS const& root,
	IWriter& stream,
	Endian const endian IGEN_DEFAULT(Endian::little)
) {
	TOGO_ASSERT(kvs::type(root) == KVSType::node, "root must be a node");
	BinaryOutputSerializer ser{stream, endian};
	ser % u32{SER_FORMAT_VERSION_KVS};
	kvs_write_binary(root, ser);
	return io::status(stream);
}

/// Write binary-format KVS to file.
bool kvs::write_binary_file(
	KVS const& root,
	StringRef const& path,
	Endian const endian IGEN_DEFAULT(Endian::little)
) {
	FileWriter stream{};
	if (!stream.open(path, false)) {
		TOGO_LOG_ERRORF(
			"failed to write KVS binary to '%.*s': failed to open file\n",
			path.size, path.data
		);
		return false;
	}

	bool success = kvs::write_binary(root, stream, endian).ok();
	stream.close();
	return success;
}

} // namespace togo
