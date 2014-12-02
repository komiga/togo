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

void renderer::register_generator_def(
	gfx::Renderer* renderer,
	gfx::GeneratorDef const& def
) {
	TOGO_ASSERT(
		def.func_read,
		"func_read must be assigned in generator definition"
	);
	TOGO_ASSERTF(
		!hash_map::has(renderer->_generators, def.name_hash),
		"generator %08x has already been registered",
		def.name_hash
	);
	hash_map::push(renderer->_generators, def.name_hash, def);
}

gfx::GeneratorDef const* renderer::get_generator_def(
	gfx::Renderer const* renderer,
	gfx::GeneratorNameHash name_hash
) {
	return hash_map::get(renderer->_generators, name_hash);
}

} // namespace gfx
} // namespace togo
