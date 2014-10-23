#line 2 "togo/resource_package.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/hash.hpp>
#include <togo/hash_map.hpp>
#include <togo/fixed_array.hpp>
#include <togo/file_io.hpp>
#include <togo/kvs.hpp>
#include <togo/resource_types.hpp>
#include <togo/resource.hpp>
#include <togo/resource_package.hpp>
#include <togo/resource_manager.hpp>

namespace togo {

ResourcePackage::ResourcePackage(
	StringRef const& root,
	Allocator& allocator
)
	: _root_hash(hash::calc64(root))
	, _data_stream()
	, _entries(allocator)
	, _root()
{
	string::copy(_root, root);
	string::trim_trailing_slashes(_root);
}

void resource_package::load_manifest(
	ResourcePackage& pkg,
	ResourceManager const& rm
) {
	FixedArray<char, 128> mpath;
	string::copy(mpath, pkg._root);
	string::append(mpath, "/manifest");
	StringRef const mpath_ref{mpath};

	FileReader stream;
	KVS manifest_root{};
	ParserInfo pinfo;
	TOGO_ASSERTF(
		stream.open(mpath_ref),
		"failed to open package manifest '%.*s'",
		mpath_ref.size, mpath_ref.data
	);
	bool const success = kvs::read(manifest_root, stream, pinfo);
	stream.close();
	TOGO_ASSERTF(
		success,
		"failed to read package manifest '%.*s': [%2u,%2u]: %s",
		mpath_ref.size, mpath_ref.data,
		pinfo.line, pinfo.column, pinfo.message
	);

	KVS const* const manifest_resources = kvs::find(manifest_root, "resources");
	TOGO_ASSERTF(
		manifest_resources != nullptr && kvs::is_array(*manifest_resources),
		"malformed package manifest '%.*s'",
		mpath_ref.size, mpath_ref.data
	);

	FixedArray<char, array_extent(&ResourcePackage::Entry::path)> full_path;
	ResourcePathParts pparts;
	StringRef rpath_ref{};
	StringRef full_path_ref{};
	bool parse_path_success;
	string::copy(full_path, pkg._root);
	fixed_array::back(full_path) = '/';
	fixed_array::push_back(full_path, '\0');
	unsigned const base_size = string::size(full_path);
	hash_map::reserve(pkg._entries, kvs::size(*manifest_resources));
	for (auto const& kvs_rp : *manifest_resources) {
		TOGO_ASSERTF(
			kvs::is_string(kvs_rp),
			"malformed package manifest '%.*s'",
			mpath_ref.size, mpath_ref.data
		);
		rpath_ref = kvs::string_ref(kvs_rp);
		if (rpath_ref.size == 0) {
			TOGO_LOG_ERRORF(
				"invalid resource path"
				" in package manifest '%.*s': ''\n",
				mpath_ref.size, mpath_ref.data
			);
			continue;
		}
		// Retain root path, replace inner path part
		fixed_array::resize(full_path, base_size);
		string::append(full_path, rpath_ref);
		full_path_ref = full_path;
		parse_path_success = resource::parse_path(rpath_ref, pparts);
		if (!parse_path_success) {
			TOGO_LOG_ERRORF(
				"invalid resource path"
				" in package manifest '%.*s': '%.*s'\n",
				mpath_ref.size, mpath_ref.data,
				rpath_ref.size, rpath_ref.data
			);
		} else if (!resource_manager::has_handler(rm, pparts.type_hash)) {
			TOGO_LOG_ERRORF(
				"resource type not recognized"
				" in package manifest '%.*s': '%.*s', of resource '%.*s'\n",
				mpath_ref.size, mpath_ref.data,
				pparts.type.size, pparts.type.data,
				rpath_ref.size, rpath_ref.data
			);
		} else if (hash_map::has(pkg._entries, pparts.name_hash)) {
			TOGO_LOG_ERRORF(
				"duplicate resource name"
				" in package manifest '%.*s': '%.*s'\n",
				mpath_ref.size, mpath_ref.data,
				rpath_ref.size, rpath_ref.data
			);
		} else {
			auto& entry = hash_map::push(pkg._entries, pparts.name_hash, {});
			entry.type = pparts.type_hash;
			string::copy(entry.path, full_path_ref);
			entry.path_size = static_cast<u8>(full_path_ref.size);
		}
	}
}

ResourcePackage::EntryNode* resource_package::find_resource(
	ResourcePackage& pkg,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto* const node = hash_map::get_node(pkg._entries, name_hash);
	return (node && node->value.type == type) ? node : nullptr;
}

IReader* resource_package::open_resource_stream(
	ResourcePackage& pkg,
	ResourcePackage::EntryNode* const node
) {
	TOGO_ASSERTE(node != nullptr);
	TOGO_ASSERTE(!pkg._data_stream.is_open());
	auto const& entry = node->value;
	StringRef const path_ref{entry.path, entry.path_size};
	return pkg._data_stream.open(path_ref) ? &pkg._data_stream : nullptr;
}

void resource_package::close_resource_stream(
	ResourcePackage& pkg
) {
	TOGO_ASSERTE(pkg._data_stream.is_open());
	pkg._data_stream.close();
}

} // namespace togo
