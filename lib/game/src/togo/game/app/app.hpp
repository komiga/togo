#line 2 "togo/game/app/app.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Application interface.
@ingroup lib_game_app
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/game/entity/types.hpp>
#include <togo/game/world/types.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/app/types.hpp>
#include <togo/game/app/app.gen_interface>

namespace togo {
namespace game {

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
	@addtogroup lib_game_app
	@{
*/

struct Globals {
	Allocator* allocator;
	AppBase* instance;
};
extern app::Globals _globals;

/// Get application instance.
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

/** @} */ // end of doc-group lib_game_app

} // namespace app

} // namespace game
} // namespace togo
