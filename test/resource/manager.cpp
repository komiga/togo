
#include <togo/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/log/log.hpp>
#include <togo/hash/hash.hpp>
#include <togo/memory/memory.hpp>
#include <togo/io/types.hpp>
#include <togo/io/io.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource.hpp>
#include <togo/resource/resource_handler.hpp>
#include <togo/resource/resource_manager.hpp>

#include "../common/helpers.hpp"

using namespace togo;

enum : ResourceNameHash {
	RES_TEST_NON_MANIFEST = "non_manifest"_resource_name,

	// data/pkg/test1
	PKG1_TEST_NON_EXISTENT = "non_existent"_resource_name,
	PKG1_TEST_1 = "1"_resource_name,
	PKG1_TEST_2 = "subdir/2"_resource_name,

	// data/pkg/test2
	PKG2_TEST_1 = "1"_resource_name,
	PKG2_TEST_3 = "3"_resource_name,
};

void test(
	ResourceManager& rm,
	ResourceNameHash const name_hash,
	bool const expect_success,
	u32 const x
) {
	TOGO_ASSERTE(!resource_manager::get_resource(
		rm, RES_TYPE_TEST, name_hash
	));
	auto const* test_resource = static_cast<TestResource*>(
		resource_manager::load_resource(rm, RES_TYPE_TEST, name_hash)
	);
	TOGO_ASSERTE(static_cast<bool>(test_resource) == expect_success);
	TOGO_ASSERTE(test_resource == resource_manager::get_resource(
		rm, RES_TYPE_TEST, name_hash
	));
	if (test_resource) {
		TOGO_ASSERTE(test_resource->x == x);
		resource_manager::unload_resource(rm, RES_TYPE_TEST, name_hash);
		test_resource = nullptr;
		TOGO_ASSERTE(!resource_manager::get_resource(
			rm, RES_TYPE_TEST, name_hash
		));
	}
}

signed main() {
	memory_init();

	ResourceManager rm{memory::default_allocator()};
	resource_handler::register_test_resource(rm);
	resource_manager::add_package(rm, "data/pkg/test1");

	// Non-existent, non-manifested
	test(rm, RES_TEST_NON_MANIFEST, false, 0);

	// Non-existent, manifested
	test(rm, PKG1_TEST_NON_EXISTENT, false, 0);

	// Existent, manifest
	test(rm, PKG1_TEST_1, true, 1);
	test(rm, PKG1_TEST_2, true, 2);

	// Additional package
	resource_manager::add_package(rm, "data/pkg/test2");

	// test2/1.test overlaps test1/1.test due to package order
	test(rm, PKG2_TEST_1, true, 42);

	// New resource in test2 that doesn't overlap any in test1
	test(rm, PKG2_TEST_3, true, 3);

	return 0;
}
