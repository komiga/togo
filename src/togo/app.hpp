#line 2 "togo/app.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file app.hpp
@brief Application interface.
@ingroup app
*/

#pragma once

#include <togo/config.hpp>
#include <togo/app_type.hpp>

namespace togo {

/// Construct application with arguments and model data.
///
/// args should not contain argv[0].
template<class Data>
inline App<Data>::App(
	unsigned num_args,
	char const* args[],
	float update_freq,
	Data&& data
)
	: AppBase(
		reinterpret_cast<AppBase::init_func_type>(AppModel<Data>::init),
		reinterpret_cast<AppBase::shutdown_func_type>(AppModel<Data>::shutdown),
		reinterpret_cast<AppBase::update_func_type>(AppModel<Data>::update),
		reinterpret_cast<AppBase::render_func_type>(AppModel<Data>::render),
		num_args,
		args,
		update_freq
	)
	, _data(data)
{}

namespace app {

/**
	@addtogroup app
	@{
*/

/// Run the application.
///
/// This will initialize the application and run the main loop until
/// a quit is signaled.
template<class Data>
inline void run(App<Data>& app) {
	extern void core_run(AppBase&);
	core_run(app);
}

/// Signal the application to quit.
///
/// This has no effect if the application is not running
/// (see app::run()).
template<class Data>
inline void quit(App<Data>& app) {
	extern void core_quit(AppBase&);
	core_quit(app);
}

/** @} */ // end of doc-group app

} // namespace app

} // namespace togo
