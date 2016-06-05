#line 2 "togo/window/opengl.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/window/config.hpp>
#include <togo/window/opengl.hpp>

#if !defined(TOGO_WINDOW_BACKEND_SUPPORTS_OPENGL)
	#error "OpenGL is not supported by the lib/window backend"
#else
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wold-style-cast"
	#include <glad/glad.c>
	#pragma GCC diagnostic pop
#endif
