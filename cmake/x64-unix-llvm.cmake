set(CMAKE_SYSTEM_NAME Linux)

# Compilers
set(CMAKE_C_COMPILER clang CACHE STRING "")
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "")

# Linker
set(CMAKE_LINKER ld.lld CACHE STRING "")
set(CMAKE_CXX_LINKER ld.lld CACHE STRING "")
set(CMAKE_C_LINKER ld.lld CACHE STRING "")

# libc++
set(CMAKE_CXX_FLAGS "-stdlib=libc++" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++ -lc++abi" CACHE STRING "")
