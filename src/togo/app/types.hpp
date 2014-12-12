#line 2 "togo/app/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Application type.
@ingroup types
@ingroup app
*/

#pragma once

#include <togo/config.hpp>
#include <togo/threading/types.hpp>
#include <togo/resource/types.hpp>
#include <togo/gfx/types.hpp>
#include <togo/input/types.hpp>

namespace togo {

/**
	@addtogroup app
	@{
*/

template<class Data>
struct App;

/// Application model.
template<class Data>
struct AppModel {
	static void init(App<Data>& app);
	static void shutdown(App<Data>& app);
	static void update(App<Data>& app, float dt);
	static void render(App<Data>& app);
};

/// Base application class.
struct AppBase {
	using init_func_type = void (AppBase& app_base);
	using shutdown_func_type = void (AppBase& app_base);
	using update_func_type = void (AppBase& app_base, float dt);
	using render_func_type = void (AppBase& app_base);

	init_func_type& _func_init;
	shutdown_func_type& _func_shutdown;
	update_func_type& _func_update;
	render_func_type& _func_render;

	unsigned _num_args;
	char const* const* _args;

	TaskManager _task_manager;
	ResourceManager _resource_manager;
	gfx::Display* _display;
	InputBuffer _input_buffer;
	gfx::Renderer* _renderer;

	float _update_freq;
	bool _quit;

	AppBase() = delete;
	AppBase(AppBase const&) = delete;
	AppBase(AppBase&&) = delete;
	AppBase& operator=(AppBase const&) = delete;
	AppBase& operator=(AppBase&&) = delete;

	~AppBase();
	AppBase(
		init_func_type& func_init,
		shutdown_func_type& func_shutdown,
		update_func_type& func_update,
		render_func_type& func_render,
		unsigned num_args,
		char const* const args[],
		StringRef const base_path,
		float update_freq
	);
};

/// Core application.
template<class Data>
struct App
	: public AppBase
{
	Data _data;

	App() = delete;
	App(App<Data> const&) = delete;
	App(App<Data>&&) = delete;
	App& operator=(App<Data> const&) = delete;
	App& operator=(App<Data>&&) = delete;

	~App() = default;

	App(
		unsigned num_args,
		char const* const args[],
		StringRef const base_path,
		float update_freq,
		Data&& data
	);
};

/** @} */ // end of doc-group app

} // namespace togo
