#line 2 "togo/resource/resource_handler/render_config.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/memory/memory.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource.hpp>
#include <togo/resource/resource_handler.hpp>
#include <togo/resource/resource_manager.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/binary_serializer.hpp>
#include <togo/serialization/gfx/render_config.hpp>

namespace togo {

namespace resource_handler {
namespace render_config {

static ResourceValue load(
	void* const type_data,
	ResourceManager& /*manager*/,
	ResourcePackage& package,
	ResourceMetadata const& metadata
) {
	TOGO_DEBUG_ASSERTE(type_data != nullptr);
	auto* const renderer = static_cast<gfx::Renderer*>(type_data);
	auto* const render_config = TOGO_CONSTRUCT_DEFAULT(
		memory::default_allocator(), gfx::RenderConfig
	);

	{// Deserialize resource
	ResourceStreamLock lock{package, metadata.id};
	BinaryInputSerializer ser{lock.stream()};
	ser % *render_config;

	// Deserialize generator units
	for (auto& pipe : render_config->pipes) {
	for (auto& layer : pipe.layers) {
	for (auto& gen_unit : layer.layout) {
		gen_unit.data = nullptr;
		auto* gen_def = gfx::renderer::find_generator_def(renderer, gen_unit.name_hash);
		TOGO_ASSERTE(gen_def);
		gen_def->func_read(*gen_def, ser, gen_unit);
	}}}}
	return render_config;
}

static void unload(
	void* const /*type_data*/,
	ResourceManager& /*manager*/,
	ResourceValue const resource
) {
	auto const* render_config = static_cast<gfx::RenderConfig*>(resource.pointer);
	TOGO_DESTROY(memory::default_allocator(), render_config);
}

} // namespace render_config
} // namespace resource_handler

void resource_handler::register_render_config(
	ResourceManager& rm,
	gfx::Renderer* const renderer
) {
	ResourceHandler const handler{
		RES_TYPE_RENDER_CONFIG,
		SER_FORMAT_VERSION_RENDER_CONFIG,
		renderer,
		resource_handler::render_config::load,
		resource_handler::render_config::unload
	};
	resource_manager::register_handler(rm, handler);
}

} // namespace togo
