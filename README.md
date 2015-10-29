
## What be it?

**togo** is a collection of libraries with simplicity and rapid iteration at
heart.

## Components

### Library: core

Basic support library. All other libraries depend on it.

Dependencies:

1. [All the Maths‽](https://github.com/komiga/am) (HEAD)

### Library: image

Imaging library.

### Library: window

Windowing library.

Dependencies:

1. [GLEW](http://glew.sourceforge.net/index.html) >= **1.13.0**
2. Backend (one of):
  1. [SDL](http://libsdl.org/download-2.0.php) >= **2.0.3** (*OpenGL, raster*)
  2. [GLFW](http://www.glfw.org/download.html) >= **3.0.4** (*OpenGL*)
3. With raster backends:
  1. `lib/image`

### Library: game

Game engine.

This library takes significant inspiration from bitsquid
(now [Stingray](http://stingrayengine.com/)), who have
[written about](https://github.com/niklasfrykholm/blog) their design in great
detail.

Dependencies:

1. `lib/window` (requires OpenGL backend)

### Tool: res_build

Resource package compiler for the game library. Interface is available as a
static library for extension and automation.

Dependencies:

1. `lib/game`

## Platforms

togo is developed under Linux and currently only supports Linux. Its primary
interface implementations utilize POSIX and the C Standard Library (by way of
the C++ Standard Library, of which only little is used).

## Building

togo libraries are compiled static libraries (by default).

All features support Clang 3.5+ with libc++ (roughly at SVN head) on
Ubuntu 14.04 x86_64. GCC and libstdc++ might possibly work, but they
are untested.

See `dep/README.md` for dependency setup. Libraries, tools, and tests can
then be compiled using plash's standard project protocol:
http://komiga.com/pp-cpp

The only difference from the project protocol is `scripts/premake` in place of
premake, which enforces recipe order to allow the use of GNU make's `--jobs`.

Build configuration:

* `--togo-window-backend=sdl | glfw | raster`

  Set window backend. See [lib/window](#library-window) above for the
  dependencies required.

## License

togo carries the MIT license, which can be found both below and in the
`LICENSE` file.

```
Copyright (c) 2014-2015 plash <plash@komiga.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
