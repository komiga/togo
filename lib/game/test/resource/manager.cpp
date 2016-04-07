
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/io/io.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource.hpp>
#include <togo/game/resource/resource_handler.hpp>
#include <togo/game/resource/resource_manager.hpp>

#include <togo/support/test.hpp>

using namespace togo;
using namespace togo::game;

enum : ResourceNameHash {
	PKG1_TEST_NON_EXISTENT = "non_existent"_resource_name,

	// data/pkg/test1
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
	s64 const x
) {
	TOGO_ASSERTE(!resource_manager::find_active(
		rm, RES_TYPE_TEST, name_hash
	));
	auto const* test_resource = resource_manager::load(rm, RES_TYPE_TEST, name_hash);
	TOGO_ASSERTE(static_cast<bool>(test_resource) == expect_success);
	TOGO_ASSERTE(test_resource == resource_manager::find_active(
		rm, RES_TYPE_TEST, name_hash
	));
	if (test_resource) {
		TOGO_ASSERTE(static_cast<TestResource*>(test_resource->value.pointer)->x == x);
		resource_manager::unload(rm, RES_TYPE_TEST, name_hash);
		test_resource = nullptr;
		TOGO_ASSERTE(!resource_manager::find_active(
			rm, RES_TYPE_TEST, name_hash
		));
	}
}

signed main() {
	memory_init();

	ResourceManager rm{"data/pkg/", memory::default_allocator()};
	resource_handler::register_test(rm);
	resource_manager::add_package(rm, "test1");

	// Non-existent, manifested
	test(rm, PKG1_TEST_NON_EXISTENT, false, 0);

	// Existent, manifest
	test(rm, PKG1_TEST_1, true, 1);
	test(rm, PKG1_TEST_2, true, 2);

	// Additional package
	resource_manager::add_package(rm, "test2");

	// test2/1.test overlaps test1/1.test due to package order
	test(rm, PKG2_TEST_1, true, 42);

	// New resource in test2 that doesn't overlap any in test1
	test(rm, PKG2_TEST_3, true, 3);

	return 0;
}
