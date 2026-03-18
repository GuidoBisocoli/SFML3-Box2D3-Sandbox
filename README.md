# SFML3-Box2D3-Sandbox
A Sandbox made with SFML3 and Box2D3, great as a base for a 2D project with physics.

# Section Usage
- Add the following folders to you Additional Include Directories:
  - $(ProjectDir)hdr
  - ...\SFML-3...\include
  - ...\box2d-3...\include
- Add the following folders to your Additional Library Directories:
  - $(ProjectDir).
  - ...\SFML-3...\lib
- Add the following to Additional Dependencies:
  - For Debug: sfml-graphics-d.lib, sfml-window-d.lib, sfml-system-d.lib and box2dd.lib
  - For Release: sfml-graphics.lib, sfml-window.lib, sfml-system.lib and box2d.lib
- Copy the following files to your [Debug/Release]\ created folder after compiling:
  - For Debug: sfml-graphics-d-3.dll, sfml-system-d-3.dll and sfml-window-d-3.dll (found in SFML-3...\bin\)
  - For Release: sfml-graphics-3.dll, sfml-system-3.dll and sfml-window-3.dll (found in SFML-3...\bin\)
- Copy box2dd.lib (found in box2d-3...\build\src\Debug) and box2d.lib (found in box2d-3...\build\src\Release) to your Project folder
