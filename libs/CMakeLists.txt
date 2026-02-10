include(FetchContent)
include(ExternalProject)
add_subdirectory(stb_image)

# GLFW
set(GLFW_BUID_X11 OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# LZ4
add_subdirectory(lz4/build/cmake)

# SDL
## Subsystems 
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_RENDER OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_DIALOG OFF CACHE BOOL "" FORCE)
set(SDL_TRAY OFF CACHE BOOL "" FORCE)

set(SDL_VIDEO ON CACHE BOOL "" FORCE) # Windowing
set(SDL_JOYSTICK ON CACHE BOOL "" FORCE) # Input

## Other
set(SDL_X11 OFF CACHE BOOL "" FORCE)

add_subdirectory(SDL)

# Msdf
###set(MSDFGEN_USE_SKIA OFF)
set(MSDF_ATLAS_BUILD_STANDALONE OFF)
set(MSDF_ATLAS_NO_ARTERY_FONT ON)
set(MSDF_ATLAS_USE_SKIA OFF)
set(MSDF_ATLAS_MSDFGEN_EXTERNAL OFF)
set(MSDF_ATLAS_BUILD_STANDALONE OFF)
add_subdirectory(msdf-atlas-gen)

# Glad
if(APPLE)
    add_subdirectory(glad-osx)
else()
    add_subdirectory(glad)
endif()

# GLM
set(GLM_ENABLE_CXX_20 true)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	8d1fd52e5ab5590e2c81768ace50c72bae28f2ed #refs/tags/1.0.3
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY	https://github.com/gabime/spdlog.git
	GIT_TAG 	79524ddd08a4ec981b7fea76afd08ee05f83755d #refs/tags/1.17.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG master
)
FetchContent_MakeAvailable(flecs)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 347321b27c58d42567e905c715de60ad43a6cb8e
)
FetchContent_MakeAvailable(miniaudio)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        407c905e45ad75fc29bf0f9bb7c5c2fd3475976f) # 12.1.0
FetchContent_MakeAvailable(fmt)
