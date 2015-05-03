#line 2 "togo/game/gfx/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Graphics types.
@ingroup lib_game_types
@ingroup lib_game_gfx
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/serialization/types.hpp>
#include <togo/game/resource/types.hpp>

#include <initializer_list>

namespace togo {
namespace game {
namespace gfx {

/**
	@addtogroup lib_game_gfx_renderer
	@{
*/

// TODO: Documentation & configuration
#define TOGO_GFX_VERTEXFORMAT_NUM_ATTRIBS 16
#define TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND 16

#define TOGO_GFX_NODE_NUM_COMMANDS 1024
#define TOGO_GFX_NODE_BUFFER_SIZE 8192

#define TOGO_GFX_KEY_SEQ_BITS 9
#define TOGO_GFX_KEY_SEQ_MAX (1ul << TOGO_GFX_KEY_SEQ_BITS)
#define TOGO_GFX_KEY_SEQ_MASK (TOGO_GFX_KEY_SEQ_MAX - 1)

#define TOGO_GFX_KEY_USER_BITS (64ul - TOGO_GFX_KEY_SEQ_BITS)
#define TOGO_GFX_KEY_USER_MASK ((1ul << TOGO_GFX_KEY_USER_BITS) - 1)

#define TOGO_GFX_CONFIG_NUM_RESOURCES 32
#define TOGO_GFX_CONFIG_NUM_PIPES 8
#define TOGO_GFX_CONFIG_NUM_VIEWPORTS 8
#define TOGO_GFX_PIPE_NUM_LAYERS 32
#define TOGO_GFX_LAYER_NUM_GENERATORS 16

#define TOGO_GFX_NUM_BUFFERS 2048
#define TOGO_GFX_NUM_BUFFER_BINDINGS 2048
#define TOGO_GFX_NUM_TEXTURES 2048
#define TOGO_GFX_NUM_RENDER_TARGETS 64
#define TOGO_GFX_NUM_FRAMEBUFFERS 64
#define TOGO_GFX_NUM_UNIFORMS 64
#define TOGO_GFX_NUM_SHADERS 128
#define TOGO_GFX_NUM_NODES 4

/// Buffer data binding mode.
enum class BufferDataBinding : unsigned {
	/// Buffer data never changes.
	fixed,
	/// Buffer data changes frequently.
	dynamic,

	NUM
};

/// Render target formats.
enum class RenderTargetFormat : u32 {
	/// RGB with 8-bit components (24 bits).
	rgb8,
	/// RGBA with 8-bit components (32 bits).
	rgba8,
	/// 16-bit depth.
	d16,
	/// 32-bit depth.
	d32,
	/// Combined 24-bit depth and 8-bit stencil.
	d24s8,

	NUM
};

enum : u32 {
	/// Null resource ID.
	ID_VALUE_NULL = 0,
};

/** @cond INTERNAL */
struct Buffer;
struct BufferBinding;
struct Texture;
struct RenderTarget;
struct Framebuffer;
struct Uniform;
struct Shader;

template<class /*R*/>
struct ResourceID {
	u32 _value;

	bool valid() const {
		return _value != gfx::ID_VALUE_NULL;
	}
};
/** @endcond */ // INTERNAL

/// Buffer ID.
using BufferID = gfx::ResourceID<gfx::Buffer>;

/// Buffer binding ID.
using BufferBindingID = gfx::ResourceID<gfx::BufferBinding>;

/// Texture ID.
using TextureID = gfx::ResourceID<gfx::Texture>;

/// Render target ID.
using RenderTargetID = gfx::ResourceID<gfx::RenderTarget>;

/// Render target binding ID.
using FramebufferID = gfx::ResourceID<gfx::Framebuffer>;

/// Uniform ID.
using UniformID = gfx::ResourceID<gfx::Uniform>;

/// Shader ID.
using ShaderID = gfx::ResourceID<gfx::Shader>;

struct GeneratorDef;
struct GeneratorUnit;

struct RenderConfig;
struct RenderObject;
struct RenderNode;
struct Camera;

/// Renderer.
struct Renderer;

/// Renderer type.
using RendererType = hash32;

/// Parameter block name hash.
using ParamBlockNameHash = hash32;

/// Generator name hash.
using GeneratorNameHash = hash32;

/// Viewport name hash.
using ViewportNameHash = hash32;

namespace hash_literals {

/// Parameter block name hash literal.
inline constexpr ParamBlockNameHash
operator"" _param_block_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// Generator name hash literal.
inline constexpr GeneratorNameHash
operator"" _generator_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// Viewport name hash literal.
inline constexpr ViewportNameHash
operator"" _viewport_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

} // namespace hash_literals

using namespace hash_literals;

/// Renderer types.
enum : RendererType {
	/// OpenGL renderer.
	RENDERER_TYPE_OPENGL = "opengl"_hash32,
};

/// Parameter block names.
enum : ParamBlockNameHash {
	/// Null name.
	PB_NAME_NULL = ""_param_block_name,
};

/// Generator names.
enum : GeneratorNameHash {
	/// Null name.
	GEN_NAME_NULL = ""_generator_name,
};

/// Viewport names.
enum : ViewportNameHash {
	/// Null name.
	VIEWPORT_NAME_NULL = ""_viewport_name,
};

/// Configured generator.
struct GeneratorUnit {
	/// GeneratorUnit exec() function.
	using exec_func_type = void (
		gfx::GeneratorUnit const& unit,
		gfx::RenderNode& node,
		gfx::RenderObject const* objects_begin,
		gfx::RenderObject const* objects_end
	);

	/// Name hash.
	gfx::GeneratorNameHash name_hash;

	/// Data index.
	u32 data_index;

	/// Data.
	void* data;

	/// Execute the generator.
	///
	/// objects_begin and objects_end are nullptr for
	/// non-Process Generators.
	exec_func_type* func_exec;
};

/// Generator definition.
struct GeneratorDef {
	/// GeneratorDef init() function.
	using init_func_type = void (
		gfx::GeneratorDef& def,
		gfx::Renderer* renderer
	);

	/// GeneratorDef destroy() function.
	using destroy_func_type = void (
		gfx::GeneratorDef const& def,
		gfx::Renderer* renderer
	);

	/// GeneratorUnit read() function.
	using read_unit_func_type = void (
		gfx::GeneratorDef const& def,
		gfx::Renderer* renderer,
		BinaryInputSerializer& ser,
		gfx::GeneratorUnit& unit
	);

	/// Name hash.
	gfx::GeneratorNameHash name_hash;

	/// Data.
	void* data;

	/// Initialize the definition data.
	init_func_type* func_init;
	/// Destroy the definition data.
	destroy_func_type* func_destroy;
	/// Read a generator unit.
	read_unit_func_type* func_read_unit;
	/// Default execution function for units.
	///
	/// If this is assigned, generator units' func_exec will be assigned to it
	/// before func_read_unit calls are made.
	gfx::GeneratorUnit::exec_func_type* func_exec_unit;
};

/// Primitive data type.
enum class Primitive : unsigned {
	u8, u16, u32,
	f32, f64,

	NUM
};

/// Integral primitive data type.
enum class IntegerPrimitive : unsigned {
	u8, u16, u32,

	NUM
};

/// Unpacked vertex attribute.
struct VertexAttrib {
	gfx::Primitive type;
	unsigned num_components;
	bool normalize_fixed;
};

// TODO: Encode attribute data to an integer to save memory
// TODO: Utilize C++14 constexpr features for compile-time construction
/// Vertex format.
struct VertexFormat {
	u32_fast _num_attribs;
	u32_fast _stride;
	gfx::VertexAttrib _attribs[TOGO_GFX_VERTEXFORMAT_NUM_ATTRIBS];
	u8 _offsets[TOGO_GFX_VERTEXFORMAT_NUM_ATTRIBS];

	inline /*constexpr*/ VertexFormat(
		std::initializer_list<gfx::VertexAttrib> const ilist
	) noexcept
		: _num_attribs(0)
		, _stride(0)
		, _attribs()
		, _offsets()
	{
		static constexpr unsigned const
		attrib_type_size[]{
			sizeof(u8), sizeof(u16), sizeof(u32),
			sizeof(f32), sizeof(f64),
		};
		for (auto const& attrib : ilist) {
			_attribs[_num_attribs] = attrib;
			_offsets[_num_attribs] = _stride;
			_stride += attrib_type_size[unsigned_cast(attrib.type)] * attrib.num_components;
			++_num_attribs;
		}
	}
};

/// Index binding.
struct IndexBinding {
	gfx::BufferID id;
	gfx::IntegerPrimitive type;
};

/// Vertex binding.
struct VertexBinding {
	gfx::BufferID id;
	gfx::VertexFormat const* format;
	u32 offset;
};

/// Parameter block binding.
struct ParamBlockBinding {
	gfx::BufferID id;
	u32 offset;
	u32 size;
};

/// Parameter block definition.
struct ParamBlockDef {
	u32 index;
	ParamBlockNameHash name_hash;
	StringRef name;
};

/// RenderTarget specification (creation).
struct RenderTargetSpec {
	/// Properties.
	enum : u32 {
		MASK_FLAG = 0xFF,
		F_SCALE = 1 << 0,
		F_CLEAR = 1 << 1,
		F_DOUBLE_BUFFERED = 1 << 2,
	};

	u32 properties;
	gfx::RenderTargetFormat format;
	f32 dim_x;
	f32 dim_y;
};

/// Shader stage.
struct ShaderStage {
	enum class Type : u32 {
		vertex = 0,
		fragment,

		NUM
	};

	Type type;
	FixedArray<StringRef, 24> sources;
};

/// Shader specification (creation).
struct ShaderSpec {
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> fixed_param_blocks;
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> draw_param_blocks;
	FixedArray<gfx::ShaderStage, unsigned_cast(gfx::ShaderStage::Type::NUM)> stages;
};

/// Shader definition (resource).
struct ShaderDef {
	/// Properties.
	enum : u32 {
		TYPE_MASK = 0x0F,
		TYPE_PRELUDE = 1 << 0,
		TYPE_UNIT = 1 << 1,

		LANG_MASK = 0xF0,
		LANG_GLSL = 1 << 4,

		// stage sources indices + param block names + size of data
		NUM_INDICES
			= unsigned_cast(gfx::ShaderStage::Type::NUM)
			+ TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND // fixed
			+ TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND // draw
			+ 1
		,
		IDX_PARAM_NAMES = unsigned_cast(gfx::ShaderStage::Type::NUM),
	};

	u32 properties;
	// String blob containing all sources in sequence and all param
	// block names. Shared source starts at 0. fragment and vertex source are
	// indexed by members.
	Array<char> data;
	FixedArray<ResourceNameHash, 8> prelude;
	FixedArray<u32, NUM_INDICES> data_offsets;
	// gfx::ParamBlockDef::name is patched to .data during serialization
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> fixed_param_blocks;
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> draw_param_blocks;

	ShaderDef() = delete;
	ShaderDef(ShaderDef const&) = delete;
	ShaderDef& operator=(ShaderDef const&) = delete;

	~ShaderDef() = default;
	ShaderDef(ShaderDef&&) = default;
	ShaderDef& operator=(ShaderDef&&) = default;

	ShaderDef(
		Allocator& allocator
	)
		: properties(0)
		, data(allocator)
		, prelude()
		, data_offsets()
		, fixed_param_blocks()
		, draw_param_blocks()
	{}
};

/// Render config resource.
struct RenderConfigResource {
	/// Properties.
	enum : u32 {
		MASK_TYPE = 0x0F,
		TYPE_RENDER_TARGET = 1 << 0,
	};

	hash32 name_hash;
	u32 properties;

	union {
		gfx::RenderTargetSpec render_target;
	} data;

	FixedArray<char, 32> name;

	/// Data type.
	u32 type() const {
		return properties & MASK_TYPE;
	}
};

/// Pipe layer.
struct Layer {
	enum class Order : u32 {
		back_front,
		front_back,
	};

	hash32 name_hash;
	FixedArray<hash32, 4> rts;
	hash32 dst;
	Order order;
	u32 seq_base;
	FixedArray<gfx::GeneratorUnit, TOGO_GFX_LAYER_NUM_GENERATORS> layout;
	FixedArray<char, 32> name;
};

/// Render pipe.
struct Pipe {
	hash32 name_hash;
	FixedArray<gfx::Layer, TOGO_GFX_PIPE_NUM_LAYERS> layers;
	FixedArray<char, 32> name;
};

/// Render viewport.
struct Viewport {
	// TODO: resources
	gfx::ViewportNameHash name_hash;
	u32 pipe;
	hash32 output_rt;
	hash32 output_dst;
	FixedArray<char, 32> name;
};

/// Render config.
struct RenderConfig {
	FixedArray<gfx::RenderConfigResource, TOGO_GFX_CONFIG_NUM_RESOURCES> shared_resources;
	FixedArray<gfx::Viewport, TOGO_GFX_CONFIG_NUM_VIEWPORTS> viewports;
	FixedArray<gfx::Pipe, TOGO_GFX_CONFIG_NUM_PIPES> pipes;
};

/// Render config with generator unit data.
struct PackedRenderConfig {
	gfx::RenderConfig config;
	Array<u8> unit_data;

	PackedRenderConfig(
		Allocator& allocator
	)
		: unit_data(allocator)
	{}
};

/// Render object.
struct RenderObject {
	Mat4x4 transform;
	gfx::BufferBindingID binding;
};

/// Render command key.
struct CmdKey {
	u64 key;
	void* data;
};

/// Render node.
struct RenderNode {
	u64 sequence;
	unsigned num_commands;
	unsigned buffer_size;
	gfx::CmdKey keys[TOGO_GFX_NODE_NUM_COMMANDS];
	u8 buffer[TOGO_GFX_NODE_BUFFER_SIZE];
};

// TODO: Move to component
/// Camera.
struct Camera {};

/** @} */ // end of doc-group lib_game_gfx_renderer

} // namespace gfx
} // namespace game
} // namespace togo
