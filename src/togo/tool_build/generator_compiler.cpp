#line 2 "togo/tool_build/generator_compiler.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/tool_build/generator_compiler.hpp>
#include <togo/tool_build/gfx_compiler.hpp>

namespace togo {
namespace tool_build {

void generator_compiler::register_standard(
	GfxCompiler& gfx_compiler
) {
	using namespace generator_compiler;
	gfx_compiler::register_generator_compiler(gfx_compiler, test_proxy);
}

} // namespace tool_build
} // namespace togo
