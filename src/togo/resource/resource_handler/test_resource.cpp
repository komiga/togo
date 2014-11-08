#line 2 "togo/resource/resource_handler/test_resource.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/memory/memory.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource_handler.hpp>
#include <togo/resource/resource_manager.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/resource/test_resource.hpp>
#include <togo/serialization/binary_serializer.hpp>

namespace togo {

namespace resource_handler {
namespace test_resource {

static void* load(
	void* type_data,
	ResourceManager& /*manager*/,
	ResourceNameHash /*name_hash*/,
	IReader& stream
) {
	TOGO_DEBUG_ASSERTE(type_data == nullptr);
	TestResource* const test_resource = TOGO_CONSTRUCT(
		memory::default_allocator(), TestResource, {0}
	);

	{// Deserialize resource
	BinaryInputSerializer ser{stream};
	ser % *test_resource;
	}
	return test_resource;
}

static void unload(
	void* type_data,
	ResourceManager& /*manager*/,
	ResourceNameHash /*name_hash*/,
	void* resource
) {
	TOGO_DEBUG_ASSERTE(type_data == nullptr);
	auto const* test_resource = static_cast<TestResource*>(resource);
	TOGO_DESTROY(memory::default_allocator(), test_resource);
}

} // namespace test_resource
} // namespace resource_handler

void resource_handler::register_test_resource(
	ResourceManager& rm
) {
	ResourceHandler const handler{
		RES_TYPE_TEST,
		resource_handler::test_resource::load,
		resource_handler::test_resource::unload
	};
	resource_manager::register_handler(rm, handler, nullptr);
}

} // namespace togo
