cmake_minimum_required (VERSION 3.31)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API YES)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)

project(Mana DESCRIPTION "Mana, the C++ Engine" VERSION 1.0.0 LANGUAGES CXX C)

# Compiler flags
add_definitions(-DNOMINMAX)

# Depedencies
add_subdirectory(libs)

# Library
add_library(Mana STATIC)
set_property(TARGET Mana PROPERTY CMAKE_CXX_STANDARD 23)
target_compile_features(Mana PUBLIC cxx_std_23)

# Find sources, but set later due to depedency stuff
target_include_directories(Mana PUBLIC src/)
file(GLOB_RECURSE ManaSources
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        )
file(GLOB_RECURSE ManaModules
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cppm"
        )

# Precompiled headers
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/Mana.cppm" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/Macros.h" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_precompile_headers(Mana PRIVATE src/mpch.h)

## Platform
set(support_opengl FALSE)
set(support_glfw FALSE)
set(support_sdl FALSE)
if(APPLE)
    message("Mana OS: OSX")
    set(support_opengl TRUE)
    set(support_glfw TRUE)

    target_compile_definitions(Mana PUBLIC MANAO_OSX)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    target_compile_options(Mana PUBLIC -fexperimental-library)
elseif (UNIX)
    message("Mana OS: UNIX")
    set(support_opengl TRUE)
    set(support_glfw TRUE)
    
    target_compile_definitions(Mana PUBLIC MANAO_UNIX)
endif()

# Debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Mana PUBLIC M_DEBUG)
    target_compile_definitions(Mana PUBLIC M_DEBUG_INFO)

    # For compiling assets
    target_link_libraries(Mana PUBLIC stb_image)
    set(ASSETS_FOLDER "${PROJECT_SOURCE_DIR}/assets/")
    target_compile_definitions(Mana PRIVATE MANA_DEV_ASSETS_FOLER="${ASSETS_FOLDER}")
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_definitions(Mana PUBLIC M_DEBUG_INFO)
    endif()

    target_compile_definitions(Mana PUBLIC M_RELEASE)
endif ()

# Depedencies
if(support_opengl)
    target_compile_definitions(Mana PUBLIC MANAB_OPENGL)
    target_link_libraries(Mana PUBLIC glad)
else()
    list(FILTER ManaSources EXCLUDE REGEX ".*/Backends/OepnGL/*")
endif()

if(support_glfw)
    target_compile_definitions(Mana PUBLIC MANAB_GLFW)
    target_link_libraries(Mana PUBLIC glfw)
else()
    list(FILTER ManaSources EXCLUDE REGEX ".*/Backends/GLFW*.*")
endif()

if(support_sdl)
    target_compile_definitions(Mana PUBLIC MANAB_SDL)
    target_link_libraries(Mana PRIVATE SDL3::SDL3)
    get_target_property(SDL3_INCLUDE_DIRS SDL3::SDL3 INTERFACE_INCLUDE_DIRECTORIES)
    if(NOT SDL3_INCLUDE_DIRS)
        message("SDL3 include directories not found, adding manually. (cheating essentially)")
        target_include_directories(Mana INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/include"
        )
    endif()
else()
    list(FILTER ManaSources EXCLUDE REGEX ".*/Backends/SDL*.*")
endif()

# Link with depedencies
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_link_libraries(Mana PUBLIC spdlog::spdlog glm::glm lz4 fmt::fmt miniaudio msdf-atlas-gen flecs::flecs_static)

# Set source
target_sources(Mana
  PRIVATE
    ${ManaSources}
    ${ManaModules}
  PUBLIC
    FILE_SET CXX_MODULES
    FILES
      ${ManaModules}
)

# Tests
if(DEFINED MANA_ENABLE_TESTS OR PROJECT_IS_TOP_LEVEL)
    add_subdirectory(tests)
endif()
if(DEFINED MANA_ENABLE_DEMOS OR PROJECT_IS_TOP_LEVEL)
    add_subdirectory(demos)
endif()
