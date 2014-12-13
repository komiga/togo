#line 2 "togo/gfx/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Graphics types.
@ingroup types
@ingroup gfx
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/hash/hash.hpp>
#include <togo/utility/traits.hpp>
#include <togo/utility/utility.hpp>
#include <togo/serialization/types.hpp>

#include <initializer_list>

namespace togo {
namespace gfx {

/**
	@addtogroup gfx_display
	@{
*/

/// Graphics configuration flags.
enum class DisplayConfigFlags : unsigned {
	/// Empty flag.
	none = 0,
	/// Double-buffered display.
	double_buffered = 1 << 0,
};

/// Graphics configuration.
///
/// If either msaa_num_buffers or msaa_num_samples are 0, MSAA is
/// disabled.
struct DisplayConfig {
	struct ColorBits {
		unsigned red;
		unsigned green;
		unsigned blue;
		unsigned alpha;
	};

	/// Color buffer bits.
	gfx::DisplayConfig::ColorBits color_bits;

	/// Depth buffer bits.
	/// If this is 0, the depth buffer should be assumed unavailable.
	unsigned depth_bits;

	/// Stencil buffer bits.
	/// If this is 0, the stencil buffer should be assumed unavailable.
	unsigned stencil_bits;

	/// Number of MSAA buffers.
	unsigned msaa_num_buffers;
	/// Number of MSAA samples.
	unsigned msaa_num_samples;

	/// Miscellaneous flags.
	gfx::DisplayConfigFlags flags;
};

/// Display flags.
enum class DisplayFlags : unsigned {
	/// Empty flag.
	none = 0,
	/// Center window.
	centered = 1 << 0,
	/// Border-less window.
	borderless = 1 << 1,
	/// Full-screen window.
	fullscreen = 1 << 2,
	/// Re-sizable window.
	resizable = 1 << 3,

	owned_by_input_buffer = 1 << 4,
};

/// Display swap modes.
enum class DisplaySwapMode : unsigned {
	/// No screen synchronization.
	immediate = 0,

	/// Wait for at least one refresh cycle before swapping.
	wait_refresh,
};

/// Graphics display.
struct Display;

/** @} */ // end of doc-group gfx_display

/**
	@addtogroup gfx_renderer
	@{
*/

// TODO: Documentation & configuration
#define TOGO_GFX_VERTEXFORMAT_NUM_ATTRIBS 16

#define TOGO_GFX_NODE_NUM_COMMANDS 1024
#define TOGO_GFX_NODE_BUFFER_SIZE 8192

#define TOGO_GFX_CONFIG_NUM_PIPES 8
#define TOGO_GFX_CONFIG_NUM_VIEWPORTS 8
#define TOGO_GFX_PIPE_NUM_LAYERS 32
#define TOGO_GFX_LAYER_NUM_GENERATORS 16

#define TOGO_GFX_NUM_VERTEX_BUFFERS 2048
#define TOGO_GFX_NUM_INDEX_BUFFERS 2048
#define TOGO_GFX_NUM_BUFFER_BINDINGS 3072
#define TOGO_GFX_NUM_TEXTURES 2048
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

enum : u32 {
	/// Null resource ID.
	ID_VALUE_NULL = 0,
};

/** @cond INTERNAL */
struct VertexBuffer;
struct IndexBuffer;
struct BufferBinding;
struct Texture;
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

/// Vertex buffer ID.
using VertexBufferID = gfx::ResourceID<gfx::VertexBuffer>;

/// Index buffer ID.
using IndexBufferID = gfx::ResourceID<gfx::IndexBuffer>;

/// Buffer binding ID.
using BufferBindingID = gfx::ResourceID<gfx::BufferBinding>;

/// Texture ID.
using TextureID = gfx::ResourceID<gfx::Texture>;

/// Uniform ID.
using UniformID = gfx::ResourceID<gfx::Uniform>;

/// Shader ID.
using ShaderID = gfx::ResourceID<gfx::Shader>;

struct GeneratorDef;
struct GeneratorUnit;

/// Render object.
struct RenderObject; // TODO

/// Render node.
struct RenderNode;

/// Renderer.
struct Renderer;

/// Renderer type.
using RendererType = hash32;

/// Renderer types.
enum : RendererType {
	/// OpenGL renderer.
	RENDERER_TYPE_OPENGL = "opengl"_hash32,
};

/// Generator name hash.
using GeneratorNameHash = hash32;

/// Generator name hash literal.
inline constexpr GeneratorNameHash
operator"" _generator_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// Generator names.
enum : GeneratorNameHash {
	/// Null name.
	GEN_NAME_NULL = ""_generator_name,
};

/// GeneratorUnit exec() function.
///
/// objects_begin and objects_end are nullptr for
/// non-Process Generators.
using generator_exec_func_type = void (
	gfx::GeneratorUnit const& unit,
	gfx::RenderNode& node,
	unsigned layer_index,
	unsigned generator_index,
	gfx::RenderObject const* objects_begin,
	gfx::RenderObject const* objects_end
);

/// Configured generator.
struct GeneratorUnit {
	gfx::GeneratorNameHash name_hash;
	void* data;
	gfx::generator_exec_func_type* func_exec;
};

/// GeneratorUnit read() function.
using generator_read_func_type = void (
	gfx::GeneratorDef& def,
	BinaryInputSerializer& ser,
	gfx::GeneratorUnit& unit
);

/// Generator definition.
struct GeneratorDef {
	gfx::GeneratorNameHash name_hash;
	void* data;
	gfx::generator_read_func_type* func_read;
};

/// Index buffer data type.
enum class IndexDataType : unsigned {
	u8, u16, u32,

	NUM
};

/// Vertex attribute data type.
enum class VertexAttribType : unsigned {
	f32, f64,
	u8, u16, u32,

	NUM
};

/// Unpacked vertex attribute.
struct VertexAttrib {
	gfx::VertexAttribType type;
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
			sizeof(f32), sizeof(f64),
			sizeof(u8), sizeof(u16), sizeof(u32)
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
	gfx::IndexBufferID id;
	gfx::IndexDataType type;
};

/// Vertex binding.
struct VertexBinding {
	gfx::VertexBufferID id;
	gfx::VertexFormat const* format;
	u32 offset;
};

/** @} */ // end of doc-group gfx_renderer

} // namespace gfx

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<gfx::DisplayConfigFlags> : true_type {};

template<>
struct enable_enum_bitwise_ops<gfx::DisplayFlags> : true_type {};
/** @endcond */ // INTERNAL

} // namespace togo
