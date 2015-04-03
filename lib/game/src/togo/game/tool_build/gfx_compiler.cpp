#line 2 "togo/game/tool_build/gfx_compiler.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/tool_build/config.hpp>
#include <togo/game/tool_build/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/collection/hash_map.hpp>
#include <togo/game/tool_build/generator_compiler.hpp>
#include <togo/game/tool_build/gfx_compiler.hpp>

namespace togo {
namespace tool_build {

GfxCompiler::GfxCompiler(Allocator& allocator)
	: _gen_compilers(allocator)
{
	hash_map::reserve(_gen_compilers, 16);
}

/// Register a generator compiler.
void gfx_compiler::register_generator_compiler(
	GfxCompiler& gfx_compiler,
	GeneratorCompiler const& gen_compiler
) {
	TOGO_ASSERT(
		gen_compiler.func_write,
		"func_write must be assigned in generator compiler"
	);
	TOGO_ASSERTF(
		!hash_map::has(gfx_compiler._gen_compilers, gen_compiler.name_hash),
		"generator %08x has already been registered",
		gen_compiler.name_hash
	);
	hash_map::push(gfx_compiler._gen_compilers, gen_compiler.name_hash, gen_compiler);
}

/// Find a generator compiler by name.
GeneratorCompiler* gfx_compiler::find_generator_compiler(
	GfxCompiler& gfx_compiler,
	gfx::GeneratorNameHash name_hash
) {
	return hash_map::find(gfx_compiler._gen_compilers, name_hash);
}

} // namespace tool_build
} // namespace togo
