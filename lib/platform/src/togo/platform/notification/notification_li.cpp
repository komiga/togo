#line 2 "togo/platform/notification/notification_li.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/platform/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/lua/lua.hpp>
#include <togo/platform/notification/notification.hpp>

namespace togo {

namespace notification {

TOGO_LI_FUNC_DEF(_mm_ctor) {
	(void)lua::new_userdata<Notification>(L,
		lua::get_string(L, 1),
		lua::get_string(L, 2),
		luaL_opt(L, lua::get_integer, 3, -1),
		static_cast<Type>(luaL_opt(L, lua::get_integer, 4, 0)),
		static_cast<Priority>(luaL_opt(L, lua::get_integer, 5, 0))
	);
	return 1;
}

TOGO_LI_FUNC_DEF(_mm_dtor) {
	auto& notification = *lua::get_userdata<Notification>(L, 1);
	notification.~Notification();
	return 0;
}

TOGO_LI_FUNC_DEF(__module_init__) {
	lua::register_userdata<Notification>(L, notification::li__mm_dtor);

	lua_createtable(L, 0, 6);
	lua::table_set_copy_raw(L, 1, "Type", -1);
	lua::table_set_raw(L, "generic" , unsigned_cast(Type::generic));
	lua::table_set_raw(L, "error" , unsigned_cast(Type::error));
	lua::table_set_raw(L, "persisting_process" , unsigned_cast(Type::persisting_process));
	lua::table_set_raw(L, "alarm" , unsigned_cast(Type::alarm));
	lua::table_set_raw(L, "message" , unsigned_cast(Type::message));
	lua::table_set_raw(L, "media" , unsigned_cast(Type::media));
	lua_pop(L, 1);

	lua_createtable(L, 0, 3);
	lua::table_set_copy_raw(L, 1, "Priority", -1);
	lua::table_set_raw(L, "normal" , unsigned_cast(Priority::normal));
	lua::table_set_raw(L, "low", unsigned_cast(Priority::low));
	lua::table_set_raw(L, "high" , unsigned_cast(Priority::high));
	lua_pop(L, 1);

	return 0;
}

TOGO_LI_FUNC_DEF(set_title) {
	auto& notification = *lua::get_userdata<Notification>(L, 1);
	auto title = lua::get_string(L, 1);
	notification::set_title(notification, title);
	return 0;
}

TOGO_LI_FUNC_DEF(set_body) {
	auto& notification = *lua::get_userdata<Notification>(L, 1);
	auto body = lua::get_string(L, 1);
	notification::set_body(notification, body);
	return 0;
}

TOGO_LI_FUNC_DEF(post) {
	auto& notification = *lua::get_userdata<Notification>(L, 1);
	lua::push_value(L, notification::post(notification));
	return 1;
}

TOGO_LI_FUNC_DEF(dismiss) {
	auto& notification = *lua::get_userdata<Notification>(L, 1);
	notification::dismiss(notification);
	return 0;
}

} // namespace notification

namespace {

static LuaModuleFunctionArray const li_funcs{
	TOGO_LI_FUNC_REF(notification, __module_init__)
	TOGO_LI_FUNC_REF(notification, _mm_ctor)
	TOGO_LI_FUNC_REF(notification, _mm_dtor)

	TOGO_LI_FUNC_REF(notification, set_title)
	TOGO_LI_FUNC_REF(notification, set_body)
	TOGO_LI_FUNC_REF(notification, post)
	TOGO_LI_FUNC_REF(notification, dismiss)
};

static LuaModuleRef const li_module{
	"togo.notification",
	"togo/platform/notification/notification.lua",
	li_funcs,
	#include <togo/platform/notification/notification.lua>
};

} // anonymous namespace

/// Register the Lua interface.
void notification::register_lua_interface(lua_State* L) {
	lua::preload_module(L, li_module);
}

} // namespace togo
