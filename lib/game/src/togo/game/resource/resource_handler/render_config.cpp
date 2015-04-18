#line 2 "togo/game/resource/resource_handler/render_config.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/resource/resource.hpp>
#include <togo/game/resource/resource_handler.hpp>
#include <togo/game/resource/resource_manager.hpp>
#include <togo/game/serialization/gfx/render_config.hpp>

namespace togo {
namespace game {

namespace resource_handler {
namespace render_config {

static ResourceValue load(
	void* const /*type_data*/,
	ResourceManager& /*manager*/,
	ResourcePackage& package,
	ResourceMetadata const& metadata
) {
	auto* const packed_config = TOGO_CONSTRUCT(
		memory::default_allocator(), gfx::PackedRenderConfig, {
		memory::default_allocator()
	});

	{// Deserialize resource
	ResourceStreamLock lock{package, metadata.id};
	BinaryInputSerializer ser{lock.stream()};
	ser % *packed_config;
	}
	return packed_config;
}

static void unload(
	void* const /*type_data*/,
	ResourceManager& /*manager*/,
	ResourceValue const resource
) {
	auto const* packed_config = static_cast<gfx::PackedRenderConfig*>(resource.pointer);
	TOGO_DESTROY(memory::default_allocator(), packed_config);
}

} // namespace render_config
} // namespace resource_handler

/// Register render_config (gfx::RenderConfig) resource handler.
void resource_handler::register_render_config(
	ResourceManager& rm,
	gfx::Renderer* const renderer
) {
	TOGO_DEBUG_ASSERTE(renderer != nullptr);
	ResourceHandler const handler{
		RES_TYPE_RENDER_CONFIG,
		SER_FORMAT_VERSION_RENDER_CONFIG,
		renderer,
		resource_handler::render_config::load,
		resource_handler::render_config::unload
	};
	resource_manager::register_handler(rm, handler);
}

} // namespace game
} // namespace togo
