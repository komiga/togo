#line 2 "togo/resource/resource_handler/shader.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource.hpp>
#include <togo/resource/resource_handler.hpp>
#include <togo/resource/resource_manager.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/gfx/shader_def.hpp>
#include <togo/serialization/binary_serializer.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/shader_def.hpp>
#include <togo/gfx/renderer.hpp>

namespace togo {

namespace resource_handler {
namespace shader {

namespace {

static void push_sources(
	gfx::ShaderStage& stage,
	gfx::ShaderDef const& def,
	ResourceManager& manager
) {
	for (auto const dep_name : def.prelude) {
		auto const* const dep_def = static_cast<gfx::ShaderDef const*>(
			resource_manager::get_resource(manager, RES_TYPE_SHADER_PRELUDE, dep_name).pointer
		);
		TOGO_DEBUG_ASSERTE(dep_def);
		push_sources(stage, *dep_def, manager);
	}
	StringRef source = gfx::shader_def::shared_source(def);
	if (source.any()) {
		fixed_array::push_back(stage.sources, source);
	}
	source
		= gfx::ShaderStage::Type::vertex == stage.type
		? gfx::shader_def::vertex_source(def)
		: gfx::shader_def::fragment_source(def)
	;
	if (source.any()) {
		fixed_array::push_back(stage.sources, source);
	}
}

static void setup_stage(
	gfx::ShaderStage& stage,
	gfx::ShaderStage::Type const type,
	gfx::ShaderDef const& def,
	ResourceManager& manager
) {
	stage.type = type;
	fixed_array::clear(stage.sources);

	// Push shared configuration if it exists
	if (resource_manager::has_resource(
		manager, RES_TYPE_SHADER_PRELUDE, RES_NAME_SHADER_CONFIG
	)) {
		auto const* const shader_config_def = static_cast<gfx::ShaderDef const*>(
			resource_manager::load_resource(
				manager, RES_TYPE_SHADER_PRELUDE, RES_NAME_SHADER_CONFIG
			).pointer
		);
		if (shader_config_def) {
			push_sources(stage, *shader_config_def, manager);
		}
	}

	// Push main sources
	push_sources(stage, def, manager);
}

} // anonymous namespace

static ResourceValue load(
	void* const type_data,
	ResourceManager& manager,
	ResourcePackage& package,
	ResourceMetadata const& metadata
) {
	auto* const renderer = static_cast<gfx::Renderer*>(type_data);
	auto& def = renderer->_shader_stage;

	{// Deserialize resource
	ResourceStreamLock lock{package, metadata.id};
	BinaryInputSerializer ser{lock.stream()};
	ser % def;
	}

	// Validate
	TOGO_DEBUG_ASSERTE(gfx::shader_def::type(def) == gfx::ShaderDef::TYPE_UNIT);
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

	// Create shader
	gfx::ShaderStage stages[2];
	setup_stage(stages[0], gfx::ShaderStage::Type::vertex, def, manager);
	setup_stage(stages[1], gfx::ShaderStage::Type::fragment, def, manager);
	gfx::ShaderID const id = gfx::renderer::create_shader(renderer, 2, stages);
	TOGO_DEBUG_ASSERTE(id.valid());
	return id._value;
}

static void unload(
	void* const type_data,
	ResourceManager& /*manager*/,
	ResourceValue const resource
) {
	auto* const renderer = static_cast<gfx::Renderer*>(type_data);
	gfx::ShaderID const id{resource.uinteger};
	gfx::renderer::destroy_shader(renderer, id);
}

} // namespace shader
} // namespace resource_handler

void resource_handler::register_shader(
	ResourceManager& rm,
	gfx::Renderer* const renderer
) {
	TOGO_DEBUG_ASSERTE(renderer);
	ResourceHandler const handler{
		RES_TYPE_SHADER,
		renderer,
		resource_handler::shader::load,
		resource_handler::shader::unload
	};
	resource_manager::register_handler(rm, handler);
}

} // namespace togo
