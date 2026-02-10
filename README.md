# Mana
A C++23 game engine library. Used in my game project Spellrite.

## Technology / Credits
* C++23 with the LLVM compiler toolcahin
* Custom OpenGL 4.5 2D renderer (using [GLAD2](https://gen.glad.sh/) loader)
* Multiplatform keyboard, mouse and controller support, using [GLFW](https://glfw.org) and [SDL](https://www.libsdl.org/) (depending on platform)
* Audio engine built upon [miniaudio](https://miniaud.io/)
* Archetypal ECS with [flecs](https://www.flecs.dev/) at it's core
* With many thanks to these open source libraries
    * [stb_image](https://github.com/nothings/stb) - For turning PNG's into pixels
    * [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen) & [msdfgen](https://github.com/Chlumsky/msdfgen) - For turning TTF's into pixels
    * [glm](https://github.com/g-truc/glm) - Calculating things
    * [spdlog](https://github.com/gabime/spdlog/) - Logging things
    * [lz4](https://github.com/lz4/lz4) - Compressing things

## What's in this repository
- assets/ - Assets used at runtime by Mana
- cmake/  - Compiler toolchains
- libs/   - Dependencies
- src/    - Source code
- tests/  - W.I.P
- tools/  - Tools for using the project
- MoteCache & out/ - Cache for compiled assets and compiled C++ code

## Building and Running
Look at BUILD.md for instruction about how to build and compile.

