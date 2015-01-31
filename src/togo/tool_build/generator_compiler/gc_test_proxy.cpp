#line 2 "togo/tool_build/generator_compiler/gc_test_proxy.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/gfx/gfx.hpp>
#include <togo/tool_build/generator_compiler.hpp>

namespace togo {
namespace tool_build {

namespace generator_compiler {
namespace gc_test_proxy {

static bool write(
	GeneratorCompiler& /*gen_compiler*/,
	BinaryOutputSerializer& /*ser*/,
	gfx::GeneratorUnit const& /*unit*/
) {
	// Nothing to do!
	return true;
}

} // namespace gc_test_proxy
} // namespace generator_compiler

GeneratorCompiler const generator_compiler::test_proxy{
	gfx::hash_generator_name("test_proxy"),
	generator_compiler::gc_test_proxy::write
};

} // namespace tool_build
} // namespace togo
