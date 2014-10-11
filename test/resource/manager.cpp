
#include <togo/types.hpp>
#include <togo/assert.hpp>
#include <togo/utility.hpp>
#include <togo/log.hpp>
#include <togo/hash.hpp>
#include <togo/memory.hpp>
#include <togo/io_types.hpp>
#include <togo/io.hpp>
#include <togo/resource_types.hpp>
#include <togo/resource.hpp>
#include <togo/resource_manager.hpp>

#include "../common/helpers.hpp"

using namespace togo;

enum : ResourceType {
	RESTYPE_DUMMY = "dummy"_resource_type,
};

enum : ResourceNameHash {
	RES_DUMMY_NON_MANIFEST = "non_manifest"_resource_name,

	// data/pkg/test1
	PKG1_DUMMY_NON_EXISTENT = "non_existent"_resource_name,
	PKG1_DUMMY_1 = "1"_resource_name,
	PKG1_DUMMY_2 = "subdir/2"_resource_name,

	// data/pkg/test2
	PKG2_DUMMY_1 = "1"_resource_name,
	PKG2_DUMMY_3 = "3"_resource_name,
};

static void* s_type_data = nullptr;

struct Dummy {
	u32 x;
	ResourceNameHash name_hash;
};

static void* dummy_load(
	void* type_data,
	ResourceManager& /*manager*/,
	ResourceNameHash name_hash,
	IReader& stream
) {
	TOGO_ASSERTE(type_data == s_type_data);
	auto* const dummy = TOGO_CONSTRUCT(
		memory::default_allocator(), Dummy, {0, name_hash}
	);
	io::read_value(stream, dummy->x);
	return dummy;
}

static void dummy_unload(
	void* type_data,
	ResourceManager& /*manager*/,
	ResourceNameHash name_hash,
	void* resource
) {
	TOGO_ASSERTE(type_data == s_type_data);
	auto const* const dummy = static_cast<Dummy*>(resource);
	TOGO_ASSERTE(dummy->name_hash == name_hash);
	TOGO_DESTROY(memory::default_allocator(), dummy);
}

static ResourceHandler const
s_handler_dummy{RESTYPE_DUMMY, dummy_load, dummy_unload};

void test(
	ResourceManager& rm,
	ResourceNameHash const name_hash,
	bool const expect_success,
	u32 const x
) {
	TOGO_ASSERTE(!resource_manager::get_resource(
		rm, RESTYPE_DUMMY, name_hash
	));
	auto const* dummy = static_cast<Dummy*>(
		resource_manager::load_resource(rm, RESTYPE_DUMMY, name_hash)
	);
	TOGO_ASSERTE(static_cast<bool>(dummy) == expect_success);
	TOGO_ASSERTE(dummy == resource_manager::get_resource(
		rm, RESTYPE_DUMMY, name_hash
	));
	if (dummy) {
		TOGO_ASSERTE(dummy->x == x && dummy->name_hash == name_hash);
		resource_manager::unload_resource(rm, RESTYPE_DUMMY, name_hash);
		dummy = nullptr;
		TOGO_ASSERTE(!resource_manager::get_resource(
			rm, RESTYPE_DUMMY, name_hash
		));
	}
}

signed main() {
	memory_init();

	ResourceManager rm{memory::default_allocator()};

	// Using arbitrary object as type data only for validation
	s_type_data = &rm;
	resource_manager::register_handler(rm, s_handler_dummy, s_type_data);
	resource_manager::add_package(rm, "data/pkg/test1");

	// Non-existent, non-manifested
	test(rm, RES_DUMMY_NON_MANIFEST, false, 0);

	// Non-existent, manifested
	test(rm, PKG1_DUMMY_NON_EXISTENT, false, 0);

	// Existent, manifest
	test(rm, PKG1_DUMMY_1, true, 1);
	test(rm, PKG1_DUMMY_2, true, 2);

	// Additional package
	resource_manager::add_package(rm, "data/pkg/test2");

	// test2/1.dummy overlaps test1/1.dummy due to package order
	test(rm, PKG2_DUMMY_1, true, 42);

	// New resource in test2 that doesn't overlap any in test1
	test(rm, PKG2_DUMMY_3, true, 3);

	return 0;
}
