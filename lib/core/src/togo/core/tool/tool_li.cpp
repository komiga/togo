#line 2 "togo/core/tool/tool_li.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/tool/tool.hpp>
#include <togo/core/lua/lua.hpp>

namespace togo {

namespace tool {

static LuaModuleRef const li_module{
	"togo.Tool",
	"togo/core/tool/Tool.lua",
	null_ref_tag{},
	#include <togo/core/tool/Tool.lua>
};

} // namespace tool

/// Register the Lua interface.
void tool::register_lua_interface(lua_State* L) {
	lua::preload_module(L, tool::li_module);
}

} // namespace togo
