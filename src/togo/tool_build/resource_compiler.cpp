#line 2 "togo/tool_build/resource_compiler.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/tool_build/resource_compiler.hpp>
#include <togo/tool_build/compiler_manager.hpp>

namespace togo {
namespace tool_build {

void resource_compiler::register_standard(
	CompilerManager& cm,
	GfxCompiler& gfx_compiler
) {
	resource_compiler::register_test(cm);
	resource_compiler::register_shader_prelude(cm);
	resource_compiler::register_shader(cm);
	resource_compiler::register_render_config(cm, gfx_compiler);
}

} // namespace tool_build
} // namespace togo
