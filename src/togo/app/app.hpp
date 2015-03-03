#line 2 "togo/app/app.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Application interface.
@ingroup app
*/

#pragma once

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/memory/types.hpp>
#include <togo/memory/memory.hpp>
#include <togo/entity/types.hpp>
#include <togo/world/types.hpp>
#include <togo/gfx/types.hpp>
#include <togo/app/types.hpp>

namespace togo {

template<class Data>
void AppModel<Data>::destruct(App<Data>& app) {
	app.data.~Data();
}

/// Construct application with arguments and model data.
///
/// args should not contain argv[0].
template<class Data>
inline App<Data>::App(
	ArrayRef<char const* const> args,
	StringRef base_path,
	float update_freq
)
	: AppBase(
		reinterpret_cast<AppBase::destruct_func_type&>(AppModel<Data>::destruct),
		reinterpret_cast<AppBase::init_func_type&>(AppModel<Data>::init),
		reinterpret_cast<AppBase::shutdown_func_type&>(AppModel<Data>::shutdown),
		reinterpret_cast<AppBase::update_func_type&>(AppModel<Data>::update),
		reinterpret_cast<AppBase::render_func_type&>(AppModel<Data>::render),
		args,
		base_path,
		update_freq
	)
	, data()
{}

namespace app {

/**
	@addtogroup app
	@{
*/

struct Globals {
	Allocator* allocator;
	AppBase* instance;
};
extern app::Globals _globals;

/// Get application.
///
/// An assertion will fail if the application has not been created.
inline AppBase& instance() {
	auto* instance = app::_globals.instance;
	TOGO_ASSERTE(instance);
	return *instance;
}

/// Initialize application.
///
/// An assertion will fail if the application has already been created.
template<class Data>
inline AppBase& init(
	Allocator& allocator,
	ArrayRef<char const* const> args,
	StringRef base_path,
	float update_freq
) {
	auto* const app = TOGO_CONSTRUCT(
		allocator, App<Data>,
		args, base_path, update_freq
	);
	extern void init_with(Allocator&, AppBase*);
	init_with(allocator, app);
	return app::instance();
}

/// Shutdown application.
///
/// An assertion will fail if the application has not been created.
void shutdown();

/// Run the application.
///
/// This will run the main loop until a quit is signaled.
void run();

/// Signal the application to quit.
///
/// This has no effect if the application is not running
/// (see app::run()).
void quit();

/// Render a world through a camera and viewport.
void render_world(
	WorldID world_id,
	EntityID camera_id,
	gfx::ViewportNameHash viewport_name_hash
);

/** @} */ // end of doc-group app

} // namespace app

} // namespace togo
