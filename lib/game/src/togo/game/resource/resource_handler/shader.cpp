#line 2 "togo/game/resource/resource_handler/shader.cpp"
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
namespace shader {

namespace {

static void push_def(
	gfx::ShaderSpec& spec,
	gfx::ShaderStage& stage,
	gfx::ShaderDef const& def,
	ResourceManager& manager,
	bool const push_param_blocks
) {
	for (auto const dep_name : def.prelude) {
		auto const* const dep_def = static_cast<gfx::ShaderDef const*>(
			resource_manager::find_resource(manager, RES_TYPE_SHADER_PRELUDE, dep_name).pointer
		);
		TOGO_DEBUG_ASSERTE(dep_def);
		push_def(spec, stage, *dep_def, manager, push_param_blocks);
	}

	// Push sources
	StringRef source = gfx::shader_def::shared_source(def);
	if (source.any()) {
		fixed_array::push_back(stage.sources, source);
	}
	source = gfx::shader_def::stage_source(def, stage.type);
	if (source.any()) {
		fixed_array::push_back(stage.sources, source);
	}

	if (push_param_blocks) {
		for (auto const& pb_def : def.fixed_param_blocks) {
			fixed_array::push_back(spec.fixed_param_blocks, pb_def);
		}
		for (auto const& pb_def : def.draw_param_blocks) {
			fixed_array::push_back(spec.draw_param_blocks, pb_def);
		}
	}
}

static void add_stage(
	gfx::ShaderSpec& spec,
	gfx::ShaderStage::Type const type,
	gfx::ShaderDef const& def,
	ResourceManager& manager,
	bool const push_param_blocks
) {
	fixed_array::increase_size(spec.stages, 1);
	gfx::ShaderStage& stage = fixed_array::back(spec.stages);

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
			push_def(spec, stage, *shader_config_def, manager, push_param_blocks);
		}
	}

	// Push graph
	push_def(spec, stage, def, manager, push_param_blocks);
}

static gfx::ParamBlockDef const* find_conflicting_param_block(
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> const& param_blocks,
	gfx::ParamBlockDef const& pb_def,
	unsigned const from_index,
	bool const index_conflicts
) {
	for (unsigned index = from_index; index < fixed_array::size(param_blocks); ++index) {
		auto const& pb_def_it = param_blocks[index];
		if (
			pb_def.name_hash == pb_def_it.name_hash ||
			(index_conflicts && pb_def.index == pb_def_it.index)
		) {
			return &pb_def_it;
		}
	}
	return nullptr;
}

static void check_conflicting_param_blocks(
	StringRef const desc,
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> const& param_blocks_a,
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> const& param_blocks_b,
	bool const index_conflicts
) {
	bool const same = &param_blocks_a == &param_blocks_b;
	for (unsigned index = 0; index < fixed_array::size(param_blocks_a); ++index) {
		auto const* const pb_def_a = &param_blocks_a[index];
		auto const* const pb_def_b = find_conflicting_param_block(
			param_blocks_b, *pb_def_a, same ? (index + 1) : 0, index_conflicts
		);
		TOGO_ASSERTF(
			!pb_def_b,
			"conflicting param blocks (%.*s): {%.*s (%08x), %u} with {%.*s (%08x), %u}",
			desc.size, desc.data,
			pb_def_a->name.size, pb_def_a->name.data, pb_def_a->name_hash, pb_def_a->index,
			pb_def_b->name.size, pb_def_b->name.data, pb_def_b->name_hash, pb_def_b->index
		);
	}
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
	gfx::shader_def::patch_param_block_names(def);

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

	// Build specification
	gfx::ShaderSpec spec;
	add_stage(spec, gfx::ShaderStage::Type::vertex, def, manager, true);
	add_stage(spec, gfx::ShaderStage::Type::fragment, def, manager, false);

	// Validate param blocks
	check_conflicting_param_blocks("fixed", spec.fixed_param_blocks, spec.fixed_param_blocks, true);
	check_conflicting_param_blocks("fixed -> draw", spec.fixed_param_blocks, spec.draw_param_blocks, false);
	check_conflicting_param_blocks("draw", spec.draw_param_blocks, spec.draw_param_blocks, false);

	{// Reindexed draw param blocks
	unsigned index = 0;
	for (auto& pb_def : spec.draw_param_blocks) {
		pb_def.index = index++;
	}}

	// Create shader
	gfx::ShaderID const id = gfx::renderer::create_shader(renderer, spec);
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

/// Register shader (gfx::ShaderID) resource handler.
void resource_handler::register_shader(
	ResourceManager& rm,
	gfx::Renderer* const renderer
) {
	TOGO_DEBUG_ASSERTE(renderer);
	ResourceHandler const handler{
		RES_TYPE_SHADER,
		SER_FORMAT_VERSION_SHADER_DEF,
		renderer,
		resource_handler::shader::load,
		resource_handler::shader::unload
	};
	resource_manager::register_handler(rm, handler);
}

} // namespace togo
