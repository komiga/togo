
## togo - a data-oriented game engine

**togo** is a game engine with data-oriented design (DOD) at its heart.

It takes significant inspiration from [bitsquid](http://bitsquid.se), who
have [written about](http://bitsquid.blogspot.com) their design in great
detail.

## Core design

togo is optimization-routed to an extent that is possibly harmful.
Interfaces are kept separate from types to reduce compilation time, and most
data structures are “open” in that they can be modified by anything. It aims
to make friends with the cache through careful data design & utilization.

togo minimally uses the C++ stdlib. It supplies its own collections and
allocators (in which the stdlib is… lacking) and most other constructs.
If we can write it, we probably do, primarily because the implementation will
then be license-free, consistent, easier to debug, and aligned with togo's
design.

Core systems are decoupled for locality, flexibility in storage, and
encapsulation. Systems are easier to understand and *much* easier to maintain
when they are decoupled — i.e., when they have few explicit dependencies on
other systems. To this end, systems expose data through event streams or
polling to higher-level systems, which connect the low-level systems in a more
intelligent and flexible manner than coupled systems could.

togo is in infancy and is not expecting to be The One True Engine.

## Platforms

togo is developed under Linux and currently only supports Linux. Its primary
implementations utilize POSIX and the C standard library (by way of the C++
standard library).

## Dependencies

togo uses:

1. [All the Maths‽](https://github.com/komiga/am) (HEAD)
2. [GLEW](http://glew.sourceforge.net/index.html) **1.11.0**
3. Graphics backend (one of):
  1. [SDL](http://libsdl.org/download-2.0.php) **2.0.3**
  2. [GLFW](http://www.glfw.org/download.html) **3.0.4**

See `dep/README.md` for dependency setup.

## Building

togo is a compiled static library (by default).

All features support Clang 3.5+ with libc++ (roughly at SVN head) on
Ubuntu 14.04 x86_64. GCC and libstdc++ might possibly work, but they
are untested.

Once dependencies are setup (see `dep/README.md`), the library and tests can be
compiled using plash's standard project protocol: http://komiga.com/pp-cpp

## License

togo carries the MIT license, which can be found both below and in the
`LICENSE` file.

```
Copyright (c) 2014 Timothy Howard

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
