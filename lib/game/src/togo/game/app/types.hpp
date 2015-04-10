#line 2 "togo/game/app/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Application type.
@ingroup lib_game_types
@ingroup lib_game_app
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/game/entity/types.hpp>
#include <togo/game/world/types.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/input/types.hpp>
#include <togo/game/resource/types.hpp>

namespace togo {
namespace game {

/**
	@addtogroup lib_game_app
	@{
*/

template<class Data>
struct App;

/// Application model.
template<class Data>
struct AppModel {
	static void destruct(App<Data>& app);

	static void init(App<Data>& app);
	static void shutdown(App<Data>& app);
	static void update(App<Data>& app, float dt);
	static void render(App<Data>& app);
};

/// Base application class.
struct AppBase {
	using destruct_func_type = void (AppBase& app_base);
	using init_func_type = void (AppBase& app_base);
	using shutdown_func_type = void (AppBase& app_base);
	using update_func_type = void (AppBase& app_base, float dt);
	using render_func_type = void (AppBase& app_base);

	destruct_func_type& _func_destruct;
	init_func_type& _func_init;
	shutdown_func_type& _func_shutdown;
	update_func_type& _func_update;
	render_func_type& _func_render;

	ArrayRef<char const* const> args;

	TaskManager task_manager;
	ResourceManager resource_manager;
	EntityManager entity_manager;
	WorldManager world_manager;

	gfx::Display* display;
	InputBuffer input_buffer;
	gfx::Renderer* renderer;

	float _update_freq;
	bool _quit;

	AppBase() = delete;
	AppBase(AppBase const&) = delete;
	AppBase(AppBase&&) = delete;
	AppBase& operator=(AppBase const&) = delete;
	AppBase& operator=(AppBase&&) = delete;

	~AppBase();
	AppBase(
		destruct_func_type& func_destruct,
		init_func_type& func_init,
		shutdown_func_type& func_shutdown,
		update_func_type& func_update,
		render_func_type& func_render,
		ArrayRef<char const* const> args,
		StringRef base_path,
		float update_freq
	);
};

/// Core application.
template<class Data>
struct App
	: public AppBase
{
	Data data;

	App() = delete;
	App(App<Data> const&) = delete;
	App(App<Data>&&) = delete;
	App& operator=(App<Data> const&) = delete;
	App& operator=(App<Data>&&) = delete;

	~App() = default;

	App(
		ArrayRef<char const* const> args,
		StringRef base_path,
		float update_freq
	);
};

/** @} */ // end of doc-group lib_game_app

} // namespace game
} // namespace togo
