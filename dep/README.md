
## Dependencies!

Symlink/clone/download the following libraries (take care they go into the
proper directories).

### lib/core

* [All the Maths‽](https://github.com/komiga/am) (HEAD) into `am/`:

  `git clone git://github.com/komiga/am.git am/`

### lib/window

* Backend (one of):

  1. OpenGL:
    1. [SDL](http://libsdl.org/download-2.0.php) >= **2.0.3** into `sdl/`
    2. [GLFW](http://www.glfw.org/download.html) >= **3.0.4** into `glfw/`
  2. Raster:
    1. [XCB (Linux)](http://xcb.freedesktop.org) >= **1.11** into `xcb/`

### lib/game

* [GLEW](http://glew.sourceforge.net/index.html) >= **1.11.0** into `glew/`
