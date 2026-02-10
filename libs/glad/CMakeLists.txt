cmake_minimum_required(VERSION 3.28)

add_library(glad SHARED "${CMAKE_CURRENT_SOURCE_DIR}/src/gl.c" "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/gl.h"  "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h")
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Export symbols for dynamic loading
set_target_properties(glad PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET default
)
