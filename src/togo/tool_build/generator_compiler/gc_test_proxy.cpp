#line 2 "togo/tool_build/generator_compiler/gc_test_proxy.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/tool_build/generator_compiler.hpp>
#include <togo/gfx/types.hpp>

namespace togo {
namespace tool_build {
namespace generator_compiler {

static bool test_proxy_write(
	GeneratorCompiler& /*gen_compiler*/,
	BinaryOutputSerializer& /*ser*/,
	gfx::GeneratorUnit const& /*unit*/
) {
	// Nothing to do!
	return true;
}

using namespace gfx::hash_literals;

GeneratorCompiler const test_proxy{
	"test_proxy"_generator_name,
	test_proxy_write
};

} // namespace generator_compiler
} // namespace tool_build
} // namespace togo
