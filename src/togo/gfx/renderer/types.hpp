#line 2 "togo/gfx/renderer/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/hash/hash.hpp>
#include <togo/memory/types.hpp>
#include <togo/gfx/types.hpp>
#include <togo/collection/types.hpp>

#if (TOGO_CONFIG_RENDERER == TOGO_RENDERER_OPENGL)
	#include <togo/gfx/renderer/opengl.hpp>
#endif

namespace togo {
namespace gfx {

template<class R>
union ResourceSlot;

template<class R>
struct ResourceHole {
	gfx::ResourceID<R> id;
	gfx::ResourceSlot<R>* next;
};

template<class R>
union ResourceSlot {
	gfx::ResourceID<R> id;
	R res;
	gfx::ResourceHole<R> hole;
};

namespace {
	constexpr u32 npot_s(u32 x, unsigned b) {
		return x | (x >> b);
	}

	constexpr u32 npot(u32 x) {
		// NB: Does not kindly handle x = 0
		/*
			--x;
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;
			++x;
		*/
		return 1 + npot_s(npot_s(npot_s(npot_s(npot_s(x - 1, 1), 2), 4), 8), 16);
	}
} // anonymous namespace

// N > 0 assumed
template<class R, unsigned N>
struct ResourceArray {
	enum : unsigned {
		NUM_SLOTS = N,
		ID_ADD = npot(NUM_SLOTS),
		INDEX_MASK = ID_ADD - 1,
	};

	u32_fast _num;
	u32 _id_gen;
	gfx::ResourceSlot<R>* _first_hole;
	gfx::ResourceSlot<R> _slots[NUM_SLOTS];

	ResourceArray()
		: _num(0)
		, _id_gen(ID_ADD)
		, _first_hole(_slots)
		, _slots()
	{
		auto* const last = _slots + (NUM_SLOTS - 1);
		for (auto* slot = _slots; slot != last; ++slot) {
			slot->hole.next = slot + 1;
		}
		last->hole.next = nullptr;
	}
};

struct Layer {
	enum class Order : u32 {
		back_front,
		front_back,
	};

	hash32 name_hash;
	FixedArray<hash32, 4> rts;
	hash32 dst;
	Order order;
	FixedArray<gfx::GeneratorUnit, TOGO_GFX_LAYER_NUM_GENERATORS> layout;
	FixedArray<char, 32> name;
};

struct Pipe {
	hash32 name_hash;
	FixedArray<gfx::Layer, TOGO_GFX_PIPE_NUM_LAYERS> layers;
	FixedArray<char, 32> name;
};

struct Viewport {
	// TODO: resources
	hash32 name_hash;
	u32 pipe;
	hash32 output_rt;
	hash32 output_dst;
	FixedArray<char, 32> name;
};

struct RenderConfig {
	// TODO: shared_resources
	FixedArray<gfx::Viewport, TOGO_GFX_CONFIG_NUM_VIEWPORTS> viewports;
	FixedArray<gfx::Pipe, TOGO_GFX_CONFIG_NUM_PIPES> pipes;
};

struct RenderCmdKey {
	u64 key;
	void* data;
};

struct RenderNode {
	gfx::RenderCmdKey keys[TOGO_GFX_NODE_NUM_COMMANDS];
	u8 buffer[TOGO_GFX_NODE_BUFFER_SIZE];
};

struct Renderer {
	Allocator* _allocator;

	gfx::RendererImpl _impl;

	gfx::ShaderDef _shader_stage;
	HashMap<gfx::GeneratorNameHash, gfx::GeneratorDef> _generators;
	gfx::RenderConfig _config;

	gfx::ResourceArray<gfx::Buffer, TOGO_GFX_NUM_VERTEX_BUFFERS> _buffers;
	gfx::ResourceArray<gfx::BufferBinding, TOGO_GFX_NUM_BUFFER_BINDINGS> _buffer_bindings;
	gfx::ResourceArray<gfx::Texture, TOGO_GFX_NUM_TEXTURES> _textures;
	gfx::ResourceArray<gfx::Uniform, TOGO_GFX_NUM_UNIFORMS> _uniforms;
	gfx::ResourceArray<gfx::Shader, TOGO_GFX_NUM_SHADERS> _shaders;

	gfx::RenderNode _nodes[TOGO_GFX_NUM_NODES];

	Renderer() = delete;
	Renderer(Renderer const&) = delete;
	Renderer& operator=(Renderer const&) = delete;

	~Renderer() = default;
	Renderer(Renderer&&) = default;
	Renderer& operator=(Renderer&&) = default;

	Renderer(
		Allocator& allocator,
		gfx::RendererImpl&& impl
	);
};

} // namespace gfx
} // namespace togo
