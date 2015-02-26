#line 2 "togo/gfx/renderer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/renderer/private.hpp>
#include <togo/gfx/renderer/private.ipp>

#if (TOGO_CONFIG_RENDERER == TOGO_RENDERER_OPENGL)
	#include <togo/gfx/renderer/opengl.ipp>
#endif

namespace togo {
namespace gfx {

Renderer::Renderer(
	Allocator& allocator,
	gfx::RendererImpl&& impl
)
	: _allocator(&allocator)
	, _impl(impl)
	, _shader_stage(allocator)
	, _num_active_draw_param_blocks(0)
	, _fixed_param_blocks()
	, _generators(allocator)
	, _config()
	, _buffers()
	, _buffer_bindings()
	, _textures()
	, _uniforms()
	, _shaders()
	, _nodes()
{}

void renderer::register_generator_def(
	gfx::Renderer* const renderer,
	gfx::GeneratorDef const& def
) {
	TOGO_ASSERT(
		def.func_read_unit,
		"func_read must be assigned in generator definition"
	);
	TOGO_ASSERTF(
		!hash_map::has(renderer->_generators, def.name_hash),
		"generator %08x has already been registered",
		def.name_hash
	);
	hash_map::push(renderer->_generators, def.name_hash, def);
}

gfx::GeneratorDef const* renderer::find_generator_def(
	gfx::Renderer const* const renderer,
	gfx::GeneratorNameHash const name_hash
) {
	return hash_map::find(renderer->_generators, name_hash);
}

} // namespace gfx
} // namespace togo
