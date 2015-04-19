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
#include <togo/core/io/memory_stream.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/serialization/gfx/render_config.hpp>
#include <togo/tool_res_build/resource_compiler.hpp>
#include <togo/tool_res_build/compiler_manager.hpp>
#include <togo/tool_res_build/gfx_compiler.hpp>

#include <cmath>

namespace togo {
namespace tool_res_build {

namespace resource_compiler {
namespace render_config {

enum : hash32 {
	RT_BACK_BUFFER = "back_buffer"_hash32
};

inline static bool is_kvs_valid_and_nonempty(
	KVS const* const kvs,
	KVSType const type
) {
	return kvs && kvs::is_type(*kvs, type) && kvs::any(*kvs);
}

static bool find_rc_resource(
	gfx::RenderConfig const& render_config,
	gfx::Viewport const& /*viewport*/,
	hash32 const name_hash
) {
	if (name_hash == RT_BACK_BUFFER) {
		return true;
	}
	for (auto const& resource : render_config.shared_resources) {
		if (name_hash == resource.name_hash) {
			return true;
		}
	}
	// TODO: Check viewport resources
	return false;
}

static bool read_resource_render_target(
	gfx::RenderConfigResource& resource,
	gfx::RenderConfig& /*render_config*/,
	KVS const& k_resource
) {
	auto& spec = resource.data.render_target;
	resource.properties = gfx::RenderConfigResource::TYPE_RENDER_TARGET;

	// Fetch and validate structure
	KVS const* const k_format = kvs::find(k_resource, "format");
	if (!k_format || !kvs::is_type(*k_format, KVSType::string)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': 'format' not defined\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	KVS const* const k_size = kvs::find(k_resource, "size");
	if (k_size && !kvs::is_type(*k_size, KVSType::vec2)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': 'size' must be a vec2 if defined\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	KVS const* const k_scale = kvs::find(k_resource, "scale");
	if (k_scale && !kvs::is_type(*k_scale, KVSType::vec2)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': 'scale' must be a vec2 if defined\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	KVS const* const k_clear = kvs::find(k_resource, "clear");
	if (k_clear && !kvs::is_type(*k_clear, KVSType::boolean)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': 'clear' must be a boolean if defined\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	KVS const* const k_double_buffered = kvs::find(k_resource, "double_buffered");
	if (k_double_buffered && !kvs::is_type(*k_double_buffered, KVSType::boolean)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': 'double_buffered' must be a boolean if defined\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	if (k_scale && k_size) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': only 'size' or 'scale' can be defined, not both\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	} else if (!k_scale && !k_size) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': no dimensions; 'size' or 'scale' must be defined\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	// Read format
	switch (hash::calc32(kvs::string_ref(*k_format))) {
	case "RGB8"_hash32: spec.format = gfx::RenderTargetFormat::rgb8; break;
	case "RGBA8"_hash32: spec.format = gfx::RenderTargetFormat::rgba8; break;
	case "D16"_hash32: spec.format = gfx::RenderTargetFormat::d16; break;
	case "D32"_hash32: spec.format = gfx::RenderTargetFormat::d32; break;
	case "D24S8"_hash32: spec.format = gfx::RenderTargetFormat::d24s8; break;
	default:
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s': format '%.*s' not recognized\n",
			kvs::name_size(k_resource), kvs::name(k_resource),
			kvs::string_size(*k_format), kvs::string(*k_format)
		);
		return false;
	}

	// Read size
	if (k_size) {
		Vec2 size = kvs::vec2(*k_size);
		spec.dim_x = std::round(size.x);
		spec.dim_y = std::round(size.y);
		if (!(spec.dim_x > 0.0f && spec.dim_y > 0.0f)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"resource '%.*s': size must be greater than (0, 0)\n",
				kvs::name_size(k_resource), kvs::name(k_resource)
			);
			return false;
		}
	}

	// Read scale
	if (k_scale) {
		spec.properties |= gfx::RenderTargetSpec::F_SCALE;
		Vec2 scale = kvs::vec2(*k_scale);
		spec.dim_x = scale.x;
		spec.dim_y = scale.y;
		if (!(
			spec.dim_x > 0.0f && spec.dim_x <= 1.0f &&
			spec.dim_y > 0.0f && spec.dim_y <= 1.0f
		)) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"resource '%.*s': scale must be in the range ((0, 0), (1, 1)]\n",
				kvs::name_size(k_resource), kvs::name(k_resource)
			);
			return false;
		}
	}

	// Read clear
	if (!k_clear || kvs::boolean(*k_clear)) {
		spec.properties |= gfx::RenderTargetSpec::F_CLEAR;
	}

	// Read double_buffered
	if (k_double_buffered && kvs::boolean(*k_double_buffered)) {
		spec.properties |= gfx::RenderTargetSpec::F_DOUBLE_BUFFERED;
	}

	return true;
}

static bool read_resource(
	gfx::RenderConfigResource& resource,
	gfx::RenderConfig& render_config,
	KVS const& k_resource
) {
	if (!kvs::is_node(k_resource) || !kvs::is_named(k_resource)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"resource '%.*s' is either not a node or unnamed\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	// Fetch and validate structure
	KVS const* const k_type = kvs::find(k_resource, "type");
	if (!k_type || !kvs::is_type(*k_type, KVSType::string)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"no type defined for resource '%.*s'\n",
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}

	// Read name
	string::copy(resource.name, kvs::name_ref(k_resource));
	resource.name_hash = hash::calc32(resource.name);

	// Read data
	switch (hash::calc32(kvs::string_ref(*k_type))) {
	case "render_target"_hash32:
		return read_resource_render_target(resource, render_config, k_resource);

	default:
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"type '%.*s' not recognized for resource '%.*s'\n",
			kvs::string_size(*k_type), kvs::string(*k_type),
			kvs::name_size(k_resource), kvs::name(k_resource)
		);
		return false;
	}
}

static bool read_pipe(
	GfxCompiler& gfx_compiler,
	gfx::RenderConfig& render_config,
	KVS const& k_pipe,
	Array<u32> const& users
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

	fixed_array::increase_size(render_config.pipes, 1);
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
		} else if (
			k_layer_dst &&
			kvs::is_type(*k_layer_dst, KVSType::string) &&
			kvs::string_size(*k_layer_dst) == 0
		) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"dst must be non-empty as a string for layer '%.*s' in pipe '%.*s'\n",
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

		fixed_array::increase_size(pipe.layers, 1);
		gfx::Layer& layer = fixed_array::back(pipe.layers);

		// Read name
		string::copy(layer.name, kvs::name_ref(k_layer));
		layer.name_hash = hash::calc32(layer.name);

		// Read RTs
		for (KVS const& k_rt : *k_layer_rts) {
			if (!kvs::is_string(k_rt) || kvs::string_size(k_rt) == 0) {
				TOGO_LOG_ERRORF(
					"malformed render_config: "
					"rts contains a value that is not a string or is empty "
					"for layer '%.*s' in pipe '%.*s'\n",
					kvs::name_size(k_layer), kvs::name(k_layer),
					kvs::name_size(k_pipe), kvs::name(k_pipe)
				);
				return false;
			}

			hash32 const rt = hash::calc32(kvs::string_ref(k_rt));
			for (auto const rt_existing : layer.rts) {
				if (rt == rt_existing) {
					TOGO_LOG_ERRORF(
						"malformed render_config: "
						"render target (rts) '%.*s' duplicated "
						"in layer '%.*s' in pipe '%.*s'\n",
						kvs::string_size(k_rt), kvs::string(k_rt),
						kvs::name_size(k_layer), kvs::name(k_layer),
						kvs::name_size(k_pipe), kvs::name(k_pipe)
					);
					return false;
				}
			}
			if (rt == RT_BACK_BUFFER) {
				TOGO_LOG_ERRORF(
					"malformed render_config: "
					"rts cannot contain 'back_buffer' for layer '%.*s' in pipe '%.*s'\n",
					kvs::name_size(k_layer), kvs::name(k_layer),
					kvs::name_size(k_pipe), kvs::name(k_pipe)
				);
				return false;
			}
			for (auto const viewport_index : users) {
				auto& viewport = render_config.viewports[viewport_index];
				if (!find_rc_resource(render_config, viewport, rt)) {
					TOGO_LOG_ERRORF(
						"malformed render_config: "
						"render target (rts) '%.*s' does not exist "
						"for layer '%.*s' in pipe '%.*s' in context of viewport '%.*s'\n",
						kvs::string_size(k_rt), kvs::string(k_rt),
						kvs::name_size(k_layer), kvs::name(k_layer),
						kvs::name_size(k_pipe), kvs::name(k_pipe),
						string::size(viewport.name), begin(viewport.name)
					);
					return false;
				}
			}
			fixed_array::push_back(layer.rts, rt);
		}

		// Read DST
		layer.dst
			= kvs::is_null(*k_layer_dst)
			? hash::IDENTITY32
			: hash::calc32(kvs::string_ref(*k_layer_dst))
		;

		if (layer.dst != hash::IDENTITY32) {
			for (auto const viewport_index : users) {
				auto& viewport = render_config.viewports[viewport_index];
				if (!find_rc_resource(render_config, viewport, layer.dst)) {
					TOGO_LOG_ERRORF(
						"malformed render_config: "
						"depth-stencil target (dst) '%.*s' does not exist "
						"for layer '%.*s' in pipe '%.*s' in context of viewport '%.*s'\n",
						kvs::string_size(*k_layer_dst), kvs::string(*k_layer_dst),
						kvs::name_size(k_layer), kvs::name(k_layer),
						kvs::name_size(k_pipe), kvs::name(k_pipe),
						string::size(viewport.name), begin(viewport.name)
					);
					return false;
				}
			}
		}

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
				0,
				const_cast<KVS*>(&k_generator),
				nullptr
			};
			if (!gfx_compiler::find_generator_compiler(gfx_compiler, gen_unit.name_hash)) {
				TOGO_LOG_ERRORF(
					"malformed render_config: "
					"no compiler found for generator unit '%.*s' "
					"for layout of layer '%.*s' in pipe '%.*s'\n",
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
	} else if (
		k_vp_output_dst &&
		kvs::is_type(*k_vp_output_dst, KVSType::string) &&
		kvs::string_size(*k_vp_output_dst) == 0
	) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"output_dst must be non-empty as a string for viewport '%.*s'\n",
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

	fixed_array::increase_size(render_config.viewports, 1);
	gfx::Viewport& viewport = fixed_array::back(render_config.viewports);

	// Read name
	string::copy(viewport.name, kvs::name_ref(k_viewport));
	viewport.name_hash = hash::calc32(viewport.name);

	// Read output_rt
	viewport.output_rt = hash::calc32(kvs::string_ref(*k_vp_output_rt));
	if (!find_rc_resource(render_config, viewport, viewport.output_rt)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"output_rt '%.*s' does not exist for viewport '%.*s'\n",
			kvs::string_size(*k_vp_output_rt), kvs::string(*k_vp_output_rt),
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}

	// Read output_dst
	viewport.output_dst
		= kvs::is_null(*k_vp_output_dst)
		? hash::IDENTITY32
		: hash::calc32(kvs::string_ref(*k_vp_output_dst))
	;
	if (
		viewport.output_dst != hash::IDENTITY32 &&
		!find_rc_resource(render_config, viewport, viewport.output_dst)
	) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"output_dst '%.*s' does not exist for viewport '%.*s'\n",
			kvs::string_size(*k_vp_output_dst), kvs::string(*k_vp_output_dst),
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}

	// Check pipe existence
	k_pipe = kvs::find(k_pipes, kvs::string_ref(*k_vp_pipe));
	if (!is_kvs_valid_and_nonempty(k_pipe, KVSType::node)) {
		TOGO_LOG_ERRORF(
			"malformed render_config: "
			"pipe '%.*s' (referenced from viewport '%.*s') does not exist\n",
			kvs::string_size(*k_vp_pipe), kvs::string(*k_vp_pipe),
			kvs::name_size(k_viewport), kvs::name(k_viewport)
		);
		return false;
	}

	// Use pipe name hash until pipes are read
	viewport.pipe = hash::calc32(kvs::string_ref(*k_vp_pipe));

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
	bool success = false;
	auto& gfx_compiler = *static_cast<GfxCompiler*>(type_data);
	gfx::PackedRenderConfig* packed = nullptr;
	KVS k_root{};

	TempAllocator<512> temp_allocator;
	HashMap<KVSNameHash, KVS const*> used{temp_allocator};
	hash_map::reserve(used, 16);

	KVS const* k_shared_resources;
	KVS const* k_pipes;
	KVS const* k_viewports;

	{// Read source
	ParserInfo pinfo;
	bool const read_success = kvs::read_text(k_root, in_stream, pinfo);
	if (!read_success) {
		TOGO_LOG_ERRORF(
			"failed to read render_config: [%2u,%2u]: %s\n",
			pinfo.line, pinfo.column, pinfo.message
		);
		goto l_failed;
	}}

	// Fetch and validate structure
	k_shared_resources = kvs::find(k_root, "shared_resources");
	if (k_shared_resources && !kvs::is_type(*k_shared_resources, KVSType::node)) {
		TOGO_LOG_ERROR("malformed render_config: 'shared_resources' must be a node if defined\n");
		goto l_failed;
	}

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

	packed = TOGO_CONSTRUCT(
		memory::default_allocator(), gfx::PackedRenderConfig, {
		memory::default_allocator()
	});

	// Check for duplicate viewports
	for (KVS const& k_viewport : *k_viewports) {
		if (hash_map::has(used, kvs::name_hash(k_viewport))) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"viewport '%.*s' defined again",
				kvs::name_size(k_viewport), kvs::name(k_viewport)
			);
			goto l_failed;
		}
		hash_map::set(used, kvs::name_hash(k_viewport), &k_viewport);
	}

	// Check for duplicate pipes
	hash_map::clear(used);
	for (KVS const& k_pipe : *k_pipes) {
		if (hash_map::has(used, kvs::name_hash(k_pipe))) {
			TOGO_LOG_ERRORF(
				"malformed render_config: "
				"pipe '%.*s' defined again",
				kvs::name_size(k_pipe), kvs::name(k_pipe)
			);
			goto l_failed;
		}
		hash_map::set(used, kvs::name_hash(k_pipe), &k_pipe);
	}

	// Read shared resources
	if (k_shared_resources) {
		for (KVS const& k_resource : *k_shared_resources) {
			fixed_array::increase_size(packed->config.shared_resources, 1);
			if (!read_resource(
				fixed_array::back(packed->config.shared_resources),
				packed->config,
				k_resource
			)) {
				goto l_failed;
			}
		}
	}

	// Read viewports
	hash_map::clear(used);
	for (KVS const& k_viewport : *k_viewports) {
		KVS const* k_pipe = nullptr;
		if (!read_viewport(packed->config, *k_pipes, k_viewport, k_pipe)) {
			goto l_failed;
		}
		if (hash_map::size(used) == TOGO_GFX_CONFIG_NUM_PIPES) {
			TOGO_LOG_ERRORF(
				"malformed render_config: too many pipes used (max = %u)\n",
				TOGO_GFX_CONFIG_NUM_PIPES
			);
			goto l_failed;
		}
		hash_map::set(
			used,
			fixed_array::back(packed->config.viewports).pipe,
			k_pipe
		);
	}

	{// Read pipes
	Array<u32> users{temp_allocator};
	array::reserve(users, 32);
	for (auto const& entry : used) {
		array::clear(users);
		for (unsigned i = 0; i < fixed_array::size(packed->config.viewports); ++i) {
			if (entry.key == packed->config.viewports[i].pipe) {
				array::push_back(users, i);
			}
		}
		if (!read_pipe(gfx_compiler, packed->config, *entry.value, users)) {
			goto l_failed;
		}
	}}

	// Correct pipe indices
	for (auto& viewport : packed->config.viewports) {
		for (unsigned i = 0; i < fixed_array::size(packed->config.pipes); ++i) {
			if (viewport.pipe == packed->config.pipes[i].name_hash) {
				viewport.pipe = i;
				break;
			}
		}
	}

	{// Serialize generator unit data
	MemoryStream stream{memory::default_allocator(), 16*1024};
	BinaryOutputSerializer ser{stream};
	for (auto const& pipe : packed->config.pipes) {
	for (auto const& layer : pipe.layers) {
	for (auto const& gen_unit : layer.layout) {
		auto* gen_compiler = gfx_compiler::find_generator_compiler(
			gfx_compiler, gen_unit.name_hash
		);
		TOGO_ASSERTE(gen_compiler);
		if (!gen_compiler->func_write(*gen_compiler, ser, packed->config, gen_unit)) {
			goto l_failed;
		}
	}}}
	packed->unit_data = rvalue_ref(stream._data);
	}

	{// Serialize resource
	BinaryOutputSerializer ser{out_stream};
	ser % *packed;
	}
	success = true;

l_failed:
	TOGO_DESTROY(memory::default_allocator(), packed);
	return success;
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
