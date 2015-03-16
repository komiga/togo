#line 2 "togo/gfx/renderer/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/memory/types.hpp>
#include <togo/collection/types.hpp>
#include <togo/hash/hash.hpp>
#include <togo/threading/types.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/command.hpp>

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

struct Renderer {
	Allocator* _allocator;

	gfx::RendererImpl _impl;

	gfx::ShaderDef _shader_stage;
	unsigned _num_active_draw_param_blocks;
	gfx::ParamBlockNameHash _fixed_param_blocks[TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND];
	HashMap<gfx::GeneratorNameHash, gfx::GeneratorDef> _generators;
	Mutex _frame_mutex;
	CondVar _frame_condvar;
	struct WorkData {
		bool active;
		gfx::Display* display;
		unsigned num_commands;
		unsigned buffer_size;
		u8 buffer[48 * sizeof(gfx::CmdRenderWorld)];
	} _work_data;

	gfx::RenderConfig _config;

	gfx::ResourceArray<gfx::Buffer, TOGO_GFX_NUM_BUFFERS> _buffers;
	gfx::ResourceArray<gfx::BufferBinding, TOGO_GFX_NUM_BUFFER_BINDINGS> _buffer_bindings;
	gfx::ResourceArray<gfx::Texture, TOGO_GFX_NUM_TEXTURES> _textures;
	gfx::ResourceArray<gfx::Uniform, TOGO_GFX_NUM_UNIFORMS> _uniforms;
	gfx::ResourceArray<gfx::Shader, TOGO_GFX_NUM_SHADERS> _shaders;

	gfx::RenderNode _nodes[TOGO_GFX_NUM_NODES];
	gfx::CmdKey _joined_keys_a[TOGO_GFX_NUM_NODES * TOGO_GFX_NODE_NUM_COMMANDS];
	gfx::CmdKey _joined_keys_b[TOGO_GFX_NUM_NODES * TOGO_GFX_NODE_NUM_COMMANDS];

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
