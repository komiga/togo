
## Dependencies!

Symlink/clone/download the following libraries (take care they go into the
proper directories).

### lib/core

1. [Lua](https://www.lua.org/download.html) == **5.3.5** into `lua/`

### lib/image

`lib/image` has no manual dependencies.

### lib/platform

`lib/platform` has no manual dependencies.

### lib/window

1. [glad](https://github.com/Dav1dde/glad) (HEAD):

  ```
  git clone git@github.com:Dav1dde/glad.git glad/
  ../scripts/glad-generate.sh   # default options (can be run anywhere)
  ```

2. Backend (one of):
  1. [SDL](http://libsdl.org/download-2.0.php) >= **2.0.3** into `sdl/`
  2. [GLFW](http://www.glfw.org/download.html) >= **3.2** into `glfw/`

### lib/game

`lib/game` has no manual dependencies.
