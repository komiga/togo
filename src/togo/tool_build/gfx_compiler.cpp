#line 2 "togo/tool_build/gfx_compiler.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/tool_build/gfx_compiler.hpp>
#include <togo/tool_build/generator_compiler.hpp>

namespace togo {
namespace tool_build {

GfxCompiler::GfxCompiler(Allocator& allocator)
	: _gen_compilers(allocator)
{
	hash_map::reserve(_gen_compilers, 16);
}

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

GeneratorCompiler* gfx_compiler::find_generator_compiler(
	GfxCompiler& gfx_compiler,
	gfx::GeneratorNameHash name_hash
) {
	return hash_map::find(gfx_compiler._gen_compilers, name_hash);
}

} // namespace tool_build
} // namespace togo
