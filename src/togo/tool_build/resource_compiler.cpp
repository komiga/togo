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
	CompilerManager& cm
) {
	resource_compiler::register_test(cm);
	resource_compiler::register_shader_prelude(cm);
	resource_compiler::register_shader(cm);
}

} // namespace tool_build
} // namespace togo
