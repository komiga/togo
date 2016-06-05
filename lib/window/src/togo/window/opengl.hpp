#line 2 "togo/window/opengl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief OpenGL includes.
*/

#pragma once

#include <togo/window/config.hpp>

#if !defined(TOGO_WINDOW_BACKEND_SUPPORTS_OPENGL)
	#error "OpenGL is not supported by the lib/window backend"
#else
	#include <glad/glad.h>
#endif
