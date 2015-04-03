#line 2 "togo/game/resource/resource_handler/shader_prelude.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/gfx/shader_def.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource.hpp>
#include <togo/game/resource/resource_handler.hpp>
#include <togo/game/resource/resource_manager.hpp>
#include <togo/game/serialization/gfx/shader_def.hpp>

namespace togo {

namespace resource_handler {
namespace shader_prelude {

static ResourceValue load(
	void* const type_data,
	ResourceManager& manager,
	ResourcePackage& package,
	ResourceMetadata const& metadata
) {
	auto* const renderer = static_cast<gfx::Renderer*>(type_data);
	auto& def = *TOGO_CONSTRUCT(
		*renderer->_allocator, gfx::ShaderDef,
		*renderer->_allocator
	);

	{// Deserialize resource
	ResourceStreamLock lock{package, metadata.id};
	BinaryInputSerializer ser{lock.stream()};
	ser % def;
	}
	gfx::shader_def::patch_param_block_names(def);

	// Validate
	TOGO_DEBUG_ASSERTE(gfx::shader_def::type(def) == gfx::ShaderDef::TYPE_PRELUDE);
	TOGO_ASSERTE(
		gfx::renderer::type(renderer) == gfx::RENDERER_TYPE_OPENGL &&
		gfx::shader_def::language(def) == gfx::ShaderDef::LANG_GLSL
	);

	// Load dependencies
	for (auto const dep_name : def.prelude) {
		TOGO_ASSERTF(
			resource_manager::load_resource(manager, RES_TYPE_SHADER_PRELUDE, dep_name).valid(),
			"failed to load shader dependency: [%16lx]",
			dep_name
		);
	}
	return &def;
}

static void unload(
	void* const type_data,
	ResourceManager& /*manager*/,
	ResourceValue const resource
) {
	auto* const renderer = static_cast<gfx::Renderer*>(type_data);
	auto* const def = static_cast<gfx::ShaderDef*>(resource.pointer);
	TOGO_DESTROY(*renderer->_allocator, def);
}

} // namespace shader_prelude
} // namespace resource_handler

/// Register shader_prelude (gfx::ShaderDef) resource handler.
void resource_handler::register_shader_prelude(
	ResourceManager& rm,
	gfx::Renderer* const renderer
) {
	TOGO_DEBUG_ASSERTE(renderer);
	ResourceHandler const handler{
		RES_TYPE_SHADER_PRELUDE,
		SER_FORMAT_VERSION_SHADER_DEF,
		renderer,
		resource_handler::shader_prelude::load,
		resource_handler::shader_prelude::unload
	};
	resource_manager::register_handler(rm, handler);
}

} // namespace togo
