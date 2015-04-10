#line 2 "togo/tool_res_build/resource_compiler/render_config.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/temp_allocator.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/collection/hash_map.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/serialization/gfx/render_config.hpp>
#include <togo/tool_res_build/resource_compiler.hpp>
#include <togo/tool_res_build/compiler_manager.hpp>
#include <togo/tool_res_build/gfx_compiler.hpp>

namespace togo {
namespace tool_res_build {

namespace resource_compiler {
namespace render_config {

inline static bool is_kvs_valid_and_nonempty(
	KVS const* const kvs,
	KVSType const type
) {
	return kvs && kvs::is_type(*kvs, type) && kvs::any(*kvs);
}

// static bool find_render_resource(
// 	gfx::RenderConfig const& /*render_config*/,
// 	gfx::Viewport const& /*viewport*/,
// 	hash32 const render_resource
// ) {
// 	enum : hash32 {
// 		RT_BACK_BUFFER = "back_buffer"_hash32
// 	};
// 	// TODO: Type distinction (DST, RT, etc.)
// 	if (render_resource == RT_BACK_BUFFER) {
// 		return true;
// 	}

// 	// TODO: Check shared and viewport resources (not yet in-struct)
// 	return false;
// }

static bool read_pipe(
	GfxCompiler& gfx_compiler,
	gfx::RenderConfig& render_config,
	KVS const& k_pipe
) {
	if (!kvs::is_node(k_pipe) || !kvs::is_named(k_pipe)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"pipe '%.*s' is either not a node or unnamed\n",
			kvs::name_size(k_pipe), kvs::name(k_pipe)
		);
		return false;
	}

	// Fetch and validate structure
	KVS const* const k_layers = kvs::find(k_pipe, "layers");
	if (!is_kvs_valid_and_nonempty(k_layers, KVSType::node)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: no layers defined for pipe '%.*s'\n",
			kvs::name_size(k_pipe), kvs::name(k_pipe)
		);
		return false;
	}

	fixed_array::resize(render_config.pipes, fixed_array::size(render_config.pipes) + 1);
	gfx::Pipe& pipe = fixed_array::back(render_config.pipes);

	// Read name
	string::copy(pipe.name, kvs::name_ref(k_pipe));
	pipe.name_hash = hash::calc32(pipe.name);

	{// Read layers
	KVS const* k_layer_rts;
	KVS const* k_layer_dst;
	KVS const* k_layer_order;
	KVS const* k_layer_layout;
	KVS const* k_layer_layout_generators;

	unsigned seq_base = 0;
	for (KVS const& k_layer : *k_layers) {
		if (!kvs::is_node(k_layer) || !kvs::is_named(k_layer)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"layer '%.*s' in pipe '%.*s' is either not a node or unnamed\n",
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			return false;
		}

		// Fetch and validate structure
		k_layer_rts = kvs::find(k_layer, "rts");
		if (!is_kvs_valid_and_nonempty(k_layer_rts, KVSType::array)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"no rts defined for layer '%.*s' in pipe '%.*s'\n",
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			return false;
		} else if (kvs::size(*k_layer_rts) > TOGO_GFX_PIPE_NUM_LAYERS) {
			TOGO_LOG_ERRORF(
				"malformed render_config: too many RTs defined (max = %u)\n",
				TOGO_GFX_PIPE_NUM_LAYERS
			);
			return false;
		}

		k_layer_dst = kvs::find(k_layer, "dst");
		if (!k_layer_dst || !kvs::is_type_any(*k_layer_dst, KVSType::null | KVSType::string)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"no dst defined for layer '%.*s' in pipe '%.*s'\n",
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			return false;
		}

		k_layer_order = kvs::find(k_layer, "order");
		if (k_layer_order && !kvs::is_string(*k_layer_order)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"type of 'order' must be string for layer '%.*s' in pipe '%.*s'\n",
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			return false;
		}

		k_layer_layout = kvs::find(k_layer, "layout");
		if (!is_kvs_valid_and_nonempty(k_layer_layout, KVSType::node)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"no layout defined for layer '%.*s' in pipe '%.*s'\n",
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			return false;
		}

		k_layer_layout_generators = kvs::find(*k_layer_layout, "generators");
		if (!is_kvs_valid_and_nonempty(k_layer_layout_generators, KVSType::array)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"no generators defined for layer '%.*s' in pipe '%.*s'\n",
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			return false;
		} else if (kvs::size(*k_layer_layout_generators) > TOGO_GFX_LAYER_NUM_GENERATORS) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"too many generators defined for layer '%.*s' in pipe '%.*s' (max = %u)\n",
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe),
				TOGO_GFX_LAYER_NUM_GENERATORS
			);
			return false;
		}

		fixed_array::resize(pipe.layers, fixed_array::size(pipe.layers) + 1);
		gfx::Layer& layer = fixed_array::back(pipe.layers);

		// Read name
		string::copy(layer.name, kvs::name_ref(k_layer));
		layer.name_hash = hash::calc32(layer.name);

		// Read RTs
		for (KVS const& k_rt : *k_layer_rts) {
			if (!kvs::is_string(k_rt) || kvs::string_size(k_rt) == 0) {
				TOGO_LOG_ERRORF(
					"malformed render_config: "
					"render target '%.*s' invalid for layer '%.*s' in pipe '%.*s'\n",
					kvs::string_size(k_rt), kvs::string(k_rt),
					kvs::name_size(k_layer), kvs::name(k_layer),
					kvs::name_size(k_pipe), kvs::name(k_pipe)
				);
				return false;
			}

			hash32 const rt_hash = hash::calc32(kvs::string_ref(k_rt));
			fixed_array::push_back(layer.rts, rt_hash);
		}

		// Read DST
		layer.dst
			= (kvs::is_null(*k_layer_dst) || kvs::string_size(*k_layer_dst) == 0)
			? hash::IDENTITY32
			: hash::calc32(kvs::string_ref(*k_layer_dst))
		;

		// Read order
		if (k_layer_order) {
			StringRef const order_name = kvs::string_ref(*k_layer_order);
			if (string::compare_equal(order_name, "back_front")) {
				layer.order = gfx::Layer::Order::back_front;
			} else if (string::compare_equal(order_name, "front_back")) {
				layer.order = gfx::Layer::Order::front_back;
			} else {
				TOGO_LOG_ERRORF(
					"order '%.*s' invalid for layer '%.*s' in pipe '%.*s'",
					order_name.size, order_name.data,
					kvs::name_size(k_layer), kvs::name(k_layer),
					kvs::name_size(k_pipe), kvs::name(k_pipe)
				);
				return false;
			}
		} else {
			layer.order = gfx::Layer::Order::back_front;
		}

		{// Read layout
		KVS const* k_generator_unit;
		StringRef unit_name;
		for (auto const& k_generator : *k_layer_layout_generators) {
			// Fetch and validate structure
			k_generator_unit = kvs::find(k_generator, "unit");
			if (!k_generator_unit || !kvs::is_string(*k_generator_unit)) {
				TOGO_LOG_ERRORF(
					"malformed render_config: "
					"generator unit not defined for layout of layer '%.*s' in pipe '%.*s'\n",
					kvs::name_size(k_layer), kvs::name(k_layer),
					kvs::name_size(k_pipe), kvs::name(k_pipe)
				);
				return false;
			}

			unit_name = kvs::string_ref(*k_generator_unit);
			gfx::GeneratorUnit gen_unit{
				gfx::hash_generator_name(unit_name),
				const_cast<KVS*>(k_generator_unit),
				nullptr
			};
			if (!gfx_compiler::find_generator_compiler(gfx_compiler, gen_unit.name_hash)) {
				TOGO_LOG_ERRORF(
					"malformed render_config: "
					"no compiler found for generator unit '%.*s' for layout of layer '%.*s' in pipe '%.*s'\n",
					unit_name.size, unit_name.data,
					kvs::name_size(k_layer), kvs::name(k_layer),
					kvs::name_size(k_pipe), kvs::name(k_pipe)
				);
				return false;
			}
			fixed_array::push_back(layer.layout, gen_unit);
		}}

		layer.seq_base = seq_base;
		seq_base += fixed_array::size(layer.layout);
		if (seq_base > TOGO_GFX_KEY_SEQ_MAX) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"too many generators defined (%u/%lu); at layer '%.*s' in pipe '%.*s'\n",
				seq_base,
				TOGO_GFX_KEY_SEQ_MAX,
				kvs::name_size(k_layer), kvs::name(k_layer),
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			return false;
		}
	}}

	return true;
}

static bool read_viewport(
	gfx::RenderConfig& render_config,
	KVS const& k_pipes,
	KVS const& k_viewport,
	KVS const*& k_pipe
) {
	if (!kvs::is_node(k_viewport) || !kvs::is_named(k_viewport)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"viewport '%.*s' is either not a node or unnamed\n",
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}

	// Fetch and validate structure
	KVS const* const k_vp_output_rt = kvs::find(k_viewport, "output_rt");
	if (!k_vp_output_rt || !kvs::is_string(*k_vp_output_rt)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: no output_rt defined for viewport '%.*s'\n",
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}

	KVS const* const k_vp_output_dst = kvs::find(k_viewport, "output_dst");
	if (!k_vp_output_dst || !kvs::is_type_any(*k_vp_output_dst, KVSType::null | KVSType::string)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: no output_dst defined for viewport '%.*s'\n",
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}

	KVS const* const k_vp_pipe = kvs::find(k_viewport, "pipe");
	if (!k_vp_pipe || !kvs::is_string(*k_vp_pipe)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: no pipe defined for viewport '%.*s'\n",
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}

	// TODO: Read resources

	fixed_array::resize(render_config.viewports, fixed_array::size(render_config.viewports) + 1);
	gfx::Viewport& viewport = fixed_array::back(render_config.viewports);

	// Read name
	string::copy(viewport.name, kvs::name_ref(k_viewport));
	viewport.name_hash = hash::calc32(viewport.name);

	// Read output_rt
	viewport.output_rt = hash::calc32(kvs::string_ref(*k_vp_output_rt));

	// Read output_dst
	viewport.output_dst
		= (kvs::is_null(*k_vp_output_dst) || kvs::string_size(*k_vp_output_dst) == 0)
		? hash::IDENTITY32
		: hash::calc32(kvs::string_ref(*k_vp_output_dst))
	;

	{// Check pipe existence
	k_pipe = kvs::find(k_pipes, kvs::string_ref(*k_vp_pipe));
	if (!is_kvs_valid_and_nonempty(k_pipe, KVSType::node)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"pipe '%.*s' (referenced from viewport '%.*s') does not exist\n",
			kvs::string_size(*k_vp_pipe), kvs::string(*k_vp_pipe),
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}}

	return true;
}

static bool compile(
	void* type_data,
	CompilerManager& /*manager*/,
	PackageCompiler& /*package*/,
	ResourceCompilerMetadata const& /*metadata*/,
	IReader& in_stream,
	IWriter& out_stream
) {
	auto& gfx_compiler = *static_cast<GfxCompiler*>(type_data);
	gfx::RenderConfig* render_config = nullptr;
	KVS k_root{};

	KVS const* k_pipes;
	KVS const* k_viewports;

	TempAllocator<256> pipes_used_allocator;
	HashMap<hash32, KVS const*> pipes_used{pipes_used_allocator};

	{// Read source
	ParserInfo pinfo;
	bool const read_success = kvs::read(k_root, in_stream, pinfo);
	if (!read_success) {
		TOGO_LOG_ERRORF(
			"failed to read render_config: [%2u,%2u]: %s\n",
			pinfo.line, pinfo.column, pinfo.message
		);
		goto l_failed;
	}}

	// Fetch and validate structure
	k_pipes = kvs::find(k_root, "pipes");
	if (!is_kvs_valid_and_nonempty(k_pipes, KVSType::node)) {
		TOGO_LOG_ERROR("malformed render_config: no pipes defined\n");
		goto l_failed;
	}

	k_viewports = kvs::find(k_root, "viewports");
	if (!is_kvs_valid_and_nonempty(k_viewports, KVSType::node)) {
		TOGO_LOG_ERROR("malformed render_config: no viewports defined\n");
		goto l_failed;
	} else if (kvs::size(*k_viewports) > TOGO_GFX_CONFIG_NUM_VIEWPORTS) {
		TOGO_LOG_ERRORF(
			"malformed render_config: too many viewports defined (max = %u)\n",
			TOGO_GFX_CONFIG_NUM_VIEWPORTS
		);
		goto l_failed;
	}

	render_config = TOGO_CONSTRUCT_DEFAULT(
		memory::default_allocator(), gfx::RenderConfig
	);

	// TODO: Read shared_resources
	// TODO: Check for duplicate pipes and viewports

	// Read viewports
	for (KVS const& k_viewport : *k_viewports) {
		KVS const* k_pipe = nullptr;
		if (!read_viewport(*render_config, *k_pipes, k_viewport, k_pipe)) {
			goto l_failed;
		}
		if (hash_map::size(pipes_used) == TOGO_GFX_CONFIG_NUM_PIPES) {
			TOGO_LOG_ERRORF(
				"malformed render_config: too many pipes used (max = %u)\n",
				TOGO_GFX_CONFIG_NUM_PIPES
			);
			goto l_failed;
		}
		hash_map::set(
			pipes_used,
			fixed_array::back(render_config->viewports).pipe,
			k_pipe
		);
	}

	// Read pipes
	for (auto const& entry : pipes_used) {
		if (!read_pipe(gfx_compiler, *render_config, *entry.value)) {
			goto l_failed;
		}
	}

	// TODO: Validate render resource references in pipes

	{// Serialize resource
	BinaryOutputSerializer ser{out_stream};
	ser % *render_config;

	// Serialize generator units
	for (auto const& pipe : render_config->pipes) {
	for (auto const& layer : pipe.layers) {
	for (auto const& gen_unit : layer.layout) {
		auto* gen_compiler = gfx_compiler::find_generator_compiler(
			gfx_compiler, gen_unit.name_hash
		);
		TOGO_ASSERTE(gen_compiler);
		if (!gen_compiler->func_write(*gen_compiler, ser, gen_unit)) {
			goto l_failed;
		}
	}}}}

	TOGO_DESTROY(memory::default_allocator(), render_config);
	return true;

l_failed:
	TOGO_DESTROY(memory::default_allocator(), render_config);
	return false;
}

} // namespace render_config
} // namespace resource_compiler

/// Register render_config resource compiler.
void resource_compiler::register_render_config(
	CompilerManager& cm,
	GfxCompiler& gfx_compiler
) {
	ResourceCompiler const compiler{
		RES_TYPE_RENDER_CONFIG,
		SER_FORMAT_VERSION_RENDER_CONFIG,
		&gfx_compiler,
		resource_compiler::render_config::compile
	};
	compiler_manager::register_compiler(cm, compiler);
}

} // namespace tool_res_build
} // namespace togo
